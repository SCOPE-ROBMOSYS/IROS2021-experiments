/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

# pragma once

#include <Skill_request.h>

#include <yarp/os/RpcServer.h>
#include <yarp/os/Network.h>

#include "@KEY_SKILL_NAME@SkillStateMachine.h"
#include "@KEY_SKILL_NAME@SkillDataModel.h"

class @KEY_SKILL_NAME@Skill:
        public Skill_request
{
public:
    @KEY_SKILL_NAME@Skill(std::string name @KEY_CONSTRUCTOR_ATTRIBUTES_p1@);

    bool start();

    SkillAck request_ack() override;
    void send_start() override;
    void send_stop() override;

private:
    std::string name;
    yarp::os::Network yarpnet;
    yarp::os::RpcServer port;
    @KEY_SKILL_NAME@SkillDataModel dataModel;
    @KEY_SKILL_NAME@SkillStateMachine stateMachine;
};
