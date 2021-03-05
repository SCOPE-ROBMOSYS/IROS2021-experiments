/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "MonitorLogComponent.h"

YARP_LOG_COMPONENT(PORTMONITORCARRIER,
                   "yarp.carrier.portmonitor2",
                   yarp::os::Log::TraceType, /* yarp::os::Log::minimumPrintLevel(), */
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)
