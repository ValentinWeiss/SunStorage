import * as Yup from "yup";

export const displayConfigurationValidationSchema = Yup.object().shape({
    d_powerSolar: Yup.boolean(),
    d_dailyProduction: Yup.boolean(),
    d_soc: Yup.boolean(),
    d_voltageLowerCell: Yup.boolean(),
    d_voltageUpperCell: Yup.boolean(),
    d_solarCurrent: Yup.boolean(),
    d_chargingCurrent: Yup.boolean(),
    d_systemCurrent: Yup.boolean(),
    d_powerSystem: Yup.boolean(),
    d_powerCharging: Yup.boolean(),
    d_dailyConsumption: Yup.boolean(),
    d_thresholdVoltage: Yup.boolean(),
    d_maximumVoltage: Yup.boolean(),
    d_trickleThreshold: Yup.boolean(),
    d_highPowerCircuit: Yup.boolean(),
    d_batteryTemperature: Yup.boolean(),
    d_batteryOverheatedHysteresis: Yup.boolean(),
    d_batterySize: Yup.boolean(),
    d_currentTarget: Yup.boolean(),
    d_chargerState: Yup.boolean(),
    d_usbMode: Yup.boolean(),
    d_azimuth: Yup.boolean(),
    d_elevation: Yup.boolean(),
    d_manualAdjustment: Yup.boolean(),
    d_nightShutdownEnabled: Yup.boolean(),
    d_nightPositionAzimuth: Yup.boolean(),
    d_nightPositionElevation: Yup.boolean(),
    d_latitude: Yup.boolean(),
    d_longitude: Yup.boolean(),
    d_angleToNorth: Yup.boolean(),
    d_time: Yup.boolean(),
    d_httpDisableTimeout: Yup.boolean(),
    d_gpsDisableTimeout: Yup.boolean(),
    d_dcfDisableTimeout: Yup.boolean(),
    d_wifiDisableTimeout: Yup.boolean(),
});

export type displayConfiguration = Yup.InferType<
    typeof displayConfigurationValidationSchema
>;
