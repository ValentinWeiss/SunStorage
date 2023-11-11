import { Button, MenuItem, Select, Switch, TextField } from "@mui/material";
import { FlexWrapper, ElementWrapper } from "../";
import { useForm, Controller } from "react-hook-form";
import { yupResolver } from "@hookform/resolvers/yup";
import { handleFetchError, httpResponseThrowIfInvalid } from "../../utils";
import { ESP32FetchClient } from "../../api/http/clients/ESP32FetchClient";
import {
    chargerConfiguration,
    chargerConfigurationValidationSchema,
    ESPStateData,
} from "../../data";
import { useContext, useEffect } from "react";
import { ESPStateContext, IESPStateFetchData } from "../../context/ESPStateContext";

export function ChargerConfig() {
    const { data: {charger}, fetch } = useContext<IESPStateFetchData>(ESPStateContext);

    const {
        setValue,
        handleSubmit,
        control,
        formState: { errors },
    } = useForm<chargerConfiguration>({
        resolver: yupResolver(chargerConfigurationValidationSchema),
        defaultValues: {
            usbMode: 0,
            currentTarget: 0,
            thresholdVoltage: 0,
            maximumVoltage: 3800,
            trickleThreshold: 3000,
            batterySize: 0,
            overheatedTemperature: 20,
            highPowerEnabled: false,
            overheatedHysteresis: 1,
        },
    });

    const onSubmit = async (data: chargerConfiguration) => {
        console.log(data);
        var result = await ESP32FetchClient.SetESPState(data)
            .then((res) => httpResponseThrowIfInvalid(res))
            .then(x => fetch())
            .catch((x) => handleFetchError(x));
        console.log(result);
    };

    useEffect(() => {
        if (charger?.usbMode)
            setValue("usbMode", charger.usbMode, {
                shouldValidate: true,
            });
        if (charger?.currentTarget)
            setValue("currentTarget", charger.currentTarget, {
                shouldValidate: true,
            });
        if (charger?.thresholdVoltage)
            setValue("thresholdVoltage", charger.thresholdVoltage, {
                shouldValidate: true,
            });
        if (charger?.maximumVoltage)
            setValue("maximumVoltage", charger.maximumVoltage, {
                shouldValidate: true,
            });
        if (charger?.trickleThreshold)
            setValue("trickleThreshold", charger.trickleThreshold, {
                shouldValidate: true,
            });
        if (charger?.batterySize)
            setValue("batterySize", charger.batterySize, {
                shouldValidate: true,
            });
        if (charger?.overheatedTemperature)
            setValue("overheatedTemperature", charger.overheatedTemperature, {
                shouldValidate: true,
            });
        if (charger?.highPowerEnabled)
            setValue("highPowerEnabled", charger.highPowerEnabled, {
                shouldValidate: true,
            });
        if (charger?.usbMode)
            setValue("usbMode", charger.usbMode, {
                shouldValidate: true,
            });
        if (charger?.overheatedHysteresis)
            setValue("overheatedHysteresis", charger.overheatedHysteresis, {
                shouldValidate: true,
            });
    }, [charger]);

    return (
        <>
            <ElementWrapper header="Configure Charger">
                <form onSubmit={handleSubmit(onSubmit)}>
                    <Controller
                        control={control}
                        name="usbMode"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Usb Mode: </td>
                                    <td>
                                        <Select
                                            error={!!errors?.usbMode}
                                            {...field}
                                        >
                                            <MenuItem value={0}>2.5 W</MenuItem>
                                            <MenuItem value={1}>5 W</MenuItem>
                                            <MenuItem value={2}>10 W</MenuItem>
                                        </Select>
                                    </td>
                                </tr>
                            </table>
                        )}
                    />
                    <Controller
                        control={control}
                        name="currentTarget"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Current Target: </td>
                                    <td>
                                        <TextField
                                            error={!!errors?.currentTarget}
                                            helperText={
                                                errors?.currentTarget?.message
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
                        name="thresholdVoltage"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Threshold Voltage: </td>
                                    <td>
                                        <TextField
                                            error={!!errors?.thresholdVoltage}
                                            helperText={
                                                errors?.thresholdVoltage
                                                    ?.message
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
                        name="maximumVoltage"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Maximum Voltage: </td>
                                    <td>
                                        <TextField
                                            error={!!errors?.maximumVoltage}
                                            helperText={
                                                errors?.maximumVoltage?.message
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
                        name="trickleThreshold"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Trickle Threshold: </td>
                                    <td>
                                        <TextField
                                            error={!!errors?.trickleThreshold}
                                            helperText={
                                                errors?.trickleThreshold
                                                    ?.message
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
                        name="batterySize"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Battery Size: </td>
                                    <td>
                                        <TextField
                                            error={!!errors?.batterySize}
                                            helperText={
                                                errors?.batterySize?.message
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
                        name="overheatedTemperature"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Overheated Temperature: </td>
                                    <td>
                                        <TextField
                                            error={
                                                !!errors?.overheatedTemperature
                                            }
                                            helperText={
                                                errors?.overheatedTemperature
                                                    ?.message
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
                        name="highPowerEnabled"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>High Power Enabled: </td>
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
                        name="overheatedHysteresis"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Overheated Hysteresis: </td>
                                    <td>
                                        <TextField
                                            error={
                                                !!errors?.overheatedHysteresis
                                            }
                                            helperText={
                                                errors?.overheatedHysteresis
                                                    ?.message
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
