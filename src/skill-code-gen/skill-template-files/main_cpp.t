/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include "@KEY_SKILL_NAME@Skill.h"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <yarp/os/ResourceFinder.h>

int main(int argc, char **argv)
{
    yarp::os::ResourceFinder &rf = yarp::os::ResourceFinder::getResourceFinderSingleton();
    rf.setDefaultConfigFile("@KEY_SKILL_NAME@.ini");  //overridden by --from parameter
    rf.setDefaultContext("@KEY_SKILL_NAME@");         //overridden by --context parameter
    rf.configure(argc, argv);

    yarp::os::Bottle config;
    config.fromString(rf.toString());

    int yargc = 0;
    char** yargv = new char*[config.size() * 2 + 2];

    yargv[yargc++] = argv[0];

    for (int i = config.size() - 1; i >= 0 ; --i) {
        auto* bot = config.get(i).asList();

        Q_ASSERT(bot->size() <= 2);
        for (size_t j = 0; j < bot->size(); ++j) {
            std::string x = (j == 0 ? "--" + bot->get(j).toString() : bot->get(j).toString());
            yargv[yargc++] = strdup(x.c_str());
        }
    }
    yargv[yargc] = nullptr;

    // We feed QCoreApplication all the options imported from the ResourceFinder.
    QCoreApplication app(yargc, yargv);
    QCoreApplication::setApplicationVersion("@KEY_SKILL_VERSION@");

    QCommandLineParser parser;
    parser.setApplicationDescription("@KEY_SKILL_NAME@ Skill");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOption({"from", "The <filename> of the config file to import.", "filename", "@KEY_SKILL_NAME@.ini"});
    parser.addOption({"context", "The <yarp context> where the config file should be located.", "yarp context", "@KEY_SKILL_NAME@"});

    parser.addOption({{"n", "skill-name"}, "The <name> of the skill (implies the port opened).", "name", "@KEY_SKILL_NAME@"});

@KEY_ADDITIONAL_OPTION_PARSE@

    // Process the actual command line arguments given by the user
    parser.process(app);

    const QStringList args = parser.positionalArguments();
    // source is args.at(0), destination is args.at(1)

    QString skillName = parser.value("skill-name");

@KEY_ADDITIONAL_OPTION_ASSIGN@

    QCoreApplication::setApplicationName(skillName);

    @KEY_SKILL_NAME@Skill bt(skillName.toStdString() @KEY_CONSTRUCTOR_ATTRIBUTES_p1_PASSED_ARGS@);
    if (!bt.start()) {
        return 1;
    }

    int ret = app.exec();

    for (int i = 0; i < yargc; ++i) {
        free (yargv[i]);
    }
    delete[] yargv;

    return ret;
}
