/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include <yarp/os/Network.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/Property.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/INavigation2D.h>

#include <GoTo.h>

#include <iostream>

class GoToComponent : public GoTo
{
public:
    GoToComponent() = default;

    bool open()
    {
        yarp::os::Property pnavclient_cfg;
        pnavclient_cfg.put("device", "navigation2DClient");
        pnavclient_cfg.put("local", "/GoToComponent/navigation2DClient");
        pnavclient_cfg.put("navigation_server", "/navigationServer");
        pnavclient_cfg.put("map_locations_server", "/mapServer");
        pnavclient_cfg.put("localization_server", "/localizationServer");

        if (!ddnavclient.open(pnavclient_cfg)) {
            yError("Could not open navigation2DClient");
            return false;
        }

        if (!ddnavclient.view(inav)) {
            yError("Could not open navigation2DClient");
            return false;
        }

        this->yarp().attachAsServer(port);
        if (!port.open("/GoToComponent")) {
            yError("Could not open ");
            return false;

        }

        return true;
    }

    void close()
    {
        port.close();
        ddnavclient.close();
    }

    void goTo(const std::string& destination) override
    {
        yWarning("goTo called with destination %s", destination.c_str());

        std::lock_guard<std::mutex> lock(mtx);
        if(!inav->checkInsideArea(destination)) {
            inav->gotoTargetByLocationName(destination);
        }
        running = true;
    }

    GoToStatus getStatus(const std::string& destination) override
    {
        yWarning("getStatus called with destination %s", destination.c_str());

        yarp::dev::Nav2D::NavigationStatusEnum status;

        std::lock_guard<std::mutex> lock(mtx);

        std::string current_destination;
        if (!inav->getNameOfCurrentTarget(current_destination)) {
            return ABORT;
        }

        if(destination != current_destination) {
            return NOT_STARTED;
        }

        if (!inav->getNavigationStatus(status)) {
            return ABORT;
        }

        switch(status) {
        case yarp::dev::Nav2D::navigation_status_idle:
            if (running) {
                if(inav->checkInsideArea(destination)) {
                    return SUCCESS;
                }
                return RUNNING;
            }
            return NOT_STARTED;
        case yarp::dev::Nav2D::navigation_status_preparing_before_move: [[fallthrough]];
        case yarp::dev::Nav2D::navigation_status_moving: [[fallthrough]];
        case yarp::dev::Nav2D::navigation_status_waiting_obstacle: [[fallthrough]];
        case yarp::dev::Nav2D::navigation_status_paused: [[fallthrough]];
        case yarp::dev::Nav2D::navigation_status_thinking:
            return RUNNING;
        case yarp::dev::Nav2D::navigation_status_goal_reached:
            return SUCCESS;
        case yarp::dev::Nav2D::navigation_status_aborted: [[fallthrough]];
        case yarp::dev::Nav2D::navigation_status_failing: [[fallthrough]];
        case yarp::dev::Nav2D::navigation_status_error: [[fallthrough]];
        default:
            return ABORT;
        }
    }

    void halt(const std::string& destination) override
    {
        yWarning("halt called with destination %s", destination.c_str());

        std::lock_guard<std::mutex> lock(mtx);

        if (!running) {
            return;
        }

//        std::string current_destination;
//        if (!inav->getNameOfCurrentTarget(current_destination) || destination.compare(current_destination) !=0) {
//            running = false;
//            return;
//        }

        inav->stopNavigation();
        running = false;
    }

    bool isAtLocation(const std::string& destination) override
    {
        yWarning("checkInsideArea called with destination %s", destination.c_str());

        std::lock_guard<std::mutex> lock(mtx);

        if (inav->checkInsideArea(destination)){
            return true;
        }
        return false;
    }

private:
    yarp::dev::PolyDriver ddnavclient;
    yarp::dev::Nav2D::INavigation2D* inav { nullptr };
    yarp::os::RpcServer port;

    std::mutex mtx;
    bool running { false };
};

int main()
{
    yarp::os::Network yarp;

    GoToComponent goToComponent;
    if (!goToComponent.open()) {
        return 1;
    }

    while (true) {
        yInfo("Server running happily");
        yarp::os::Time::delay(10);
    }

    goToComponent.close();

    return 0;
}
