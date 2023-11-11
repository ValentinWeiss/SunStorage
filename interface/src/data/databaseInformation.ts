export interface databaseInformation {
    TimestampID: Date;
    GpsLat?: string;
    GpsLon?: string;
    BatteryLevel?: string;
    BatteryTemperature?: string;
    ChargerState?: string;
    CompassBearing?: string;
    CurrentIn?: string;
    CurrentOut?: string;
    GpsSatellitesInView?: string;
    GpsUsedSatellites?: string;
    GyroscopeAccelX?: string;
    GyroscopeAccelY?: string;
    GyroscopeAccelZ?: string;
    GyroscopeTemperature?: string;
    MagneticVariation?: string;
    VoltageHigherCell?: string;
    VoltageLowerCell?: string;
}

export interface databaseInformationAllParams {
    startTime: Date;
    endTime?: Date;
}
