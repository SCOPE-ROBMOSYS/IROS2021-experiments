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

@INCLUDE_THRIFT_SERVICE@

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

class @KEY_SKILL_NAME@SkillDataModel: public QScxmlCppDataModel
{
    Q_OBJECT
    Q_SCXML_DATAMODEL

public:
    @CONSTRUCTOR@

    bool setup(const QVariantMap& initialDataValues) override;

    yarp::os::Network yarp;

    @KEY_LIST_PORTS_SERVICES@

    @KEY_LIST_PUBLIC_ATTRIBUTES@
};

Q_DECLARE_METATYPE(::@KEY_SKILL_NAME@SkillDataModel*)
