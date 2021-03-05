/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#pragma once

#include <QScxmlCppDataModel>
#include <QVariantMap>

#include <QTime>
#include <QTimer>
#include <QDebug>

#include <yarp/os/Network.h>
#include <yarp/os/RpcClient.h>

#include "BatteryReader.h" 
 

struct Connector
{
    Connector(std::string from, std::string to, std::string carrier) :
            m_from(std::move(from)),
            m_to(std::move(to)),
            m_carrier(std::move(carrier))
    {
        if (!yarp::os::Network::connect(m_from, m_to, carrier)) {
            qFatal("Error! Could not connect to server /fakeBattery");
        }
    }

    ~Connector()
    {
        if (!yarp::os::Network::disconnect(m_from, m_to)) {
            qFatal("Error! Could not disconnect from server /fakeBattery");
        }
    }

    std::string m_from;
    std::string m_to;
    std::string m_carrier;
};

class BatteryLevelSkillDataModel: public QScxmlCppDataModel
{
    Q_OBJECT
    Q_SCXML_DATAMODEL

public:
    BatteryLevelSkillDataModel() = default;

    bool setup(const QVariantMap& initialDataValues) override;

    yarp::os::Network yarp;

    yarp::os::RpcClient client_port_batteryReader;
    

    BatteryReader batteryReader;
    double level { 100.0 };
    
};

Q_DECLARE_METATYPE(::BatteryLevelSkillDataModel*)
