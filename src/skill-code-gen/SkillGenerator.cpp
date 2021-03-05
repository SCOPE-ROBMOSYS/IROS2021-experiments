/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include "SkillGenerator.h"

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


SkillGenerator::SkillGenerator(TranslationUnit* translationUnit) :
        m_translationUnit(translationUnit)
{
    //    init(); called from main
}

int SkillGenerator::init()
{

    configGeneration();
    int result = write();

    return result;
}

void SkillGenerator::configGeneration()
{
    // default constructor
    m_skillDescription.add_default_constructor = false;

    // used only for .conf files
    // **********************************++ !path & new folder **********************************++
    //detect root path
    QString ss_whole = m_translationUnit->Path.toUtf8().constData(); //receive from outside

    // 1) search the word "skill" inside the string
    // 2) fetch string before that.
    QString path_root_package = ss_whole.left(ss_whole.lastIndexOf("src")); // i.e. /home/scope/bt-implementation/

    // locate folder with template files
    // e.g. /home/scope/bt-implementation/src/skill-code-gen/skill-template-files/
    m_templatePath = path_root_package + "src/skill-code-gen/skill-template-files/";

    //detect skill name
    QString ss = m_translationUnit->scxmlFileName.toUtf8().constData();
    QString str_first_part = ss.left(ss.indexOf("SkillStateMachine.scxml"));
    //cout << "Skill name --> " << str_first_part << "\n\n";
    m_skillDescription.skill_name = str_first_part;

    // **********************************++ !read config **********************************++
    QString file_with_path = path_root_package + "src/skill-code-gen/input-files/config-skills/";
    file_with_path = file_with_path + str_first_part + "Skill.conf";
    SkillConfigReader m_skillConfigReader(file_with_path);
    m_skillConfig = m_skillConfigReader.getConfig();
}

QString SkillGenerator::decoderEnum(int id)
{
    // enum BT_Status { Running, Idle, Success, Failure, Undefined };
    switch (id) {
    case 0:
        return "UNDEFINED";
    case 1:
        return "IDLE";
    case 2:
        return "SUCCESS";
    case 3:
        return "FAILURE";
    case 4:
        return "RUNNING";
    default:
        return "ERROR_IN_SCXML";
    }
}

QStringList SkillGenerator::generateStringList_name_instance(const QList<Attribute>& ListAttributes)
{
    QStringList output;
    for (auto& ListAttribute : ListAttributes) {
        output.push_back(ListAttribute.name_instance);
    }
    return output;
}

QStringList SkillGenerator::generateStringList_data_type(const QList<Attribute>& ListAttributes)
{
    QStringList output;
    for (auto& ListAttribute : ListAttributes) {
        output.push_back(ListAttribute.data_type);
    }
    return output;
}

QString SkillGenerator::generateListConstructorParametersPassArgs(const QStringList& ListParamToAssign_name_instance)
{
    QString output;
    for (int i = 0; i < ListParamToAssign_name_instance.size(); i++) {
        output = output + ListParamToAssign_name_instance[i];
        output = output + "";
        if (i != (ListParamToAssign_name_instance.size() - 1)) {
            output = output + ", ";
        }
    }
    return output;
}

QString SkillGenerator::generateListConstructorParameters(const QStringList& ListParamToAssign_data_type,
                                                          const QStringList& ListParamToAssign_name_instance)
{
    QString output;
    for (int i = 0; i < ListParamToAssign_data_type.size(); i++) {
        output = output + ListParamToAssign_data_type[i] + " " + ListParamToAssign_name_instance[i];
        if (i != (ListParamToAssign_data_type.size() - 1)) {
            output = output + ", ";
        }
    }
    return output;
}

QString SkillGenerator::generateListConstructorParametersAssign(const QStringList& ListMemberAttributes,
                                                                const QStringList& ListParamToAssign)
{
    QString output;
    for (int i = 0; i < ListMemberAttributes.size(); i++) {
        output = output + ListMemberAttributes[i] + "(std::move(" + ListParamToAssign[i] + "))";
        if (i != (ListMemberAttributes.size() - 1)) {
            output = output + ", ";
        }
    }
    return output;
}

