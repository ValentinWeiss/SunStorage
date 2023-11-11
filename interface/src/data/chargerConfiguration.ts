import * as Yup from "yup";

export const chargerConfigurationValidationSchema = Yup.object().shape({
    usbMode: Yup.number().oneOf([0, 1, 2]), // value in W -> 0: 2.5, 1: 5, 2: 10
    currentTarget: Yup.number().min(0).max(2000), // value in mA
    thresholdVoltage: Yup.number().min(0).max(1000),
    maximumVoltage: Yup.number().min(3800).max(4200),
    trickleThreshold: Yup.number().min(3000).max(4200), // value in mV
    batterySize: Yup.number().min(0).max(10000),
    overheatedTemperature: Yup.number().min(20).max(60),
    highPowerEnabled: Yup.boolean(),
    overheatedHysteresis: Yup.number().min(1).max(10),
});

export type chargerConfiguration = Yup.InferType<
    typeof chargerConfigurationValidationSchema
>;
