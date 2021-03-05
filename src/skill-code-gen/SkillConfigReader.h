#ifndef SKILLCONFIGREADER_H
#define SKILLCONFIGREADER_H

#include <QString>
#include <QStringList>

struct SkillConfig
{
    bool specify_port_name_client_attribute;
    QStringList port_name_list;
};

class SkillConfigReader
{
public:
    SkillConfigReader(QString file);

    SkillConfig getConfig();
    bool readConfig();

    QString m_file;
    SkillConfig m_skillConfig;
};

#endif // SKILLCONFIGREADER_H