QString SkillGenerator::generateListConstructorParametersAssignParsedAsOption(const QStringList& ListParamToAssign)
{
    QString output = "dataModel("; //name of the object instance of type GoToSkillDataModel (that takes in input all the list of params)
    for (int i = 0; i < ListParamToAssign.size(); i++) {
        output = output + "std::move(" + ListParamToAssign[i] + ")"; // insert in the constructor of datamodel object all the parameters parsed as option from docker-compose.yml
        if (i != (ListParamToAssign.size() - 1)) {
            output = output + ", ";
        } else {
            output = output + ")";
        }
    }
    return output;
}


void SkillGenerator::generate_Main()
{
    QFile template_file(":/skill-template-files/main_cpp.t"); //    QFile template_file (m_templatePath + "main_cpp.t");

    // 1: open and read the template
    template_file.open(QIODevice::Text | QIODevice::ReadOnly);
    QString dataText = template_file.readAll();

    // 2: replace

    // 2.1: skill name and version
    dataText.replace(m_keys.key_skill_name, m_skillDescription.skill_name);
    dataText.replace(m_keys.key_skill_version, m_skillDescription.skill_version);

    // 2.2 @KEY_CONSTRUCTOR_ATTRIBUTES_p1_PASSED_ARGS@
    QRegularExpression KEY_CONSTRUCTOR_ATTRIBUTES_p1_PASSED_ARGS(" @KEY_CONSTRUCTOR_ATTRIBUTES_p1_PASSED_ARGS@");
    QString attrib_1_pass_args;
    if (m_skillDescription.add_default_constructor == true) {
        QStringList ListParamToAssign_name_instance = generateStringList_name_instance(m_skillDescription.ListAttributesParsedAsOption);
        attrib_1_pass_args = generateListConstructorParametersPassArgs(ListParamToAssign_name_instance);
    }
    QString attrib_1_pass_args_with_comma = ", " + attrib_1_pass_args;
    QString value_CONSTRUCTOR_ATTRIBUTES_p1_PASS_ARGS_with_comma = attrib_1_pass_args_with_comma;
    if (!m_skillDescription.ListAttributesParsedAsOption.empty()) { // otherwise insert a not needed comma
        dataText.replace(KEY_CONSTRUCTOR_ATTRIBUTES_p1_PASSED_ARGS, value_CONSTRUCTOR_ATTRIBUTES_p1_PASS_ARGS_with_comma);
    } else {
        dataText.replace(KEY_CONSTRUCTOR_ATTRIBUTES_p1_PASSED_ARGS, m_values.empty);
    }

    // 2.3 @KEY_ADDITIONAL_OPTION_PARSE@
    QRegularExpression KEY_ADDITIONAL_OPTION_PARSE("@KEY_ADDITIONAL_OPTION_PARSE@");
    QString all_instances_options;
    for (auto& i : m_skillDescription.ListAttributesParsedAsOption) {
        QString single_instance = "    parser.addOption({\""
                                + i.name_instance
                                + "\", \"The <"
                                + i.name_instance
                                + "> attribute.\", \""
                                + i.name_instance
                                + "\", \""
                                + m_skillDescription.skill_name
                                + "\"});\n";
        all_instances_options = all_instances_options + single_instance;
    }
    QString value_ADDITIONAL_OPTION_PARSE = all_instances_options;
    dataText.replace(KEY_ADDITIONAL_OPTION_PARSE, value_ADDITIONAL_OPTION_PARSE);

    // 2.4 @KEY_ADDITIONAL_OPTION_ASSIGN@
    QRegularExpression KEY_ADDITIONAL_OPTION_ASSIGN("@KEY_ADDITIONAL_OPTION_ASSIGN@");
    QString all_instances_options_assign;
    for (auto& i : m_skillDescription.ListAttributesParsedAsOption) {
        QString single_instance = "    std::string " + i.name_instance + " = parser.value(\"" + i.name_instance + "\").toStdString();\n";
        all_instances_options_assign = all_instances_options_assign + single_instance;
    }
    QString value_ADDITIONAL_OPTION_ASSIGN = all_instances_options_assign;
    dataText.replace(KEY_ADDITIONAL_OPTION_ASSIGN, value_ADDITIONAL_OPTION_ASSIGN);

    // 2.4:  KEY_LIST_PUBLIC_ATTRIBUTES_main        REMOVED
    // QRegularExpression  KEY_LIST_PUBLIC_ATTRIBUTES_main("@KEY_LIST_PUBLIC_ATTRIBUTES_main@");
    // QString all_instances_main;
    // // list of attributes (without value assigned)
    // for(unsigned int i=0; i<m_skillDescription.UsedAttributes.size(); i++){
    //     QString single_instance;
    //     if( m_skillDescription.UsedAttributes[i].init_source != "initialize_inside_header") {
    //         single_instance = "" + m_skillDescription.UsedAttributes[i].data_type + " " + m_skillDescription.UsedAttributes[i].name_instance + ";\n    " ;
    //     }
    //     all_instances_main = all_instances_main + single_instance;
    // }
    // QString value_LIST_PUBLIC_ATTRIBUTES_main = all_instances_main;
    // dataText.replace( KEY_LIST_PUBLIC_ATTRIBUTES_main, value_LIST_PUBLIC_ATTRIBUTES_main);

    // 3: create new file and insert the dataText
    QFile output_file(m_skillDescription.path_skill_folder + "main.cpp");
    if (output_file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&output_file);
        out << dataText;
    }
    output_file.close();
}

