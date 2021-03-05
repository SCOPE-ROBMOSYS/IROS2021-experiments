/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file yarp_action.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 */


#include <yarp_action.h>
#include <yarp/os/LogStream.h>

YARPAction::YARPAction(string name, const NodeConfiguration& config) :
        ActionNodeBase(name, config),
        YARPNode(name, name)
{
    Optional<std::string> msg = getInput<std::string>("carrier");
    if (msg.has_value()) {
        set_carrier(msg.value());
    }

    bool ok = init();

    if(!ok)
    {
       yError() << "Something went wrong in the node init() of " << name;
    }
}

NodeStatus YARPAction::tick()
{
    yDebug() << "Node" << ActionNodeBase::name() << "sending tick to skill";
    return YARPNode::tick();
}

PortsList YARPAction::providedPorts()
{
    // This action has a 2 input ports ("port_name" and "carrier")
    // Any port must have a name. The type is optional.
    return { InputPort<std::string>("port_name"),
             InputPort<std::string>("carrier") };
}

void YARPAction::halt()
{
    yDebug() << "Node" << ActionNodeBase::name() << "sending halt to skill";
    m_bt_request.send_stop();
    // send halt request to server
}
