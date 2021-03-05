/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include "@KEY_SKILL_NAME@SkillDataModel.h"
#include <QDebug>
#include <QTimer>
#include <QScxmlStateMachine>

@ADD_CONSTRUCTOR@

bool @KEY_SKILL_NAME@SkillDataModel::setup(const QVariantMap &initialDataValues)
{
    Q_UNUSED(initialDataValues)

    if (!yarp::os::NetworkBase::checkNetwork()) {
        qWarning("Error! YARP Network is not initialized");
        return false;
    }

@OPEN_PORTS_AND_ATTACH_CLIENTS@

@OPEN_CONNECTIONS_TO_COMPONENTS@

    return true;
}