void SkillGenerator::generate_Skill_h()
{
    QFile template_file(":/skill-template-files/Name_Skill_h.t");

    // 1: open and read the template
    template_file.open(QIODevice::Text | QIODevice::ReadOnly);
    QString dataText = template_file.readAll();

    // 2: replace

    // 2.1: name
    dataText.replace(m_keys.key_skill_name, m_skillDescription.skill_name);

    // 2.2 @KEY_CONSTRUCTOR_ATTRIBUTES_p1@
    QRegularExpression KEY_CONSTRUCTOR_ATTRIBUTES_p1(" @KEY_CONSTRUCTOR_ATTRIBUTES_p1@");
    QString attrib_1;
    if (m_skillDescription.add_default_constructor == true) {
        QStringList ListParamToAssign_data_type = generateStringList_data_type(m_skillDescription.ListAttributesParsedAsOption);
        QStringList ListParamToAssign_name_instance = generateStringList_name_instance(m_skillDescription.ListAttributesParsedAsOption);
        attrib_1 = generateListConstructorParameters(ListParamToAssign_data_type, ListParamToAssign_name_instance);
    }
    QString attrib_1_with_comma = ", " + attrib_1;
    QString value_CONSTRUCTOR_ATTRIBUTES_p1_with_comma = attrib_1_with_comma;
    m_values.value_CONSTRUCTOR_ATTRIBUTES_p1_with_comma = value_CONSTRUCTOR_ATTRIBUTES_p1_with_comma; // assign the global value to exploit the value globally
    if (!m_skillDescription.ListAttributesParsedAsOption.empty()) {                                            // otherwise insert a not needed comma
        dataText.replace(m_keys.KEY_CONSTRUCTOR_ATTRIBUTES_p1, value_CONSTRUCTOR_ATTRIBUTES_p1_with_comma);
    } else {
        dataText.replace(m_keys.KEY_CONSTRUCTOR_ATTRIBUTES_p1, m_values.empty);
    }

    // 3: create new file and insert the dataText
    QFile output_file(m_skillDescription.path_skill_folder + m_skillDescription.skill_name + "Skill.h");
    if (output_file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&output_file);
        out << dataText;
    }
    output_file.close();
}

