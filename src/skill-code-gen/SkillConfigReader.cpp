/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
#include "SkillConfigReader.h"

#include <QDebug>
#include <QFile>

SkillConfigReader::SkillConfigReader(QString file) :
        m_file(std::move(file))
{
}

//used to return the value outside
SkillConfig SkillConfigReader::getConfig()
{
    readConfig();
    return m_skillConfig;
}

bool SkillConfigReader::readConfig()
{
    QFile file(m_file);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Couldn't open config file for reading.";
        return false;
    }
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (line.contains("port_name_client_attribute")) {
            // only if specified
            m_skillConfig.specify_port_name_client_attribute = true;

            // detect only the delimiter because it returns the start
            // position of the searched string
            auto detectedPosition = line.indexOf(':');
            if (detectedPosition != -1) {
                auto key_port_name = line.left(detectedPosition);
                auto value_port_name = line.mid(detectedPosition + 1);
                m_skillConfig.port_name_list.push_back(value_port_name);
            }
        } else {
            m_skillConfig.specify_port_name_client_attribute = false;
        }
    }

    return true;
}
