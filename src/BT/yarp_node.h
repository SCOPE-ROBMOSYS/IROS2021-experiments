/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file yarp_node.h
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 */


#pragma once

#include <Skill_request.h>
#include <behaviortree_cpp_v3/action_node.h>
#include <yarp/os/RpcClient.h>
#include <Skill_request.h>

using namespace std;
using namespace BT;

class YARPNode //: public LeafNode
{
public:
    YARPNode(string name, string server_port_name, string carrier = "tcp"s);
    NodeStatus tick();
    NodeStatus status() const;
    bool init();
    string name;

    void set_carrier(std::string carrier);

private:
    string m_client_port_name;
    string m_server_port_name;
    string m_carrier;
    yarp::os::RpcClient m_rpc_client;
protected:
    mutable Skill_request m_bt_request; // mutable because status() is const

};