void SkillGenerator::generate_Skill_cpp()
{
    QFile template_file(":/skill-template-files/Name_Skill_cpp.t");

    // 1: open and read the template
    template_file.open(QIODevice::Text | QIODevice::ReadOnly);
    QString dataText = template_file.readAll();

    // 2: replace

    // 2.1: name
    dataText.replace(m_keys.key_skill_name, m_skillDescription.skill_name);

    // 2.2: states
    QRegularExpression key_states("@KEY_SKILL_STATES@");
    QString all;
    for (auto& ListState : m_skillDescription.ListStates) {
        auto actual_state = ListState.id;
        QString actual_state_string = actual_state.toUtf8().constData();

        if (actual_state_string != "wrapper") {
            QString result_ack = ListState.SkillAck;
            QString single_state_condit =
                "            if (state == \"" + actual_state_string + "\") {\n"
                "                stateMachine.submitEvent(\"REQUEST_ACK\");\n"
                "                return SKILL_" + result_ack + ";\n"
                "            }\n";
            all = all + single_state_condit;
        }
    }
#ifdef _DEBUG
    cout << all;
#endif
    QString value_all_states_condit = all;
    dataText.replace(key_states, value_all_states_condit);

    // 2.3 @KEY_CONSTRUCTOR_ATTRIBUTES_p1@ + @KEY_CONSTRUCTOR_ATTRIBUTES_p2@

    QRegularExpression KEY_CONSTRUCTOR_ATTRIBUTES_p1(" @KEY_CONSTRUCTOR_ATTRIBUTES_p1@");
    if (!m_skillDescription.ListAttributesParsedAsOption.empty()) {
        dataText.replace(m_keys.KEY_CONSTRUCTOR_ATTRIBUTES_p1, m_values.value_CONSTRUCTOR_ATTRIBUTES_p1_with_comma);
    } else {
        dataText.replace(m_keys.KEY_CONSTRUCTOR_ATTRIBUTES_p1, m_values.empty);
    }

    QRegularExpression KEY_CONSTRUCTOR_ATTRIBUTES_p2("@KEY_CONSTRUCTOR_ATTRIBUTES_p2@");
    QString attrib_2;
    if (m_skillDescription.add_default_constructor == true) {
        QString space = ",\n        ";
        attrib_2 = attrib_2 + space;
        QStringList ListParamToAssign = generateStringList_name_instance(m_skillDescription.ListAttributesParsedAsOption);
        attrib_2 = attrib_2 + generateListConstructorParametersAssignParsedAsOption(ListParamToAssign);
    }
    QString value_CONSTRUCTOR_ATTRIBUTES_p2 = attrib_2;
    dataText.replace(KEY_CONSTRUCTOR_ATTRIBUTES_p2, value_CONSTRUCTOR_ATTRIBUTES_p2);

    // 3: create new file and insert the dataText
    QFile output_file(m_skillDescription.path_skill_folder + m_skillDescription.skill_name + "Skill.cpp");
    if (output_file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&output_file);
        out << dataText;
    }
    output_file.close();
}

