import {
    Box,
    MenuItem,
    Select,
    TextField,
    Checkbox,
    FormControlLabel,
    Button,
    SelectChangeEvent,
    Typography,
} from "@mui/material";
import { FC, useContext, useEffect, useState } from "react";
import { GridType, ISimulationContext, SimulationMode } from ".";
import { SimulationContext } from "./SimulationContext";
import { handleChangeObj, handleChangeVal } from "../../utils";
import { MobileDateTimePicker } from "@mui/x-date-pickers";
import * as Yup from "yup";
import { ESP32FetchClient } from "../../api/http/clients/ESP32FetchClient";
import {
    positioningConfiguration,
    positioningConfigurationValidationSchema,
} from "../../data";

export const SubMenuSimulation: FC = () => {
    const {
        showSimulationInfo,
        setShowSimulationInfo,
        animationSpeed,
        setAnimationSpeed,
        gridType,
        setGridType,
        gridSize,
        setGridSize,
        simulationMode,
        setSimulationMode,
        date,
        setDate,
        timeZoneOffsetInHours,
        setTimeZoneOffsetInHours,
        positioning,
        setPositioning,
    } = useContext<ISimulationContext>(SimulationContext);

    const [errorMessages, setErrorMessages] = useState<string[]>([]);

    const handleGridTypeChange = (event: SelectChangeEvent<GridType>) => {
        const g = event.target.value as GridType;
        setGridType(g);
    };

    const handleSimulationModeChange = (
        event: SelectChangeEvent<SimulationMode>
    ) => {
        const s = event.target.value as SimulationMode;
        setSimulationMode(s);
    };

    useEffect(() => {
        if (setAnimationSpeed) setAnimationSpeed(0);
        if (setGridType) setGridType(GridType.Simulation);
        if (setGridSize) setGridSize(15);
    }, [simulationMode]);

    const onSetPanelAngleClick = async () => {
        var spc: positioningConfiguration = {
            manualMode: positioning?.manualMode,
            azimuth: positioning?.azimuth,
            elevation: positioning?.elevation,
        };

        if (!spc.manualMode) {
            spc.azimuth = undefined;
            spc.elevation = undefined;
        }

        console.log(spc);

        positioningConfigurationValidationSchema
            .validate(spc, {
                abortEarly: false,
            })
            .then(() => {
                setErrorMessages([]);
                ESP32FetchClient.SetESPState(spc);
            })
            .catch((errors) => {
                // Validation failed
                console.log("Validation failed");
                const errorMessages = errors.inner.map(
                    (error) => error.message
                );
                console.log(errorMessages);
                setErrorMessages(errorMessages);
            });
    };

    return (
        <>
            <Box
                sx={{
                    display: "flex",
                    justifyContent: "center",
                }}
            >
                <FormControlLabel
                    control={
                        <Checkbox
                            value={showSimulationInfo}
                            onChange={(
                                e: React.ChangeEvent<HTMLInputElement>
                            ) => {
                                setShowSimulationInfo(e.target.checked);
                            }}
                        />
                    }
                    label="Show Simulation Info"
                />
            </Box>
            <Box>
                <Select
                    value={simulationMode}
                    onChange={handleSimulationModeChange}
                    fullWidth
                >
                    {Object.keys(SimulationMode).map((x) => (
                        <MenuItem
                            key={x}
                            value={
                                SimulationMode[x as keyof typeof SimulationMode]
                            }
                        >
                            {x}
                        </MenuItem>
                    ))}
                </Select>
            </Box>
            {simulationMode === SimulationMode.Playground && (
                <>
                    <Box>
                        <TextField
                            label="Animation Speed"
                            type="number"
                            onChange={(e) =>
                                handleChangeVal(e, setAnimationSpeed)
                            }
                            value={animationSpeed}
                            fullWidth
                        />
                    </Box>
                    <Select
                        value={gridType}
                        onChange={handleGridTypeChange}
                        fullWidth
                    >
                        {Object.keys(GridType).map((x) => (
                            <MenuItem
                                key={x}
                                value={GridType[x as keyof typeof GridType]}
                            >
                                {x}
                            </MenuItem>
                        ))}
                    </Select>
                    <Box>
                        <TextField
                            label="Grid Size"
                            type="number"
                            onChange={(e) => handleChangeVal(e, setGridSize)}
                            value={gridSize}
                            fullWidth
                        />
                    </Box>
                </>
            )}
            <Box>
                <TextField
                    label="Longitude"
                    name="longitude"
                    type="number"
                    onChange={(e) => handleChangeObj(e, setPositioning)}
                    value={positioning?.longitude}
                    fullWidth
                    disabled={simulationMode !== SimulationMode.Playground}
                />
            </Box>
            <Box>
                <TextField
                    label="Latitude"
                    name="latitude"
                    type="number"
                    onChange={(e) => handleChangeObj(e, setPositioning)}
                    value={positioning?.latitude}
                    fullWidth
                    disabled={simulationMode !== SimulationMode.Playground}
                />
            </Box>
            <Box>
                <MobileDateTimePicker
                    format="dd.MM.yyyy HH:mm"
                    value={date}
                    ampm={false}
                    label="Date Time"
                    onChange={(e) => e && setDate(e)}
                    sx={{
                        width: "100%",
                    }}
                    disabled={simulationMode !== SimulationMode.Playground}
                />
            </Box>
            <Box>
                <TextField
                    label="TimeZoneOffsetInHours"
                    type="number"
                    onChange={(e) =>
                        handleChangeVal(e, setTimeZoneOffsetInHours)
                    }
                    value={timeZoneOffsetInHours}
                    fullWidth
                    disabled={simulationMode !== SimulationMode.Playground}
                />
            </Box>
            <Box>
                <TextField
                    label="Panel: Azimuth Angle"
                    type="number"
                    name="azimuth"
                    onChange={(e) => handleChangeObj(e, setPositioning)}
                    value={positioning?.azimuth}
                    fullWidth
                    disabled={!positioning?.manualMode}
                    InputLabelProps={{ shrink: true }}
                />
                <TextField
                    label="Panel: Elevation Angle"
                    type="number"
                    name="elevation"
                    onChange={(e) => handleChangeObj(e, setPositioning)}
                    value={positioning?.elevation}
                    fullWidth
                    disabled={!positioning?.manualMode}
                    InputLabelProps={{ shrink: true }}
                />
                <Box
                    sx={{
                        display: "flex",
                        justifyContent: "center",
                    }}
                >
                    <FormControlLabel
                        control={
                            <Checkbox
                                value={!positioning?.manualMode}
                                onChange={(
                                    e: React.ChangeEvent<HTMLInputElement>
                                ) => {
                                    setPositioning((prevState) => ({
                                        ...prevState,
                                        manualMode: !e.target.checked,
                                    }));
                                }}
                            />
                        }
                        label="Best Panel Angle"
                    />
                </Box>
                {simulationMode === SimulationMode.Device && (
                    <>
                        <Box>
                            {errorMessages.map((errorMessage) => (
                                <Typography sx={{ color: "red" }}>
                                    {errorMessage}
                                </Typography>
                            ))}
                        </Box>
                        <Box>
                            <Button
                                variant="contained"
                                onClick={onSetPanelAngleClick}
                                fullWidth
                            >
                                Set Panel Angle
                            </Button>
                        </Box>
                    </>
                )}
            </Box>
        </>
    );
};
