/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file main.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 */


#include <iostream>
#include <behaviortree_cpp_v3/behavior_tree.h>
#include <yarp_condition.h>
#include <yarp_action.h>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <yarp/os/LogStream.h>
using namespace std;
using namespace BT;

class FlipFlopSuccessCondition : public ConditionNode
{

public:
    FlipFlopSuccessCondition(const std::string& name) :
        ConditionNode(name, {} )
    {
        setRegistrationID("AlwaysSuccess");
    }

private:
    int n = 0;
    virtual BT::NodeStatus tick() override
    {
        if(n++ < 30)
        {
            cout << "condition true" << endl;
            return NodeStatus::SUCCESS;
        }
        else
        {
            n = 0;
            cout << "condition false" << endl;
            return NodeStatus::FAILURE;
        }
    }
};


int main()
{
    ReactiveSequence seq("ciao");
//    YARPCondition yarp_condion("ConditionName", "/ConditionName_skill/Skill_request/server");
    FlipFlopSuccessCondition ff_condition("FlipFlopCondition");
    YARPAction yarp_action("ActionName", "/goto_skill/Skill_request/server");

    bool ok = yarp_action.init();

    if(!ok)
    {
        yError() << "Something went wrong.";
        return 1;
    }

//    ok = yarp_condion.init();

//    if(!ok)
//    {
//        yError() << "Something went wrong.";
//        return 1;
//    }

//    seq.addChild(&yarp_condion);
    seq.addChild(&ff_condition);
    seq.addChild(&yarp_action);

    while(true)
    {
        seq.executeTick();
        std::this_thread::sleep_for (std::chrono::milliseconds(1000));
    }
    return 0;
}

