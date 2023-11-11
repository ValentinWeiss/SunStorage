import { Button, Switch, TextField } from "@mui/material";
import { FlexWrapper, ElementWrapper } from "../";
import { useForm, Controller, useWatch } from "react-hook-form";
import { yupResolver } from "@hookform/resolvers/yup";
import { handleFetchError, httpResponseThrowIfInvalid } from "../../utils";
import { ESP32FetchClient } from "../../api/http/clients/ESP32FetchClient";
import {
    ESPStateData,
    positioningConfiguration,
    positioningConfigurationValidationSchema,
} from "../../data";
import { useContext, useEffect } from "react";
import { ESPStateContext, IESPStateFetchData } from "../../context/ESPStateContext";

export function PositionConfig() {
    const { data: {positioning}, fetch } = useContext<IESPStateFetchData>(ESPStateContext);

    const {
        setValue,
        handleSubmit,
        control,
        formState: { errors, dirtyFields },
    } = useForm<positioningConfiguration>({
        resolver: yupResolver(positioningConfigurationValidationSchema),
        defaultValues: {
            latitude: 0,
            longitude: 0,
            azimuth: 0,
            elevation: 0,
            angleToNorth: 0,
            manualMode: false,
        },
    });

    const manualMode = useWatch({
        control,
        name: "manualMode",
        defaultValue: false,
    });

    const onSubmit = async (data: positioningConfiguration) => {
        console.log(data);
        var result = await ESP32FetchClient.SetESPState(data)
            .then((res) => httpResponseThrowIfInvalid(res))
            .then(x => fetch())
            .catch((x) => handleFetchError(x));
        console.log(result);
    };

    useEffect(() => {
        if (positioning?.latitude)
            setValue("latitude", positioning.latitude, {
                shouldValidate: true,
            });
        if (positioning?.longitude)
            setValue("longitude", positioning.longitude, {
                shouldValidate: true,
            });
        if (positioning?.azimuth)
            setValue("azimuth", positioning.azimuth, {
                shouldValidate: true,
            });
        if (positioning?.elevation)
            setValue("elevation", positioning.elevation, {
                shouldValidate: true,
            });
        if (positioning?.angleToNorth)
            setValue("angleToNorth", positioning.angleToNorth, {
                shouldValidate: true,
            });
        if (positioning?.manualMode)
            setValue("manualMode", positioning.manualMode, {
                shouldValidate: true,
            });
    }, [positioning]);

    return (
        <>
            <ElementWrapper header="Configure Position">
                <form onSubmit={handleSubmit(onSubmit)}>
                    <Controller
                        control={control}
                        name="latitude"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Latitude: </td>
                                    <td>
                                        <TextField
                                            error={!!errors?.latitude}
                                            helperText={
                                                errors?.latitude?.message
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
                        name="longitude"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Longitude: </td>
                                    <td>
                                        <TextField
                                            error={!!errors?.longitude}
                                            helperText={
                                                errors?.longitude?.message
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
                        name="azimuth"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Azimuth: </td>
                                    <td>
                                        <TextField
                                            error={!!errors?.azimuth}
                                            helperText={
                                                errors?.azimuth?.message
                                            }
                                            type="number"
                                            disabled={!manualMode}
                                            {...field}
                                        />
                                    </td>
                                </tr>
                            </table>
                        )}
                    />
                    <Controller
                        control={control}
                        name="elevation"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Elevation: </td>
                                    <td>
                                        <TextField
                                            error={!!errors?.elevation}
                                            helperText={
                                                errors?.elevation?.message
                                            }
                                            type="number"
                                            disabled={!manualMode}
                                            {...field}
                                        />
                                    </td>
                                </tr>
                            </table>
                        )}
                    />
                    <Controller
                        control={control}
                        name="manualMode"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>ManualMode: </td>
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
                        name="angleToNorth"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Angle To North: </td>
                                    <td>
                                        <TextField
                                            error={!!errors?.angleToNorth}
                                            helperText={
                                                errors?.angleToNorth?.message
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
