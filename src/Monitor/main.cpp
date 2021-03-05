/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/Log.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/ResourceFinder.h>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QScxmlStateMachine>
#include <QTextStream>
#include <QIcon>

enum class EventType
{
    Unknown,
    Tick,
    Command,
    Reply
};

std::string eventTypeToString(EventType t)
{
    switch (t) {
    case EventType::Tick:
        return "tick";
    case EventType::Command:
        return "command";
    case EventType::Reply:
        return "reply";
    case EventType::Unknown:
    default:
        return "[unknown]";
    }
}

EventType stringToEventType(const std::string& s)
{
    if (s == "tick") {
        return EventType::Tick;
    }
    if (s == "command") {
        return EventType::Command;
    }
    if (s == "reply") {
        return EventType::Reply;
    }
    return EventType::Unknown;
}


// FIXME copied from GoToStatus.h
enum GoToStatus
{
    NOT_STARTED = 0,
    RUNNING = 1,
    SUCCESS = 2,
    ABORT = 3
};


// FIXME Maybe use thrift...
struct MonitorEvent
{
    double timestamp {0.0};
    std::string source;
    std::string destination;
    EventType type;
    std::string command;
    std::string arguments;
};

class MonitorReader :
        public QObject,
        public yarp::os::Bottle
{
    Q_OBJECT
    Q_DISABLE_COPY(MonitorReader)
    Q_PROPERTY(double batteryLevel READ batteryLevel NOTIFY batteryLevelChanged)
    Q_PROPERTY(QString destination READ destination NOTIFY destinationChanged)
//     QML_ELEMENT

    MonitorReader() {}

public:
    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
    {
        Q_UNUSED(engine);
        Q_UNUSED(scriptEngine);

        if(!m_instance) {
            m_instance = new MonitorReader();
        }
        return m_instance;
    }

    bool read(yarp::os::ConnectionReader &reader) override;

    double batteryLevel() const { return m_batteryLevel; }
    QString destination() const { return m_destination;  }

Q_SIGNALS:
    void tick();

    void destinationChangeRequested(const QString& destination);
    void destinationChanged(const QString& destination);
    void batteryLevelChanged(double level);

private:
    static QObject* m_instance;

    double m_batteryLevel {100.0};
    QString m_destination {""};
};

QObject* MonitorReader::m_instance = nullptr;

bool MonitorReader::read(yarp::os::ConnectionReader &reader)
{
    bool ret = yarp::os::Bottle::read(reader);

    yInfo("%s", toString().c_str());

    MonitorEvent event {
        /*.timestamp =*/ get(0).asDouble(),
        /*.source =*/ get(1).asString(),
        /*.destination =*/ get(2).asString(),
        /*.type =*/ stringToEventType(get(3).asString()),
        /*.command =*/ (get(4).asList()->size() > 0 ? get(4).asList()->get(0).asString() : ""),
        /*.arguments =*/ (get(4).asList()->size() > 1 ? get(4).asList()->toString().substr(get(4).asList()->get(0).asString().size() + 1) : "")
    };

    yDebugExternalTime(event.timestamp, "Message received:\n  From     : %s\n  To       : %s\n  %s: %s\n  Arguments: %s",
        event.type == EventType::Reply ? event.destination.c_str() : event.source.c_str(),
        event.type == EventType::Reply ? event.source.c_str() : event.destination.c_str(),
        (event.type == EventType::Reply ? "Reply    " : (
         event.type == EventType::Command ? "Command  " : (
         event.type == EventType::Tick ? "Tick     " : "Unknown  "))),
        event.command.c_str(),
        event.arguments.c_str()
    );

    if (event.type == EventType::Tick) {
        yInfo("tick");
        Q_EMIT tick();
    }

    if (event.type == EventType::Reply
          && event.source == "/BatteryReaderBatteryLevelClient"
          && event.destination == "/BatteryComponent"
          && event.command == "level") {
        double batteryLevel = get(4).asList()->get(1).asDouble();
        if (m_batteryLevel != batteryLevel) {
            m_batteryLevel = batteryLevel;
            yInfo("level changed %f", m_batteryLevel);
            Q_EMIT batteryLevelChanged(m_batteryLevel);
        }
    }

    if (event.type == EventType::Command
          && (
            (event.source == "/GoToDestination/BT_rpc/client" &&
             event.destination == "/GoToDestination/BT_rpc/server") ||
            (event.source == "/GoToChargingStation/BT_rpc/client" &&
             event.destination == "/GoToChargingStation/BT_rpc/server")
          )
          && event.command == "send_start") {
        QString destination = ((event.destination == "/GoToDestination/BT_rpc/server") ? "kitchen" : "charging_station");
        yInfo("destination change requested: %s", destination.toStdString().c_str());
        Q_EMIT destinationChangeRequested(destination.toStdString().c_str());
    }

    if (event.type == EventType::Reply
          && event.destination == "/GoToComponent"
          && event.command == "getStatus") {
        QString destination = get(4).asList()->get(1).asString().c_str();
        GoToStatus status = static_cast<GoToStatus>(get(4).asList()->get(2).asInt32());
        if (m_destination != destination && status == RUNNING) {
            m_destination = destination;
            yInfo("destination changed: %s", m_destination.toStdString().c_str());
            Q_EMIT destinationChanged(m_destination.toStdString().c_str());
        }
    }

    return ret;
}



int main (int argc, char *argv[])
{
    yarp::os::ResourceFinder &rf = yarp::os::ResourceFinder::getResourceFinderSingleton();
//     rf.setDefaultConfigFile("Monitor.ini");  //overridden by --from parameter
//     rf.setDefaultContext("Monitor");         //overridden by --context parameter
    rf.configure(argc, argv);

    yarp::os::Network yarp;

    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon::fromTheme("gnome-power-manager"));

    QQmlApplicationEngine engine;

    qmlRegisterSingletonType<MonitorReader>("scope.monitor.MonitorReader", 1, 0,
                                            "MonitorReader",
                                            &MonitorReader::qmlInstance);


    engine.load(QUrl(QStringLiteral("qrc:///monitor.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    yarp::os::Port port;
    auto* monitorReader = dynamic_cast<MonitorReader*>(MonitorReader::qmlInstance(&engine, nullptr));
    port.setReader(*monitorReader);
    port.setCallbackLock();
    port.open("/monitor");

    // Just for testing that signals work
    QObject::connect(monitorReader, &MonitorReader::tick, [](){ yWarning("SIGNAL RECEIVED: tick"); });
    QObject::connect(monitorReader, &MonitorReader::batteryLevelChanged, [](double level){ yWarning("SIGNAL RECEIVED: batteryLevelChanged(level = %f)", level); });
    QObject::connect(monitorReader, &MonitorReader::destinationChangeRequested, [](const QString& destination){ yWarning("SIGNAL RECEIVED: destinationChangeRequested(destination = %s)", destination.toStdString().c_str()); });
    QObject::connect(monitorReader, &MonitorReader::destinationChanged, [](const QString& destination){ yWarning("SIGNAL RECEIVED: destinationChanged(destination = %s)", destination.toStdString().c_str()); });

    return app.exec();
}

#include "main.moc"
