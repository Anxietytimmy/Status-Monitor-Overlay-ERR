/*
 * Copyright (c) 2018-2020 Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <mesosphere.hpp>
#include "kern_k_sleep_manager.hpp"
#include "kern_secure_monitor.hpp"

namespace ams::kern::board::nintendo::nx {

    namespace {

        /* Struct representing registers saved on wake/sleep. */
        class SavedSystemRegisters {
            private:
                u64 ttbr0_el1;
                u64 tcr_el1;
                u64 elr_el1;
                u64 sp_el0;
                u64 spsr_el1;
                u64 daif;
                u64 cpacr_el1;
                u64 vbar_el1;
                u64 csselr_el1;
                u64 cntp_ctl_el0;
                u64 cntp_cval_el0;
                u64 cntkctl_el1;
                u64 tpidr_el0;
                u64 tpidrro_el0;
                u64 mdscr_el1;
                u64 contextidr_el1;
                u64 dbgwcrN_el1[16];
                u64 dbgwvrN_el1[16];
                u64 dbgbcrN_el1[16];
                u64 dbgbvrN_el1[16];
                u64 pmccfiltr_el0;
                u64 pmccntr_el0;
                u64 pmcntenset_el0;
                u64 pmcr_el0;
                u64 pmevcntrN_el0[31];
                u64 pmevtyperN_el0[31];
                u64 pmintenset_el1;
                u64 pmovsset_el0;
                u64 pmselr_el0;
                u64 pmuserenr_el0;
            public:
                void Save();
                void Restore() const;
        };

        constexpr s32 SleepManagerThreadPriority = 2;

        /* Globals for sleep/wake. */
        constinit u64 g_sleep_target_cores;
        constinit KLightLock g_request_lock;
        constinit KLightLock g_cv_lock;
        constinit KLightConditionVariable g_cv;
        constinit KPhysicalAddress g_sleep_buffer_phys_addrs[cpu::NumCores];
        alignas(1_KB) constinit u64 g_sleep_buffers[cpu::NumCores][1_KB / sizeof(u64)];
        constinit SavedSystemRegisters g_sleep_system_registers[cpu::NumCores] = {};

        void PowerOnCpu(int core_id, KPhysicalAddress entry_phys_addr, u64 context_id) {
            /* Request the secure monitor power on the core. */
            smc::CpuOn(cpu::MultiprocessorAffinityRegisterAccessor().GetCpuOnArgument() | core_id, GetInteger(entry_phys_addr), context_id);
        }

        void WaitOtherCpuPowerOff() {
            constexpr u64 PmcPhysicalAddress = 0x7000E400;
            constexpr u64 APBDEV_PMC_PWRGATE_STATUS = PmcPhysicalAddress + 0x38;

            constexpr u32 PWRGATE_STATUS_CE123_MASK = ((1u << 3) - 1) << 9;

            u32 value;
            do {
                bool res = smc::ReadWriteRegister(std::addressof(value), APBDEV_PMC_PWRGATE_STATUS, 0, 0);
                MESOSPHERE_ASSERT(res);
            } while ((value & PWRGATE_STATUS_CE123_MASK) != 0);
        }

        void SavedSystemRegisters::Save() {
            /* Save system registers. */
            this->ttbr0_el1     = cpu::GetTtbr0El1();
            this->tcr_el1       = cpu::GetTcrEl1();
            this->tpidr_el0     = cpu::GetTpidrEl0();
            this->elr_el1       = cpu::GetElrEl1();
            this->sp_el0        = cpu::GetSpEl0();
            this->spsr_el1      = cpu::GetSpsrEl1();
            this->daif          = cpu::GetDaif();
            this->cpacr_el1     = cpu::GetCpacrEl1();
            this->vbar_el1      = cpu::GetVbarEl1();
            this->csselr_el1    = cpu::GetCsselrEl1();
            this->cntp_ctl_el0  = cpu::GetCntpCtlEl0();
            this->cntp_cval_el0 = cpu::GetCntpCvalEl0();
            this->cntkctl_el1   = cpu::GetCntkCtlEl1();
            this->tpidrro_el0   = cpu::GetTpidrRoEl0();

            /* Save pmu registers. */
            {
                /* Get and clear pmcr_el0 */
                this->pmcr_el0 = cpu::GetPmcrEl0();
                cpu::SetPmcrEl0(0);
                cpu::EnsureInstructionConsistency();

                /* Save other pmu registers. */
                this->pmuserenr_el0  = cpu::GetPmUserEnrEl0();
                this->pmselr_el0     = cpu::GetPmSelrEl0();
                this->pmccfiltr_el0  = cpu::GetPmcCfiltrEl0();
                this->pmcntenset_el0 = cpu::GetPmCntEnSetEl0();
                this->pmintenset_el1 = cpu::GetPmIntEnSetEl1();
                this->pmovsset_el0   = cpu::GetPmOvsSetEl0();
                this->pmccntr_el0    = cpu::GetPmcCntrEl0();

                switch (cpu::PerformanceMonitorsControlRegisterAccessor(this->pmcr_el0).GetN()) {
                    #define HANDLE_PMU_CASE(N)                                       \
                        case (N+1):                                                  \
                            this->pmevcntrN_el0 [ N ] = cpu::GetPmevCntr##N##El0();  \
                            this->pmevtyperN_el0[ N ] = cpu::GetPmevTyper##N##El0(); \
                        [[fallthrough]]

                    HANDLE_PMU_CASE(30);
                    HANDLE_PMU_CASE(29);
                    HANDLE_PMU_CASE(28);
                    HANDLE_PMU_CASE(27);
                    HANDLE_PMU_CASE(26);
                    HANDLE_PMU_CASE(25);
                    HANDLE_PMU_CASE(24);
                    HANDLE_PMU_CASE(23);
                    HANDLE_PMU_CASE(22);
                    HANDLE_PMU_CASE(21);
                    HANDLE_PMU_CASE(20);
                    HANDLE_PMU_CASE(19);
                    HANDLE_PMU_CASE(18);
                    HANDLE_PMU_CASE(17);
                    HANDLE_PMU_CASE(16);
                    HANDLE_PMU_CASE(15);
                    HANDLE_PMU_CASE(14);
                    HANDLE_PMU_CASE(13);
                    HANDLE_PMU_CASE(12);
                    HANDLE_PMU_CASE(11);
                    HANDLE_PMU_CASE(10);
                    HANDLE_PMU_CASE( 9);
                    HANDLE_PMU_CASE( 8);
                    HANDLE_PMU_CASE( 7);
                    HANDLE_PMU_CASE( 6);
                    HANDLE_PMU_CASE( 5);
                    HANDLE_PMU_CASE( 4);
                    HANDLE_PMU_CASE( 3);
                    HANDLE_PMU_CASE( 2);
                    HANDLE_PMU_CASE( 1);
                    HANDLE_PMU_CASE( 0);

                    #undef HANDLE_PMU_CASE
                    case 0:
                    default:
                        break;
                }
            }

            /* Save debug registers. */
            const u64 dfr0 = cpu::GetIdAa64Dfr0El1();

            this->mdscr_el1      = cpu::GetMdscrEl1();
            this->contextidr_el1 = cpu::GetContextidrEl1();

            /* Save watchpoints. */
            switch (cpu::DebugFeatureRegisterAccessor(dfr0).GetNumWatchpoints()) {
                #define HANDLE_DBG_CASE(N)                                 \
                    case N:                                                \
                        this->dbgwcrN_el1[ N ] = cpu::GetDbgWcr##N##El1(); \
                        this->dbgwvrN_el1[ N ] = cpu::GetDbgWvr##N##El1(); \
                    [[fallthrough]]

                HANDLE_DBG_CASE(15);
                HANDLE_DBG_CASE(14);
                HANDLE_DBG_CASE(13);
                HANDLE_DBG_CASE(12);
                HANDLE_DBG_CASE(11);
                HANDLE_DBG_CASE(10);
                HANDLE_DBG_CASE( 9);
                HANDLE_DBG_CASE( 8);
                HANDLE_DBG_CASE( 7);
                HANDLE_DBG_CASE( 6);
                HANDLE_DBG_CASE( 5);
                HANDLE_DBG_CASE( 4);
                HANDLE_DBG_CASE( 3);
                HANDLE_DBG_CASE( 2);

                #undef HANDLE_DBG_CASE

                case 1:
                    this->dbgwcrN_el1[1] = cpu::GetDbgWcr1El1();
                    this->dbgwvrN_el1[1] = cpu::GetDbgWvr1El1();
                    this->dbgwcrN_el1[0] = cpu::GetDbgWcr0El1();
                    this->dbgwvrN_el1[0] = cpu::GetDbgWvr0El1();
                    [[fallthrough]];
                default:
                    break;
            }

            /* Save breakpoints. */
            switch (cpu::DebugFeatureRegisterAccessor(dfr0).GetNumBreakpoints()) {
                #define HANDLE_DBG_CASE(N)                                 \
                    case N:                                                \
                        this->dbgbcrN_el1[ N ] = cpu::GetDbgBcr##N##El1(); \
                        this->dbgbvrN_el1[ N ] = cpu::GetDbgBvr##N##El1(); \
                    [[fallthrough]]

                HANDLE_DBG_CASE(15);
                HANDLE_DBG_CASE(14);
                HANDLE_DBG_CASE(13);
                HANDLE_DBG_CASE(12);
                HANDLE_DBG_CASE(11);
                HANDLE_DBG_CASE(10);
                HANDLE_DBG_CASE( 9);
                HANDLE_DBG_CASE( 8);
                HANDLE_DBG_CASE( 7);
                HANDLE_DBG_CASE( 6);
                HANDLE_DBG_CASE( 5);
                HANDLE_DBG_CASE( 4);
                HANDLE_DBG_CASE( 3);
                HANDLE_DBG_CASE( 2);

                #undef HANDLE_DBG_CASE

                case 1:
                    this->dbgbcrN_el1[1] = cpu::GetDbgBcr1El1();
                    this->dbgbvrN_el1[1] = cpu::GetDbgBvr1El1();
                    [[fallthrough]];
                default:
                    break;
            }

            this->dbgbcrN_el1[0] = cpu::GetDbgBcr0El1();
            this->dbgbvrN_el1[0] = cpu::GetDbgBvr0El1();
            cpu::EnsureInstructionConsistency();

            /* Clear mdscr_el1. */
            cpu::SetMdscrEl1(0);
            cpu::EnsureInstructionConsistency();
        }

        void SavedSystemRegisters::Restore() const {
            /* Restore debug registers. */
            const u64 dfr0 = cpu::GetIdAa64Dfr0El1();
            cpu::EnsureInstructionConsistency();

            cpu::SetMdscrEl1(0);
            cpu::EnsureInstructionConsistency();

            cpu::SetOslarEl1(0);
            cpu::EnsureInstructionConsistency();

            /* Restore watchpoints. */
            switch (cpu::DebugFeatureRegisterAccessor(dfr0).GetNumWatchpoints()) {
                #define HANDLE_DBG_CASE(N)                              \
                    case N:                                             \
                        cpu::SetDbgWcr##N##El1(this->dbgwcrN_el1[ N ]); \
                        cpu::SetDbgWvr##N##El1(this->dbgwvrN_el1[ N ]); \
                    [[fallthrough]]

                HANDLE_DBG_CASE(15);
                HANDLE_DBG_CASE(14);
                HANDLE_DBG_CASE(13);
                HANDLE_DBG_CASE(12);
                HANDLE_DBG_CASE(11);
                HANDLE_DBG_CASE(10);
                HANDLE_DBG_CASE( 9);
                HANDLE_DBG_CASE( 8);
                HANDLE_DBG_CASE( 7);
                HANDLE_DBG_CASE( 6);
                HANDLE_DBG_CASE( 5);
                HANDLE_DBG_CASE( 4);
                HANDLE_DBG_CASE( 3);
                HANDLE_DBG_CASE( 2);

                #undef HANDLE_DBG_CASE

                case 1:
                    cpu::SetDbgWcr1El1(this->dbgwcrN_el1[1]);
                    cpu::SetDbgWvr1El1(this->dbgwvrN_el1[1]);
                    cpu::SetDbgWcr0El1(this->dbgwcrN_el1[0]);
                    cpu::SetDbgWvr0El1(this->dbgwvrN_el1[0]);
                    [[fallthrough]];
                default:
                    break;
            }

            /* Restore breakpoints. */
            switch (cpu::DebugFeatureRegisterAccessor(dfr0).GetNumBreakpoints()) {
                #define HANDLE_DBG_CASE(N)                              \
                    case N:                                             \
                        cpu::SetDbgBcr##N##El1(this->dbgbcrN_el1[ N ]); \
                        cpu::SetDbgBvr##N##El1(this->dbgbvrN_el1[ N ]); \
                    [[fallthrough]]

                HANDLE_DBG_CASE(15);
                HANDLE_DBG_CASE(14);
                HANDLE_DBG_CASE(13);
                HANDLE_DBG_CASE(12);
                HANDLE_DBG_CASE(11);
                HANDLE_DBG_CASE(10);
                HANDLE_DBG_CASE( 9);
                HANDLE_DBG_CASE( 8);
                HANDLE_DBG_CASE( 7);
                HANDLE_DBG_CASE( 6);
                HANDLE_DBG_CASE( 5);
                HANDLE_DBG_CASE( 4);
                HANDLE_DBG_CASE( 3);
                HANDLE_DBG_CASE( 2);

                #undef HANDLE_DBG_CASE

                case 1:
                    cpu::SetDbgBcr1El1(this->dbgbcrN_el1[1]);
                    cpu::SetDbgBvr1El1(this->dbgbvrN_el1[1]);
                    [[fallthrough]];
                default:
                    break;
            }

            cpu::SetDbgBcr0El1(this->dbgbcrN_el1[0]);
            cpu::SetDbgBvr0El1(this->dbgbvrN_el1[0]);
            cpu::EnsureInstructionConsistency();

            cpu::SetContextidrEl1(this->contextidr_el1);
            cpu::EnsureInstructionConsistency();

            cpu::SetMdscrEl1(this->mdscr_el1);
            cpu::EnsureInstructionConsistency();

            /* Restore pmu registers. */
            cpu::SetPmUserEnrEl0(0);
            cpu::PerformanceMonitorsControlRegisterAccessor().SetEventCounterReset(true).SetCycleCounterReset(true).Store();
            cpu::SetPmOvsClrEl0(static_cast<u64>(static_cast<u32>(~u32())));
            cpu::SetPmIntEnClrEl1(static_cast<u64>(static_cast<u32>(~u32())));
            cpu::SetPmCntEnClrEl0(static_cast<u64>(static_cast<u32>(~u32())));

            switch (cpu::PerformanceMonitorsControlRegisterAccessor(this->pmcr_el0).GetN()) {
                #define HANDLE_PMU_CASE(N)                                    \
                    case (N+1):                                               \
                        cpu::SetPmevCntr##N##El0 (this->pmevcntrN_el0 [ N ]); \
                        cpu::SetPmevTyper##N##El0(this->pmevtyperN_el0[ N ]); \
                    [[fallthrough]]

                HANDLE_PMU_CASE(30);
                HANDLE_PMU_CASE(29);
                HANDLE_PMU_CASE(28);
                HANDLE_PMU_CASE(27);
                HANDLE_PMU_CASE(26);
                HANDLE_PMU_CASE(25);
                HANDLE_PMU_CASE(24);
                HANDLE_PMU_CASE(23);
                HANDLE_PMU_CASE(22);
                HANDLE_PMU_CASE(21);
                HANDLE_PMU_CASE(20);
                HANDLE_PMU_CASE(19);
                HANDLE_PMU_CASE(18);
                HANDLE_PMU_CASE(17);
                HANDLE_PMU_CASE(16);
                HANDLE_PMU_CASE(15);
                HANDLE_PMU_CASE(14);
                HANDLE_PMU_CASE(13);
                HANDLE_PMU_CASE(12);
                HANDLE_PMU_CASE(11);
                HANDLE_PMU_CASE(10);
                HANDLE_PMU_CASE( 9);
                HANDLE_PMU_CASE( 8);
                HANDLE_PMU_CASE( 7);
                HANDLE_PMU_CASE( 6);
                HANDLE_PMU_CASE( 5);
                HANDLE_PMU_CASE( 4);
                HANDLE_PMU_CASE( 3);
                HANDLE_PMU_CASE( 2);
                HANDLE_PMU_CASE( 1);
                HANDLE_PMU_CASE( 0);

                #undef HANDLE_PMU_CASE
                case 0:
                default:
                    break;
            }

            cpu::SetPmUserEnrEl0 (this->pmuserenr_el0);
            cpu::SetPmSelrEl0    (this->pmselr_el0);
            cpu::SetPmcCfiltrEl0 (this->pmccfiltr_el0);
            cpu::SetPmCntEnSetEl0(this->pmcntenset_el0);
            cpu::SetPmIntEnSetEl1(this->pmintenset_el1);
            cpu::SetPmOvsSetEl0  (this->pmovsset_el0);
            cpu::SetPmcCntrEl0   (this->pmccntr_el0);
            cpu::EnsureInstructionConsistency();

            cpu::SetPmcrEl0(this->pmcr_el0);
            cpu::EnsureInstructionConsistency();

            /* Restore system registers. */
            cpu::SetTtbr0El1   (this->ttbr0_el1);
            cpu::SetTcrEl1     (this->tcr_el1);
            cpu::SetTpidrEl0   (this->tpidr_el0);
            cpu::SetElrEl1     (this->elr_el1);
            cpu::SetSpEl0      (this->sp_el0);
            cpu::SetSpsrEl1    (this->spsr_el1);
            cpu::SetDaif       (this->daif);
            cpu::SetCpacrEl1   (this->cpacr_el1);
            cpu::SetVbarEl1    (this->vbar_el1);
            cpu::SetCsselrEl1  (this->csselr_el1);
            cpu::SetCntpCtlEl0 (this->cntp_ctl_el0);
            cpu::SetCntpCvalEl0(this->cntp_cval_el0);
            cpu::SetCntkCtlEl1 (this->cntkctl_el1);
            cpu::SetTpidrRoEl0 (this->tpidrro_el0);
            cpu::EnsureInstructionConsistency();

            /* Invalidate the entire tlb. */
            cpu::InvalidateEntireTlb();
        }

    }

    void KSleepManager::Initialize() {
        /* Create a sleep manager thread for each core. */
        for (size_t core_id = 0; core_id < cpu::NumCores; core_id++) {
            /* Reserve a thread from the system limit. */
            MESOSPHERE_ABORT_UNLESS(Kernel::GetSystemResourceLimit().Reserve(ams::svc::LimitableResource_ThreadCountMax, 1));

            /* Create a new thread. */
            KThread *new_thread = KThread::Create();
            MESOSPHERE_ABORT_UNLESS(new_thread != nullptr);

            /* Launch the new thread. */
            MESOSPHERE_R_ABORT_UNLESS(KThread::InitializeKernelThread(new_thread, KSleepManager::ProcessRequests, reinterpret_cast<uintptr_t>(g_sleep_buffers[core_id]), SleepManagerThreadPriority, static_cast<s32>(core_id)));

            /* Register the new thread. */
            KThread::Register(new_thread);

            /* Run the thread. */
            new_thread->Run();
        }
    }

    void KSleepManager::SleepSystem() {
        /* Ensure device mappings are not modified during sleep. */
        KDevicePageTable::Lock();
        ON_SCOPE_EXIT { KDevicePageTable::Unlock(); };

        /* Request that the system sleep. */
        {
            KScopedLightLock lk(g_request_lock);

            /* Signal the manager to sleep on all cores. */
            {
                KScopedLightLock lk(g_cv_lock);
                MESOSPHERE_ABORT_UNLESS(g_sleep_target_cores == 0);

                g_sleep_target_cores = (1ul << cpu::NumCores) - 1;
                g_cv.Broadcast();

                while (g_sleep_target_cores != 0) {
                    g_cv.Wait(std::addressof(g_cv_lock));
                }
            }
        }
    }

    void KSleepManager::ProcessRequests(uintptr_t buffer) {
        const s32 core_id = GetCurrentCoreId();
        KPhysicalAddress resume_entry_phys_addr = Null<KPhysicalAddress>;

        /* Get the physical addresses we'll need. */
        {
            MESOSPHERE_ABORT_UNLESS(Kernel::GetKernelPageTable().GetPhysicalAddress(std::addressof(g_sleep_buffer_phys_addrs[core_id]), KProcessAddress(buffer)));
            MESOSPHERE_ABORT_UNLESS(Kernel::GetKernelPageTable().GetPhysicalAddress(std::addressof(resume_entry_phys_addr), KProcessAddress(&::ams::kern::board::nintendo::nx::KSleepManager::ResumeEntry)));

        }
        const KPhysicalAddress sleep_buffer_phys_addr = g_sleep_buffer_phys_addrs[core_id];
        const u64 target_core_mask = (1ul << core_id);

        /* Loop, processing sleep when requested. */
        while (true) {
            /* Wait for a request. */
            {
                KScopedLightLock lk(g_cv_lock);
                while (!(g_sleep_target_cores & target_core_mask)) {
                    g_cv.Wait(std::addressof(g_cv_lock));
                }
            }

            /* Perform Sleep/Wake sequence. */
            {
                /* Disable interrupts. */
                KScopedInterruptDisable di;

                /* Save the system registers for the current core. */
                g_sleep_system_registers[core_id].Save();

                /* Change the translation tables to use the kernel table. */
                {
                    /* Get the current value of the translation control register. */
                    const u64 tcr = cpu::GetTcrEl1();

                    /* Disable translation table walks on tlb miss. */
                    cpu::TranslationControlRegisterAccessor(tcr).SetEpd0(true).Store();
                    cpu::EnsureInstructionConsistency();

                    /* Change the translation table base (ttbr0) to use the kernel table. */
                    cpu::SetTtbr0El1(Kernel::GetKernelPageTable().GetIdentityMapTtbr0(core_id));
                    cpu::EnsureInstructionConsistency();

                    /* Enable translation table walks on tlb miss. */
                    cpu::TranslationControlRegisterAccessor(tcr).SetEpd0(false).Store();
                    cpu::EnsureInstructionConsistency();
                }

                /* Invalidate the entire tlb. */
                cpu::InvalidateEntireTlb();

                /* Ensure that all cores get to this point before continuing. */
                cpu::SynchronizeAllCores();

                /* If on core 0, put the device page tables to sleep. */
                if (core_id == 0) {
                    KDevicePageTable::Sleep();
                }

                /* Ensure that all cores get to this point before continuing. */
                cpu::SynchronizeAllCores();

                /* Save the interrupt manager's state. */
                Kernel::GetInterruptManager().Save(core_id);

                /* Ensure that all cores get to this point before continuing. */
                cpu::SynchronizeAllCores();

                /* Log that the core is going to sleep. */
                MESOSPHERE_LOG("Core[%d]: Going to sleep, buffer = %010lx\n", core_id, GetInteger(sleep_buffer_phys_addr));

                /* If we're on a core other than zero, we can just invoke the sleep handler. */
                if (core_id != 0) {
                    CpuSleepHandler(GetInteger(sleep_buffer_phys_addr), GetInteger(resume_entry_phys_addr));
                } else {
                    /* Wait for all other cores to be powered off. */
                    WaitOtherCpuPowerOff();

                    /* Log that we're about to enter SC7. */
                    MESOSPHERE_LOG("Entering SC7\n");

                    /* Save the debug log state. */
                    KDebugLog::Save();

                    /* Invoke the sleep handler. */
                    CpuSleepHandler(GetInteger(sleep_buffer_phys_addr), GetInteger(resume_entry_phys_addr));

                    /* Restore the debug log state. */
                    KDebugLog::Restore();

                    /* Log that we're about to exit SC7. */
                    MESOSPHERE_LOG("Exiting SC7\n");

                    /* Wake up the other cores. */
                    for (s32 i = 1; i < static_cast<s32>(cpu::NumCores); ++i) {
                        PowerOnCpu(i, resume_entry_phys_addr, GetInteger(g_sleep_buffer_phys_addrs[i]));
                    }
                }

                /* Log that the core is waking from sleep. */
                MESOSPHERE_LOG("Core[%d]: Woke from sleep.\n", core_id);

                /* Ensure that all cores get to this point before continuing. */
                cpu::SynchronizeAllCores();

                /* Restore the interrupt manager's state. */
                Kernel::GetInterruptManager().Restore(core_id);

                /* Ensure that all cores get to this point before continuing. */
                cpu::SynchronizeAllCores();

                /* If on core 0, wake up the device page tables. */
                if (core_id == 0) {
                    KDevicePageTable::Wakeup();
                }

                /* Ensure that all cores get to this point before continuing. */
                cpu::SynchronizeAllCores();

                /* Restore the system registers for the current core. */
                g_sleep_system_registers[core_id].Restore();
            }

            /* Signal request completed. */
            {
                KScopedLightLock lk(g_cv_lock);
                g_sleep_target_cores &= ~target_core_mask;
                if (g_sleep_target_cores == 0) {
                    g_cv.Broadcast();
                }
            }
        }
    }

}