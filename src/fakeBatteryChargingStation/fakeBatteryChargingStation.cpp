/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/RFModule.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/RpcClient.h>
#include <yarp/dev/ILocalization2D.h>
#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/IBattery.h>

#include <cmath>
#include "FakeBatteryService.h"

class FakeBatteryChargingStation :
        public yarp::os::RFModule
{
public:
    FakeBatteryChargingStation() = default;
    FakeBatteryChargingStation(const FakeBatteryChargingStation&) = delete;
    FakeBatteryChargingStation(FakeBatteryChargingStation&&) = delete;
    FakeBatteryChargingStation& operator=(const FakeBatteryChargingStation&) = delete;
    FakeBatteryChargingStation& operator=(FakeBatteryChargingStation&&) = delete;

    ~FakeBatteryChargingStation() override = default;

    bool configure(yarp::os::ResourceFinder& rf) override
    {

        if (!yarp::os::NetworkBase::checkNetwork()) {
            yError("Error! YARP Network is not initialized");
            return false;
        }

        // Read options
        // name
        name = "FakeBatteryChargingStation";

        // localization_server
        std::string localization_server = "/localizationServer";

        // navigation_server
        std::string navigation_server = "/navigationServer";

        // map_locations_server
        std::string map_locations_server = "/mapServer";

        // fake_battery server
        std::string fakeBattery_server = "/fakeBattery";

        // location_name
        location_name = "charging_station";

        // linear_tolerance
        linear_tolerance = 0.5;

        // angular_tolerance
        angular_tolerance = 10.0;

        // navigation2DClient
        yarp::os::Property pnav_cfg;
        pnav_cfg.put("device", "navigation2DClient");
        pnav_cfg.put("local", "/" + name + "/navigation2DClient");
        pnav_cfg.put("navigation_server", navigation_server);
        pnav_cfg.put("map_locations_server", map_locations_server);
        pnav_cfg.put("localization_server", localization_server);

        if (!ddnavclient.open(pnav_cfg)) {
            yError("Could not open navigation2DClient");
            return false;
        }

        if (!ddnavclient.view(inav)) {
            yError("Could not open navigation2DClient");
            return false;
        }

        // batteryClient
        yarp::os::Property pbatteryclient_cfg;
        pbatteryclient_cfg.put("device", "batteryClient");
        pbatteryclient_cfg.put("remote", fakeBattery_server);
        pbatteryclient_cfg.put("local", "/" + name + "/batteryClient");


        if (!ddbatteryclient.open(pbatteryclient_cfg)) {
            yError("Could not open batteryClient");
            return false;
        }

        if (!ddbatteryclient.view(ibat)) {
            yError("Could not open batteryClient");
            return false;
        }

        // fakeBattery_client_port
        if (!fakeBattery_client_port.open("/" + name + "/" + fakeBattery_server + "control/rpc:o")) {
            yError("Error! Cannot open RPC port");
            return false;
        }

        if (!fakeBatteryService.yarp().attachAsClient(fakeBattery_client_port)) {
            yError("Error! Could not attach as client");
            return false;
        }

        if (!yarp::os::Network::connect(fakeBattery_client_port.getName(), fakeBattery_server + "/control/rpc:i")) {
            yError("Error! Could not connect to fakeBattery RPC port");
            return false;
        }

        // Small delay to ensure that data is ready
        yarp::os::Time::delay(1.0);

        return true;
    }

    bool close() override
    {
        fakeBattery_client_port.close();
        ddbatteryclient.close();
        ddnavclient.close();
        return true;
    };

    bool updateModule() override
    {
        bool docked = inav->checkNearToLocation(location_name, linear_tolerance, angular_tolerance);

        double charge;
        ibat->getBatteryCharge(charge);

        if (docked) {
            if (!prev_docked) {
                yInfo() << name << "Docked!";
                // Save the current before docking
                ibat->getBatteryCurrent(discharge_current);

                // Just in case it was interrupted
                discharge_current=fabs(discharge_current);
                if (discharge_current < 0.3) {
                    discharge_current = 1.0;
                }

            }
            fakeBatteryService.setBatteryCurrent((charge >= 99.7) ? 0.0 : -discharge_current);
        } else if (prev_docked) {
            // Restore the current
            yInfo() << name << "Undocked!";
            fakeBatteryService.setBatteryCurrent(discharge_current);
        }

        double current;
        ibat->getBatteryCurrent(current);
        yDebug() << "Current =" << current << "Charge =" << charge;

        prev_docked = docked;
        return true;
    };

private:
    yarp::os::Network yarp;

    yarp::dev::PolyDriver ddnavclient;
    yarp::dev::Nav2D::INavigation2D* inav { nullptr };

    yarp::dev::PolyDriver ddbatteryclient;
    yarp::dev::IBattery* ibat { nullptr };

    yarp::os::RpcClient fakeBattery_client_port;
    FakeBatteryService fakeBatteryService;

    yarp::dev::Nav2D::Map2DLocation loc;

    std::string name;
    std::string location_name;
    double linear_tolerance;
    double angular_tolerance;

    double discharge_current;
    bool prev_docked {false};
};


int main(int argc, char *argv[])
{
    yarp::os::Network yarp;

    /* prepare and configure the resource finder */
    yarp::os::ResourceFinder& rf = yarp::os::ResourceFinder::getResourceFinderSingleton();
    rf.configure(argc, argv);
    rf.setVerbose(true);
    // Create and run our module
    FakeBatteryChargingStation fakeBatteryChargingStation;
    fakeBatteryChargingStation.setName("/fakeBatteryChargingStation");

    return fakeBatteryChargingStation.runModule(rf);
}
