/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file yarp_action.h
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 */

#pragma once


#include <yarp_node.h>
#include <string>

class YARPAction :  public ActionNodeBase, public YARPNode
        // public virtual ActionNodeBase because the BT factory accepts only classes that explicitly inherits from ActionNodeBase or ConditionNode
{
public:
    YARPAction(string name, const NodeConfiguration &config);
    void halt() override;
    NodeStatus tick() override;

    static PortsList providedPorts();
};

