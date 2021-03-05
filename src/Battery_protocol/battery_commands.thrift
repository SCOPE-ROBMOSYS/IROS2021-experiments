/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

 #enum BatteryLevel {BATTERY_LOW, BATTERY_MEDIUM, BATTERY_HIGH}
enum ChargingStatus {BATTERY_NOT_CHARGING, BATTERY_CHARGING}

service BatteryReader {
    double level();
    ChargingStatus charging_status();
}
