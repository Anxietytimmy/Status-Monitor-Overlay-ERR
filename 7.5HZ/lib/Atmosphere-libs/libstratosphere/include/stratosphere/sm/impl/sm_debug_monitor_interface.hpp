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

#pragma once
#include <vapours.hpp>
#include <stratosphere/sm/sm_types.hpp>
#include <stratosphere/sf.hpp>

namespace ams::sm::impl {

    #define AMS_SM_I_DEBUG_MONITOR_INTERFACE_INTERFACE_INFO(C, H)                                                                                          \
        AMS_SF_METHOD_INFO(C, H, 65000, Result, AtmosphereGetRecord,     (sf::Out<ServiceRecord> record, ServiceName service))                             \
        AMS_SF_METHOD_INFO(C, H, 65001, void,   AtmosphereListRecords,   (const sf::OutArray<ServiceRecord> &records, sf::Out<u64> out_count, u64 offset)) \
        AMS_SF_METHOD_INFO(C, H, 65002, void,   AtmosphereGetRecordSize, (sf::Out<u64> record_size))

    AMS_SF_DEFINE_INTERFACE(IDebugMonitorInterface, AMS_SM_I_DEBUG_MONITOR_INTERFACE_INTERFACE_INFO)

}
