/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include <QCoreApplication>
#include <QXmlStreamReader>
#include <QFile>
#include <QDir>
#include <QDebug>

#include "qscxmlcompiler_p.h"
#include <QTextCodec>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "iostream"
#include "vector"

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QTextStream>

#include "SkillGenerator.h"

using namespace std;

static void collectAllDocuments(DocumentModel::ScxmlDocument *doc,
                                QList<DocumentModel::ScxmlDocument *> *docs)
{
    docs->append(doc);
    for (DocumentModel::ScxmlDocument *subDoc : qAsConst(doc->allSubDocuments)) {
        collectAllDocuments(subDoc, docs);
    }
}

int main(int argc, char* argv[])
{
    cout << "start main ";
    QCoreApplication app(argc, argv);
    app.setApplicationVersion(QString::fromLatin1("%1 (Qt %2)").arg(
                              QString::number(/*Q_QSCXMLC_OUTPUT_REVISION*/2),
                              QString::fromLatin1(QT_VERSION_STR)));

    QCommandLineParser cmdParser;
    QTextStream errs(stderr, QIODevice::WriteOnly);

    cmdParser.addHelpOption();
    cmdParser.addVersionOption();
    cmdParser.setApplicationDescription(QCoreApplication::translate("main",
                       "Compiles the given input.scxml file to a header and a cpp file."));

    QCommandLineOption optionNamespace(QLatin1String("namespace"),
                       QCoreApplication::translate("main", "Put generated code into <namespace>."),
                       QCoreApplication::translate("main", "namespace"));
    QCommandLineOption optionClassName(QLatin1String("classname"),
                       QCoreApplication::translate("main", "Generate <name> for state machine class name."),
                       QCoreApplication::translate("main", "name"));
    QCommandLineOption optionStateMethods(QLatin1String("statemethods"),
                       QCoreApplication::translate("main", "Generate read and notify methods for states"));

    cmdParser.addPositionalArgument(QLatin1String("input"),
                       QCoreApplication::translate("main", "Input SCXML file."));

//    cmdParser.addPositionalArgument(QLatin1String("application_root_path"),
//                       QCoreApplication::translate("main", "root path."));


//    QCommandLineOption rootPath(QStringList() << "o" << "opt", QCoreApplication::translate("main", "Option is: rootPath")
//                                       ,QCoreApplication::translate("main", "opt"), "2");
//    cmdParser.addOption(rootPath);
//    QString opt2 = cmdParser.value(rootPath);
//    qDebug() << opt2;


    cmdParser.addOption(optionNamespace);
    cmdParser.addOption(optionClassName);
    cmdParser.addOption(optionStateMethods);

    cmdParser.process(QCoreApplication::arguments());

    const QStringList inputFiles = cmdParser.positionalArguments();

    if (inputFiles.count() < 1) {
        errs << QCoreApplication::translate("main", "Error: no input file.") << endl;
        cmdParser.showHelp(NoInputFilesError);
    }

    if (inputFiles.count() > 1) {
        errs << QCoreApplication::translate("main", "Error: unexpected argument(s): %1")
                .arg(inputFiles.mid(1).join(QLatin1Char(' '))) << endl;
        cmdParser.showHelp(NoInputFilesError);
    }

    const QString scxmlFileName = inputFiles.at(0);
    //qDebug() << "\n ----------------- Skill Auto-generation ----------------- \nComplete path of input file -->" << scxmlFileName << "\n";

    TranslationUnit options;
    options.stateMethods = cmdParser.isSet(optionStateMethods);
    if (cmdParser.isSet(optionNamespace))
        options.namespaceName = cmdParser.value(optionNamespace);

    QString mainClassName = cmdParser.value(optionClassName);

    //qDebug() << __LINE__;

    QFile file(scxmlFileName);
    if (!file.open(QFile::ReadOnly)) {
        errs << QStringLiteral("Error: cannot open input file %1").arg(scxmlFileName);
        return CannotOpenInputFileError;
    }
    //qDebug() << __LINE__;

    QXmlStreamReader reader(&file);
    QScxmlCompiler compiler(&reader);
    compiler.setFileName(file.fileName());
    compiler.compile();
    if (!compiler.errors().isEmpty()) {
        const auto errors = compiler.errors();
        for (const QScxmlError &error : errors) {
            errs << error.toString() << endl;
        }
        return ParseError;
    }
    //qDebug() << __LINE__;

    auto mainDoc = QScxmlCompilerPrivate::get(&compiler)->scxmlDocument();
    if (mainDoc == nullptr) {
        Q_ASSERT(!compiler.errors().isEmpty());
        const auto errors = compiler.errors();
        for (const QScxmlError &error : errors) {
            errs << error.toString() << endl;
        }
        return ScxmlVerificationError;
    }

    //qDebug() << __LINE__;
    if (mainClassName.isEmpty())
        mainClassName = mainDoc->root->name;
    if (mainClassName.isEmpty()) {
        mainClassName = QFileInfo(scxmlFileName).fileName();
        int dot = mainClassName.lastIndexOf(QLatin1Char('.'));
        if (dot != -1)
            mainClassName = mainClassName.left(dot);
    }
    qWarning() << mainClassName;
    //qDebug() << __LINE__;

    QList<DocumentModel::ScxmlDocument *> docs;
    collectAllDocuments(mainDoc, &docs);

    TranslationUnit tu = options;
    tu.allDocuments = docs;
    tu.scxmlFileName = QFileInfo(file).fileName();
    tu.mainDocument = mainDoc;

    tu.classnameForDocument.insert(mainDoc, mainClassName);

    cout << "DOCS SIZE1 : " << docs.size() << "\n\n";

    docs.pop_front();

    for (DocumentModel::ScxmlDocument *doc : qAsConst(docs)) {
        auto name = doc->root->name;
        auto prefix = name;
        //qDebug() << "\n\n DEBUG NAME: " << name << "\n\n";
        if (name.isEmpty()) {
            prefix = QStringLiteral("%1_StateMachine").arg(mainClassName);
            name = prefix;
        }

        int counter = 1;
        while (tu.classnameForDocument.key(name) != nullptr)
            name = QStringLiteral("%1_%2").arg(prefix).arg(++counter);

        tu.classnameForDocument.insert(doc, name);
    }

    // QString try_str = docs.at(0)->root->cppDataModelClassName;
    // cout << "DOCS SIZE2 : " << docs.size() << "\n\n";
    // cout << "\n\nTRY : " << try_str.toStdString() ;
    // qDebug() << try_str;

    tu.Path = scxmlFileName;
    SkillGenerator generator(&tu);
    return generator.init(); //    return write(&tu);
}
