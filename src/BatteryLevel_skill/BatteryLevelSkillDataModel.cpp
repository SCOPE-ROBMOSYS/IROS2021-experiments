/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include "BatteryLevelSkillDataModel.h"
#include <QDebug>
#include <QTimer>
#include <QScxmlStateMachine>



bool BatteryLevelSkillDataModel::setup(const QVariantMap &initialDataValues)
{
    Q_UNUSED(initialDataValues)

    if (!yarp::os::NetworkBase::checkNetwork()) {
        qWarning("Error! YARP Network is not initialized");
        return false;
    }

    // open ports

    if (!client_port_batteryReader.open("/BatteryReaderBatteryLevelClient")) {
        qWarning("Error! Cannot open YARP port with command: client_port_batteryReader.open(/BatteryReaderClient) " );
        return false;
    }

    // attach services as clients

    if(!batteryReader.yarp().attachAsClient(client_port_batteryReader)) {
        qWarning("Error! Could not attach as client with command : batteryReader.yarp().attachAsClient(client_port_batteryReader) "  );
        return false;
}


    // open connections to components

    if (!yarp::os::Network::connect(client_port_batteryReader.getName(), "/BatteryComponent", "tcp+send.portmonitor2+file.BatteryReader+type.dll")) {
        qWarning("Error! Could not connect to server : /BatteryComponent " );
        return false;
    }


    return true;
}
