/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#ifndef SKILLGENERATOR_H
#define SKILLGENERATOR_H

#include "SkillConfigReader.h"

#include "qscxmlcompiler_p.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QTextCodec>
#include <QTextStream>
#include <QXmlStreamReader>

struct TranslationUnit
{
    QString Path;
    QString scxmlFileName;
    QString namespaceName;
    bool stateMethods {false};
    DocumentModel::ScxmlDocument* mainDocument {nullptr};
    QList<DocumentModel::ScxmlDocument*> allDocuments;
    QHash<DocumentModel::ScxmlDocument*, QString> classnameForDocument;
    QList<TranslationUnit*> dependencies;
};

enum
{
    NoError = 0,
    CommandLineArgumentsError = -1,
    NoInputFilesError = -2,
    CannotOpenInputFileError = -3,
    ParseError = -4,
    CannotOpenOutputHeaderFileError = -5,
    CannotOpenOutputCppFileError = -6,
    ScxmlVerificationError = -7,
    NoTextCodecError = -8
};

struct State
{
    QString id;
    int SkillAckEnum;
    QString SkillAck;
};

struct Service
{
    QString name_instance;
    QString service_type;
    QString connect_type;
    QString thrift_protocol;
    QString service_function;
    QString port_name_client_attribute;
    QString port_name_server;
};

struct Attribute
{
    QString data_type;
    QString name_instance;
    QString value;
    QString init_source; // if "define_in_skill_class_constructor" then it will be added in the constructor
};

struct ClientPort
{ // not really needed
    QString client_port_name;
};

struct SkillDescription
{
    QString skill_name;
    QString skill_version;
    QString path_skill_folder;
    QList<State> ListStates;
    QStringList UsedComponents;
    QList<Service> UsedServices;
    QList<Attribute> UsedAttributes;
    QList<ClientPort> UsedClientPorts;
    bool specify_port_name_client_attribute;
    bool add_default_constructor {false};
    QList<Attribute> ListAttributesParsedAsOption;
};

struct Keys
{
    QString key_skill_name {"@KEY_SKILL_NAME@"};
    QString key_skill_version {"@KEY_SKILL_VERSION@"};
    QString KEY_CONSTRUCTOR_ATTRIBUTES_p1 {"@KEY_CONSTRUCTOR_ATTRIBUTES_p1@"};
};

struct Values
{
    QString empty;
    QString value_CONSTRUCTOR_ATTRIBUTES_p1_with_comma;
};

class SkillGenerator
{
public:
    SkillGenerator(TranslationUnit* tu);
    int init();

private:
    QString m_templatePath;
    TranslationUnit* m_translationUnit;
    SkillDescription m_skillDescription;
    Keys m_keys;
    Values m_values;
    SkillConfig m_skillConfig;

    void replaceKeyInsideTemplate(QString path_new_skill, QFile& input_file, QFile& output_file, QRegularExpression key, QString value);
    static QString decoderEnum(int id);

    static QStringList generateStringList_name_instance(const QList<Attribute>& ListAttributes);
    static QStringList generateStringList_data_type(const QList<Attribute>& ListAttributes);

    static QString generateListConstructorParameters(const QStringList& ListParamToAssign_data_type,
                                                     const QStringList& ListParamToAssign_name_instance);
    static QString generateListConstructorParametersAssign(const QStringList& ListMemberAttributes,
                                                           const QStringList& ListParamToAssign);
    static QString generateListConstructorParametersAssignParsedAsOption(const QStringList& ListParamToAssign);
    static QString generateListConstructorParametersPassArgs(const QStringList& ListParamToAssign_name_instance);

    int write();
    void configGeneration();
    void generate_Main();
    void generate_Skill_h();
    void generate_Skill_cpp();
    void generate_Skill_DataModel_h();
    void generate_Skill_DataModel_cpp();
};

#endif // SKILLGENERATOR_H