void SkillGenerator::generate_Skill_DataModel_h()
{
    QFile template_file(":/skill-template-files/Name_SkillDataModel_h.t");

    // 1: open and read the template
    template_file.open(QIODevice::Text | QIODevice::ReadOnly);
    QString dataText = template_file.readAll();

    // 2: replace

    // 2.1: name
    dataText.replace(m_keys.key_skill_name, m_skillDescription.skill_name);

    // 2.1.2 #include @INCLUDE_THRIFT_SERVICE@
    QRegularExpression KEY_INCLUDE_THRIFT_SERVICE("@INCLUDE_THRIFT_SERVICE@");
    QString str_include;
    for (auto& UsedService : m_skillDescription.UsedServices) {
        str_include = str_include + "#include \"" + UsedService.service_type + ".h\" \n ";
        // with < >
        //str_include = str_include + "#include <" + m_skillDescription.UsedServices[i].service_type + ".h> \n ";
    }
    QString value_INCLUDE_THRIFT_SERVICE = str_include;
    dataText.replace(KEY_INCLUDE_THRIFT_SERVICE, value_INCLUDE_THRIFT_SERVICE);

    // 2.2.1: @KEY_LIST_PORTS_SERVICES@
    QRegularExpression KEY_LIST_PORTS_SERVICES("@KEY_LIST_PORTS_SERVICES@");
    QString all_instances_ports;
    // list of services
    for (auto& UsedService : m_skillDescription.UsedServices) {
        // e.g. yarp::os::RpcClient client_port_NAME;
        QString single_instance = "yarp::os::RpcClient client_port_" + UsedService.name_instance + ";\n    ";
        all_instances_ports = all_instances_ports + single_instance;
    }
    QString value_LIST_PORTS_SERVICES = all_instances_ports;
    dataText.replace(KEY_LIST_PORTS_SERVICES, value_LIST_PORTS_SERVICES);

    // 2.2.2: KEY_LIST_PUBLIC_ATTRIBUTES
    QRegularExpression KEY_LIST_PUBLIC_ATTRIBUTES("@KEY_LIST_PUBLIC_ATTRIBUTES@");
    QString all_instances;
    // list of services
    for (auto& UsedService : m_skillDescription.UsedServices) {
        QString single_instance = "" + UsedService.service_type + " " + UsedService.name_instance + ";\n    ";
        all_instances = all_instances + single_instance;
    }

    // list of attributes (with value assigned)
    for (auto& UsedAttribute : m_skillDescription.UsedAttributes) {
        QString single_instance;
        if (UsedAttribute.init_source == "initialize_inside_header") {
            single_instance = "" + UsedAttribute.data_type + " " + UsedAttribute.name_instance + " { " + UsedAttribute.value + " };\n    ";
        } else if (UsedAttribute.init_source == "not_initialize") {
            single_instance = "";
        } else {
            single_instance = "" + UsedAttribute.data_type + " " + UsedAttribute.name_instance + ";\n    ";
        }
        all_instances = all_instances + single_instance;
    }

    //    cout << "\n\n\PRINT COMPONENTS --> all_instances : " << all_instances << "\n\n\ " ;
    QString value_LIST_PUBLIC_ATTRIBUTES = all_instances;
    dataText.replace(KEY_LIST_PUBLIC_ATTRIBUTES, value_LIST_PUBLIC_ATTRIBUTES);

    // 2.3 @CONSTRUCTOR@
    QRegularExpression KEY_CONSTRUCTOR("@CONSTRUCTOR@");
    QString construct;
    if (m_skillDescription.add_default_constructor == false) { // it is also used in .cpp to insert the not default constructor
        construct = construct + m_skillDescription.skill_name + "SkillDataModel() = default;";
    } else { // e.g.  GoToSkillDataModel(QString location);
        QStringList ListParamToAssign_data_type = generateStringList_data_type(m_skillDescription.ListAttributesParsedAsOption);
        QStringList ListParamToAssign_name_instance = generateStringList_name_instance(m_skillDescription.ListAttributesParsedAsOption);
        QString params = generateListConstructorParameters(ListParamToAssign_data_type, ListParamToAssign_name_instance);
        construct = construct + m_skillDescription.skill_name + "SkillDataModel(" + params + ");";
    }

    QString value_CONSTRUCTOR = construct;
    dataText.replace(KEY_CONSTRUCTOR, value_CONSTRUCTOR);

    // 3: create new file and insert the dataText
    QFile output_file(m_skillDescription.path_skill_folder + m_skillDescription.skill_name + "SkillDataModel.h");
    if (output_file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&output_file);
        out << dataText;
    }
    output_file.close();
}

