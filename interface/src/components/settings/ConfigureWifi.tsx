import { Button, MenuItem, Select, TextField } from "@mui/material";
import { FlexWrapper, ElementWrapper } from "../";
import { useFetch } from "../../hooks";
import { useForm, Controller } from "react-hook-form";
import { yupResolver } from "@hookform/resolvers/yup";
import { handleFetchError, httpResponseThrowIfInvalid } from "../../utils";
import { ESP32FetchClient } from "../../api/http/clients/ESP32FetchClient";
import {
    ConfigureWifiParams,
    ConfigureWifiParamsValidationSchema,
    accessPointDataWrapper,
} from "../../data";

export function ConfigureWifi() {
    const { data: accessPoints } = useFetch<accessPointDataWrapper>(
        ESP32FetchClient.GetWifiSsids,
        {} as accessPointDataWrapper,
        handleFetchError,
        true
    );

    const {
        setValue,
        handleSubmit,
        control,
        formState: { errors },
    } = useForm<ConfigureWifiParams>({
        resolver: yupResolver(ConfigureWifiParamsValidationSchema),
        defaultValues: {
            ssid: "",
            password: "",
        },
    });

    const onSubmit = async (data: ConfigureWifiParams) => {
        console.log(data);
        var result = await ESP32FetchClient.SetWifiSettings(data)
            .then((res) => httpResponseThrowIfInvalid(res))
            .catch((x) => handleFetchError(x));
        console.log(result);
    };

    return (
        <>
            <ElementWrapper header="Configure Wifi">
                <form onSubmit={handleSubmit(onSubmit)}>
                    <FlexWrapper
                        sx={{
                            "& > *": {
                                minWidth: "200px",
                                flex: 1,
                            },
                        }}
                    >
                        <Controller
                            control={control}
                            name="ssid"
                            render={({ field }) => (
                                <>
                                    <Select
                                        label="SSID"
                                        error={!!errors?.ssid}
                                        {...field}
                                    >
                                        {accessPoints &&
                                            accessPoints.accessPoints &&
                                            accessPoints.accessPoints.length &&
                                            accessPoints.accessPoints.map(
                                                (x) => {
                                                    return (
                                                        <MenuItem
                                                            key={x.ssid}
                                                            value={x.ssid}
                                                        >
                                                            {x.ssid}
                                                        </MenuItem>
                                                    );
                                                }
                                            )}
                                    </Select>
                                </>
                            )}
                        />
                        <Controller
                            control={control}
                            name="password"
                            render={({ field }) => (
                                <>
                                    <TextField
                                        label="Password"
                                        error={!!errors?.password}
                                        helperText={errors?.password?.message}
                                        type="password"
                                        {...field}
                                    />
                                </>
                            )}
                        />
                    </FlexWrapper>
                    <FlexWrapper
                        sx={{
                            flexDirection: "row-reverse",
                            marginTop: "10px",
                        }}
                    >
                        <Button
                            variant="contained"
                            color="primary"
                            type="submit"
                        >
                            Apply Configuration
                        </Button>
                    </FlexWrapper>
                </form>
            </ElementWrapper>
        </>
    );
}
