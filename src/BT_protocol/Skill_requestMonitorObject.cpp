/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "Skill_requestMonitorObject.h"

#include <Skill_request.h>
#include <SkillAck.h>
// Include also the .cpp file in order to use the internal types
#include <Skill_request.cpp>
#include <SkillAck.cpp>

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Things.h>

namespace {
YARP_LOG_COMPONENT(SKILLREQUESTMONITOR,
                   "scope.carrier.portmonitor.Skill_request",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)
}

bool Skill_requestMonitorObject::create(const yarp::os::Property& options)
{
    sender = options.find("sender_side").asBool();
    if (!sender) {
        yCError(SKILLREQUESTMONITOR) << "Attaching on receiver side is not supported yet.";
        return false;
    }

    source = options.find("source").asString();
    destination = options.find("destination").asString();

    if (!port.open/*Fake*/(source + "/monitor")) {
        return false;
    }

    if (!port.addOutput("/monitor")) {
        return false;
    }

    return true;
}

yarp::os::Things& Skill_requestMonitorObject::update(yarp::os::Things& thing)
{
    std::lock_guard<std::mutex> lock(mutex);

    yCTrace(SKILLREQUESTMONITOR) << "update()";

    yarp::os::Bottle msg;
    msg.addDouble(yarp::os::SystemClock::nowSystem());
    msg.addString(source);
    msg.addString(destination);
    msg.addString("command");
    //msg.addBool(sender);
    auto& bcmd = msg.addList();


#if 0
    if (!sender) {
        yCTrace(SKILLREQUESTMONITOR) << "update() -> receiver";
        const auto* cmd = thing.cast_as<yarp::os::Bottle>();
        if (cmd) {
            yCInfo(SKILLREQUESTMONITOR) << "Received command:" << cmd->toString();
        }
        return thing;
    }
# endif
    yCTrace(SKILLREQUESTMONITOR) << "update() -> sender";
//     yarp::os::Portable::copyPortable(*(thing.getPortWriter()), data);
//     yCInfo(SKILLREQUESTMONITOR) << "Sending command:" << data.toString();

    if (/*const auto* cmd = */thing.cast_as<Skill_request_request_ack_helper>()) {
        yCDebug(SKILLREQUESTMONITOR) << "Sending command 'request_ack'";
        bcmd.addString("request_ack");
    } else if (/*const auto* cmd = */thing.cast_as<Skill_request_send_start_helper>()) {
        yCDebug(SKILLREQUESTMONITOR) << "Sending command 'send_start'";
        bcmd.addString("send_start");
    } else if (/*const auto* cmd = */thing.cast_as<Skill_request_send_stop_helper>()) {
        yCDebug(SKILLREQUESTMONITOR) << "Sending command 'send_stop'";
        bcmd.addString("send_stop");
    } else {
        yCWarning(SKILLREQUESTMONITOR) << "Sending unknown command";
        bcmd.addString("[unknown]");
    }

    yCDebug(SKILLREQUESTMONITOR, "Writing: %s", msg.toString().c_str());
    port.write(msg);

    return thing;
}



yarp::os::Things& Skill_requestMonitorObject::updateReply(yarp::os::Things& thing)
{
    std::lock_guard<std::mutex> lock(mutex);

    yCTrace(SKILLREQUESTMONITOR) << "updateReply()";

    yarp::os::Bottle msg;
    msg.addDouble(yarp::os::SystemClock::nowSystem());
    msg.addString(source);
    msg.addString(destination);
    msg.addString("reply");
    //msg.addBool(sender);
    auto& breply = msg.addList();

#if 0
    if (!sender) {
        yCTrace(SKILLREQUESTMONITOR) << "update() -> sender";
        yAssert(false); // It doesn't work on receiver side yet
//         yarp::os::Portable::copyPortable(*(thing.getPortWriter()), data);
//         yCInfo(SKILLREQUESTMONITOR) << "Sending reply:" << data.toString();
        return thing;
    }
#endif

    yCTrace(SKILLREQUESTMONITOR) << "update() -> receiver";

    // FIXME SkillAckVocab::toString should be static.
    if (const auto* reply = thing.cast_as<Skill_request_request_ack_helper>()) {
        yCDebug(SKILLREQUESTMONITOR) << "Received reply to 'request_ack':" << SkillAckVocab().toString(reply->m_return_helper);
        breply.addString("request_ack");
        breply.addInt32(static_cast<int32_t>(reply->m_return_helper));
    } else if (/*const auto* reply = */thing.cast_as<Skill_request_send_start_helper>()) {
        yCDebug(SKILLREQUESTMONITOR) << "Received reply to 'send_start'";
        breply.addString("send_start");
    } else if (/*const auto* reply = */thing.cast_as<Skill_request_send_stop_helper>()) {
        yCDebug(SKILLREQUESTMONITOR) << "Received reply to 'send_stop'";
        breply.addString("send_stop");
    } else {
        yCWarning(SKILLREQUESTMONITOR) << "Received unknown reply";
        breply.addString("[unknown]");
    }

    yCDebug(SKILLREQUESTMONITOR, "Writing: %s", msg.toString().c_str());
    port.write(msg);

    return thing;
}