void SkillGenerator::generate_Skill_DataModel_cpp()
{
    QFile template_file(":/skill-template-files/Name_SkillDataModel_cpp.t");

    // 1: open and read the template
    template_file.open(QIODevice::Text | QIODevice::ReadOnly);
    QString dataText = template_file.readAll();

    // 2: replace

    // 2.1: name
    dataText.replace(m_keys.key_skill_name, m_skillDescription.skill_name);

    // 2.2  @OPEN_PORTS_AND_ATTACH_CLIENTS@
    QRegularExpression KEY_OPEN_PORTS_AND_ATTACH_CLIENTS("@OPEN_PORTS_AND_ATTACH_CLIENTS@");

    QString all_ports;
    QString all_clients;
    QString single_port;
    for (auto& UsedService : m_skillDescription.UsedServices) {
        if (UsedService.port_name_client_attribute != "default") { // if(m_skillConfig.specify_port_name_client_attribute)
            // additional name spec if needed
            QString port_name_attribute = UsedService.port_name_client_attribute; // assume that the specifier is always the first option
            single_port =
                "    if (!client_port_" + UsedService.name_instance + ".open(\"/" + UsedService.service_type + m_skillDescription.skill_name + "Client/\" + " + port_name_attribute + ")) {\n"
                "        qWarning(\"Error! Cannot open YARP port with command: client_port_"
                            + UsedService.name_instance + ".open(/" + UsedService.service_type + "Client/" + port_name_attribute + ") \" );\n"
                "        return false;\n"
                "    }\n\n";
        } else {
            single_port =
                "    if (!client_port_" + UsedService.name_instance + ".open(\"/" + UsedService.service_type + m_skillDescription.skill_name + "Client\")) {\n"
                "        qWarning(\"Error! Cannot open YARP port with command: client_port_"
                            + UsedService.name_instance + ".open(/" + UsedService.service_type + "Client) \" );\n"
                "        return false;\n"
                "    }\n\n";
        }
        all_ports = all_ports + single_port;

        QString single_client =
            "    if(!" + UsedService.name_instance + ".yarp().attachAsClient(client_port_" + UsedService.name_instance + ")) {\n"
            "        qWarning(\"Error! Could not attach as client with command : "
                        + UsedService.name_instance + ".yarp().attachAsClient(client_port_" + UsedService.name_instance + ") \"  );\n"
            "        return false;\n"
            "}\n";
        all_clients = all_clients + single_client;
    }
    QString merge = "    // open ports\n\n" + all_ports + "    // attach services as clients\n\n" + all_clients;

    QString value_OPEN_PORTS_AND_ATTACH_CLIENTS = merge;
    dataText.replace(KEY_OPEN_PORTS_AND_ATTACH_CLIENTS, value_OPEN_PORTS_AND_ATTACH_CLIENTS);

    // 2.3  @OPEN_CONNECTIONS_TO_COMPONENTS@
    QRegularExpression KEY_OPEN_CONNECTIONS_TO_COMPONENTS("@OPEN_CONNECTIONS_TO_COMPONENTS@");
    QString all_components;
    for (auto& UsedService : m_skillDescription.UsedServices) {
        all_components = all_components +
            "    if (!yarp::os::Network::connect(client_port_"
                        + UsedService.name_instance + ".getName(), \"" + UsedService.port_name_server + "\", \"" + UsedService.connect_type + "\")) {\n"
            "        qWarning(\"Error! Could not connect to server : " + UsedService.port_name_server + " \" );\n"
            "        return false;\n"
            "    }\n";
    }
    all_components = "    // open connections to components\n\n" + all_components;
    QString value_OPEN_CONNECTIONS_TO_COMPONENTS = all_components;
    dataText.replace(KEY_OPEN_CONNECTIONS_TO_COMPONENTS, value_OPEN_CONNECTIONS_TO_COMPONENTS);

    // 2.4 @ADD_CONSTRUCTOR@ if needed (case not default in .h)
    QRegularExpression KEY_ADD_CONSTRUCTOR("@ADD_CONSTRUCTOR@");
    if (m_skillDescription.add_default_constructor == true) {

        QString attributes_init_string_intro = m_skillDescription.skill_name + "SkillDataModel::" + m_skillDescription.skill_name + "SkillDataModel("; // GoToSkillDataModel::GoToSkillDataModel

        QStringList ListParamToAssign_data_type = generateStringList_data_type(m_skillDescription.ListAttributesParsedAsOption);
        QStringList ListParamToAssign_name_instance = generateStringList_name_instance(m_skillDescription.ListAttributesParsedAsOption);
        QString attributes_init_string_part_1 = generateListConstructorParameters(ListParamToAssign_data_type, ListParamToAssign_name_instance);

        QString attributes_init_string_mid = ") :\n                        ";

        QStringList ListMemberAttributes = generateStringList_name_instance(m_skillDescription.ListAttributesParsedAsOption);
        const QStringList& ListParamToAssign = ListMemberAttributes;
        QString attributes_init_string_part_2 = generateListConstructorParametersAssign(ListMemberAttributes, ListParamToAssign);

        QString attributes_init_string_end = "\n{\n}";

        QString constructor_string = attributes_init_string_intro + attributes_init_string_part_1 + attributes_init_string_mid + attributes_init_string_part_2 + attributes_init_string_end;
        QString value_ADD_CONSTRUCTOR = constructor_string;
        dataText.replace(KEY_ADD_CONSTRUCTOR, value_ADD_CONSTRUCTOR);

    } else {
        dataText.replace(KEY_ADD_CONSTRUCTOR, "");
    }

    // 3: create new file and insert the dataText
    QFile output_file(m_skillDescription.path_skill_folder + m_skillDescription.skill_name + "SkillDataModel.cpp");
    if (output_file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&output_file);
        out << dataText;
    }
    output_file.close();
}

