import * as Yup from "yup";

export const nightShutdownConfigurationValidationSchema = Yup.object().shape({
    nsEnabled: Yup.boolean(),
    nsAzimuth: Yup.number().min(0).max(360), // 0-360*
    nsElevation: Yup.number().min(0).max(90), // 0-90° (0° unten, 90° T-Pose)
});

export type nightShutdownConfiguration = Yup.InferType<
    typeof nightShutdownConfigurationValidationSchema
>;
