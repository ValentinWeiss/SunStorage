import * as Yup from "yup";

export const shutdownConfigurationValidationSchema = Yup.object().shape({
    http: Yup.number().min(0),
    gps: Yup.number().min(0),
    wifi: Yup.number().min(0),
    dcf77: Yup.number().min(0),
});

export type shutdownConfiguration = Yup.InferType<
    typeof shutdownConfigurationValidationSchema
>;
