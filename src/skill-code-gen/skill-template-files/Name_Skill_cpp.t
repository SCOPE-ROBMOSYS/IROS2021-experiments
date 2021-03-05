/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include "@KEY_SKILL_NAME@Skill.h"

#include <QTimer>
#include <QDebug>
#include <QTime>
#include <QStateMachine>

@KEY_SKILL_NAME@Skill::@KEY_SKILL_NAME@Skill(std::string name @KEY_CONSTRUCTOR_ATTRIBUTES_p1@) :
        name(std::move(name))@KEY_CONSTRUCTOR_ATTRIBUTES_p2@
{
    stateMachine.setDataModel(&dataModel);
}

bool @KEY_SKILL_NAME@Skill::start()
{
    if (!yarp::os::NetworkBase::checkNetwork()) {
        qWarning("Error! YARP Network is not initialized");
        return false;
    }

    if (!port.open("/" + name + "/BT_rpc/server")) {
        qWarning("Error! Cannot open YARP port");
        return false;
    }

    if(!this->yarp().attachAsServer(port)) {
        qWarning("Error! Could not attach as server");
        return false;
    }

    stateMachine.start();

    return true;
}

SkillAck @KEY_SKILL_NAME@Skill::request_ack()
{
    while (true) {
        for (const auto& state : stateMachine.activeStateNames()) {
@KEY_SKILL_STATES@
        }
    }
}

void @KEY_SKILL_NAME@Skill::send_start()
{
    stateMachine.submitEvent("CMD_START");
}

void @KEY_SKILL_NAME@Skill::send_stop()
{
    stateMachine.submitEvent("CMD_STOP",  QStateMachine::HighPriority);
}
