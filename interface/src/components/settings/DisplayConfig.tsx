import { Button, Switch, TextField } from "@mui/material";
import { FlexWrapper, ElementWrapper } from "../";
import { useForm, Controller } from "react-hook-form";
import { handleFetchError, httpResponseThrowIfInvalid } from "../../utils";
import { ESP32FetchClient } from "../../api/http/clients/ESP32FetchClient";
import { displayConfiguration, ESPStateData } from "../../data";
import { useContext, useEffect } from "react";
import { ESPStateContext, IESPStateFetchData } from "../../context/ESPStateContext";

export function DisplayConfig() {
    const { data: {display}, fetch } = useContext<IESPStateFetchData>(ESPStateContext);

    const { setValue, handleSubmit, control } = useForm<displayConfiguration>({
        defaultValues: {
            d_powerSolar: false,
            d_dailyProduction: false,
            d_soc: false,
            d_voltageLowerCell: false,
            d_voltageUpperCell: false,
            d_solarCurrent: false,
            d_chargingCurrent: false,
            d_systemCurrent: false,
            d_powerSystem: false,
            d_powerCharging: false,
            d_dailyConsumption: false,
            d_thresholdVoltage: false,
            d_maximumVoltage: false,
            d_trickleThreshold: false,
            d_highPowerCircuit: false,
            d_batteryTemperature: false,
            d_batteryOverheatedHysteresis: false,
            d_batterySize: false,
            d_currentTarget: false,
            d_chargerState: false,
            d_usbMode: false,
            d_azimuth: false,
            d_elevation: false,
            d_manualAdjustment: false,
            d_nightShutdownEnabled: false,
            d_nightPositionAzimuth: false,
            d_nightPositionElevation: false,
            d_latitude: false,
            d_longitude: false,
            d_angleToNorth: false,
            d_time: false,
            d_httpDisableTimeout: false,
            d_gpsDisableTimeout: false,
            d_dcfDisableTimeout: false,
            d_wifiDisableTimeout: false,
        },
    });

    const displayConfigurationKeys = Object.keys(
        control._defaultValues
    ) as (keyof typeof control._defaultValues)[];

    const onSubmit = async (data: displayConfiguration) => {
        console.log(data);
        var result = await ESP32FetchClient.SetESPState(data)
            .then((res) => httpResponseThrowIfInvalid(res))
            .then(x => fetch())
            .catch((x) => handleFetchError(x));
        console.log(result);
    };

    useEffect(() => {
        if (display?.d_powerSolar)
            setValue("d_powerSolar", display.d_powerSolar);
        if (display?.d_dailyProduction)
            setValue("d_dailyProduction", display.d_dailyProduction);
        if (display?.d_soc) setValue("d_soc", display.d_soc);
        if (display?.d_voltageLowerCell)
            setValue("d_voltageLowerCell", display.d_voltageLowerCell);
        if (display?.d_voltageUpperCell)
            setValue("d_voltageUpperCell", display.d_voltageUpperCell);
        if (display?.d_solarCurrent)
            setValue("d_solarCurrent", display.d_solarCurrent);
        if (display?.d_chargingCurrent)
            setValue("d_chargingCurrent", display.d_chargingCurrent);
        if (display?.d_systemCurrent)
            setValue("d_systemCurrent", display.d_systemCurrent);
        if (display?.d_powerSystem)
            setValue("d_powerSystem", display.d_powerSystem);
        if (display?.d_powerCharging)
            setValue("d_powerCharging", display.d_powerCharging);
        if (display?.d_dailyConsumption)
            setValue("d_dailyConsumption", display.d_dailyConsumption);
        if (display?.d_thresholdVoltage)
            setValue("d_thresholdVoltage", display.d_thresholdVoltage);
        if (display?.d_maximumVoltage)
            setValue("d_maximumVoltage", display.d_maximumVoltage);
        if (display?.d_trickleThreshold)
            setValue("d_trickleThreshold", display.d_trickleThreshold);
        if (display?.d_highPowerCircuit)
            setValue("d_highPowerCircuit", display.d_highPowerCircuit);
        if (display?.d_batteryTemperature)
            setValue("d_batteryTemperature", display.d_batteryTemperature);
        if (display?.d_batteryOverheatedHysteresis)
            setValue(
                "d_batteryOverheatedHysteresis",
                display.d_batteryOverheatedHysteresis
            );
        if (display?.d_batterySize)
            setValue("d_batterySize", display.d_batterySize);
        if (display?.d_currentTarget)
            setValue("d_currentTarget", display.d_currentTarget);
        if (display?.d_chargerState)
            setValue("d_chargerState", display.d_chargerState);
        if (display?.d_usbMode) setValue("d_usbMode", display.d_usbMode);
        if (display?.d_azimuth) setValue("d_azimuth", display.d_azimuth);
        if (display?.d_elevation) setValue("d_elevation", display.d_elevation);
        if (display?.d_manualAdjustment)
            setValue("d_manualAdjustment", display.d_manualAdjustment);
        if (display?.d_nightShutdownEnabled)
            setValue("d_nightShutdownEnabled", display.d_nightShutdownEnabled);
        if (display?.d_nightPositionAzimuth)
            setValue("d_nightPositionAzimuth", display.d_nightPositionAzimuth);
        if (display?.d_nightPositionElevation)
            setValue(
                "d_nightPositionElevation",
                display.d_nightPositionElevation
            );
        if (display?.d_latitude) setValue("d_latitude", display.d_latitude);
        if (display?.d_longitude) setValue("d_longitude", display.d_longitude);
        if (display?.d_angleToNorth)
            setValue("d_angleToNorth", display.d_angleToNorth);
        if (display?.d_time) setValue("d_time", display.d_time);
        if (display?.d_httpDisableTimeout)
            setValue("d_httpDisableTimeout", display.d_httpDisableTimeout);
        if (display?.d_gpsDisableTimeout)
            setValue("d_gpsDisableTimeout", display.d_gpsDisableTimeout);
        if (display?.d_dcfDisableTimeout)
            setValue("d_dcfDisableTimeout", display.d_dcfDisableTimeout);
        if (display?.d_wifiDisableTimeout)
            setValue("d_wifiDisableTimeout", display.d_wifiDisableTimeout);
    }, [display]);

    return (
        <>
            <ElementWrapper header="Configure Display">
                <form onSubmit={handleSubmit(onSubmit)}>
                    {displayConfigurationKeys?.map((key) => (
                        <Controller
                            key={key}
                            control={control}
                            name={key}
                            render={({ field }) => (
                                <table>
                                    <tr>
                                        <td>{key.substr(2)}: </td>
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
                    ))}
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
