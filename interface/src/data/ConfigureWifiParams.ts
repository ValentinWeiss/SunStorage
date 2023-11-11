import * as Yup from "yup";

export const ConfigureWifiParamsValidationSchema = Yup.object().shape({
    ssid: Yup.string().required(),
    password: Yup.string().required("Password required"),
});
export type ConfigureWifiParams = Yup.InferType<
    typeof ConfigureWifiParamsValidationSchema
>;
