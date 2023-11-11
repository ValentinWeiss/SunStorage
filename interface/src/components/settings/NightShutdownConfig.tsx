import { Button, MenuItem, Select, Switch, TextField } from "@mui/material";
import { FlexWrapper, ElementWrapper } from "../";
import { useForm, Controller } from "react-hook-form";
import { yupResolver } from "@hookform/resolvers/yup";
import { handleFetchError, httpResponseThrowIfInvalid } from "../../utils";
import { ESP32FetchClient } from "../../api/http/clients/ESP32FetchClient";
import {
    ESPStateData,
    nightShutdownConfiguration,
    nightShutdownConfigurationValidationSchema,
} from "../../data";
import { useContext, useEffect } from "react";
import { ESPStateContext, IESPStateFetchData } from "../../context/ESPStateContext";

export function NightShutdownConfig() {
    const { data: {nightShutdown}, fetch } = useContext<IESPStateFetchData>(ESPStateContext);

    const {
        setValue,
        handleSubmit,
        control,
        formState: { errors },
    } = useForm<nightShutdownConfiguration>({
        resolver: yupResolver(nightShutdownConfigurationValidationSchema),
        defaultValues: {
            nsEnabled: false,
            nsAzimuth: 0,
            nsElevation: 0,
        },
    });

    const onSubmit = async (data: nightShutdownConfiguration) => {
        console.log(data);
        var result = await ESP32FetchClient.SetESPState(data)
            .then((res) => httpResponseThrowIfInvalid(res))
            .then(x => fetch())
            .catch((x) => handleFetchError(x));
        console.log(result);
    };

    useEffect(() => {
        if (nightShutdown?.nsEnabled)
            setValue("nsEnabled", nightShutdown.nsEnabled, {
                shouldValidate: true,
            });
        if (nightShutdown?.nsAzimuth)
            setValue("nsAzimuth", nightShutdown.nsAzimuth, {
                shouldValidate: true,
            });
        if (nightShutdown?.nsElevation)
            setValue("nsElevation", nightShutdown.nsElevation, {
                shouldValidate: true,
            });
    }, [nightShutdown]);

    return (
        <>
            <ElementWrapper header="Configure Night Shutdown Position">
                <form onSubmit={handleSubmit(onSubmit)}>
                    <Controller
                        control={control}
                        name="nsEnabled"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Enabled: </td>
                                    <td>
                                        <Switch
                                            checked={field.value}
                                            {...field}
                                        />
                                    </td>
                                </tr>
                            </table>
                        )}
                    />
                    <Controller
                        control={control}
                        name="nsAzimuth"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Azimuth: </td>
                                    <td>
                                        <TextField
                                            error={!!errors?.nsAzimuth}
                                            helperText={
                                                errors?.nsAzimuth?.message
                                            }
                                            type="number"
                                            {...field}
                                        />
                                    </td>
                                </tr>
                            </table>
                        )}
                    />
                    <Controller
                        control={control}
                        name="nsElevation"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Elevation: </td>
                                    <td>
                                        <TextField
                                            error={!!errors?.nsElevation}
                                            helperText={
                                                errors?.nsElevation?.message
                                            }
                                            type="number"
                                            {...field}
                                        />
                                    </td>
                                </tr>
                            </table>
                        )}
                    />
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