//void SkillGenerator::generate_Format()
//{

//    QFile template_file (m_templatePath + "format.t");

//    // 1: open and read the template
//    template_file.open(QIODevice::Text | QIODevice::ReadOnly);
//    QString dataText = template_file.readAll();

//    // 2: replace

//    // 2.1: name
//    dataText.replace(m_keys.key_skill_name, m_skillDescription.skill_name);

//    // 2.2:  ....

//    // 3: create new file and insert the dataText
//    QFile output_file(m_skillDescription.path_skill_folder + "format.extension");
//    if( output_file.open(QFile::WriteOnly | QFile::Truncate)) {
//        QTextStream out(& output_file);
//        out << dataText;
//    }
//    output_file.close();
//}

int SkillGenerator::write()
{
    // Read skill_version from the main scxml file
    m_skillDescription.skill_version = m_translationUnit->mainDocument->root->skill_version;
    if (m_skillDescription.skill_version.isEmpty()) {
        m_skillDescription.skill_version = "0.0";
    }

    auto docs = m_translationUnit->allDocuments;

    for (int i = 0, ei = docs.size(); i != ei; ++i) {
        auto doc = docs.at(i);
        for (DocumentModel::AbstractState* astate : doc->allStates) { // doc->allStates identifies all the states inside the scxml input file
            auto state = astate->asState();
            if (!state) {
#ifdef _DEBUG
                qDebug() << astate->id;
#endif
                continue;
            }
#ifdef _DEBUG
            qDebug() << state->id << state->xmlLocation.line << state->xmlLocation.column << state->dataElements.size() << state->bt_status;
#endif
            // need to control the gerarchy of the states, s.t. the external "wrapper" is not tackled into the list
            State new_state;
            new_state.id = state->id;
            new_state.SkillAckEnum = state->bt_status;
            new_state.SkillAck = decoderEnum(state->bt_status);
            m_skillDescription.ListStates.push_back(new_state);
        }

        // <datamodel> interpreter
        for (DocumentModel::Node* anode : doc->allNodes) {
            auto data = anode->asDataElement();
            if (!data) {
                // not a <data> element
                continue;
            }
#ifdef _DEBUG
            cout << "\n\n ************ ACTUAL TEST************  \n\n ";
            //qDebug() << data->id << data->expr << data->cpp_type << data->thrift_protocol << data->service_type  << data->client_port_name << data->init_source << data->connect_type;
            cout << "\n\n ************ end ACTUAL TEST************  \n\n ";
#endif
            // access to data element
            if (data->thrift_protocol != "") { // means that the data represents a service!
                Service service;
                service.name_instance = data->id;
                service.service_type = data->service_type;
                service.connect_type = data->connect_type;
                service.thrift_protocol = data->thrift_protocol;
                service.service_function = data->service_function;
                service.port_name_client_attribute = data->port_name_client_attribute;
                service.port_name_server = data->port_name_server;
                m_skillDescription.UsedServices.push_back(service);
            } else if (data->cpp_type != "") { // means that the data represents an attribute!
                Attribute attribute;
                attribute.data_type = data->cpp_type;
                attribute.name_instance = data->id;
                attribute.value = data->expr;
                attribute.init_source = data->init_source;
                m_skillDescription.UsedAttributes.push_back(attribute);
                if (attribute.init_source == "parse_as_option") {
                    m_skillDescription.add_default_constructor = true;
                    m_skillDescription.ListAttributesParsedAsOption.push_back(attribute);
                }
            }
        }
    }

#ifdef _DEBUG
    std::cout << "\n\n\n SKILL_DESCRIPTION m_skillDescription : \n      ListStates size : " << m_skillDescription.ListStates.size() << "\n\n";
    qDebug() << " Print: " << m_skillDescription.ListStates[0].id << " " << m_skillDescription.ListStates[1].id << " " << m_skillDescription.ListStates[2].id << " " << m_skillDescription.ListStates[3].id << " .\n\n";
#endif

    // **********************************++ !generate skill files **********************************++
    generate_Main();
    generate_Skill_h();
    generate_Skill_cpp();
    generate_Skill_DataModel_h();
    generate_Skill_DataModel_cpp();

    return NoError;
}
