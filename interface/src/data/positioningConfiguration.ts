import * as Yup from "yup";

export const positioningConfigurationValidationSchema = Yup.object().shape({
    manualMode: Yup.boolean(), // if this is false, the other values can be ignored -> auto angle
    azimuth: Yup.number().min(0).max(360), // 0-360*
    elevation: Yup.number().min(0).max(90), // 0-90° (0° unten, 90° T-Pose)
    latitude: Yup.number().min(-90).max(90),
    longitude: Yup.number().min(-180).max(180),
    angleToNorth: Yup.number().min(0).max(360),
});

export type positioningConfiguration = Yup.InferType<
    typeof positioningConfigurationValidationSchema
>;
