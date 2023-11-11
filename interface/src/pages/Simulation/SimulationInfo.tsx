import { Box, Typography } from "@mui/material";
import { useContext, useEffect, useState } from "react";
import {
    GetAngle,
    ISimulationContext,
    radiansPerDegree,
    Round,
    SimulationContext,
} from ".";
import { HourFloatToString } from "../../utils";

const hundredPercent = 100;
const quaterCircleDegrees = 90;

export const SimulationInfo = () => {
    const { showSimulationInfo, sunDirection, positioning } =
        useContext<ISimulationContext>(SimulationContext);

    const [solarPanelEfficiently, setSolarPanelEfficiently] =
        useState<number>();

    useEffect(() => {
        const angle = GetAngle(sunDirection?.direction, {azimuthAngle: positioning.azimuth!, elevationAngle: positioning.elevation!});

        if (
            angle < quaterCircleDegrees &&
            sunDirection &&
            sunDirection.direction.elevationAngle >= 0
        ) {
            // setSolarPanelEfficiently(
            //     (hundredPercent / quaterCircleDegrees) *
            //         (quaterCircleDegrees - angle)
            setSolarPanelEfficiently(
                Math.cos(angle * radiansPerDegree) * hundredPercent
            );
            //);
        } else {
            setSolarPanelEfficiently(0);
        }
    }, [sunDirection, positioning]);

    return (
        <>
            {showSimulationInfo && (
                <Box
                    sx={{
                        position: "absolute",
                        padding: "5px",
                        zIndex: 50,
                    }}
                >
                    <Typography variant="body2">Simulation Info:</Typography>
                    <Typography variant="body2">
                        Day: {Round(sunDirection?.dayOfYear)}
                    </Typography>
                    <Typography variant="body2">
                        LocalTime: {Round(sunDirection?.localTime)}{" "}
                        {sunDirection?.localTime &&
                            "(" +
                                HourFloatToString(sunDirection?.localTime) +
                                ")"}
                    </Typography>
                    <Typography variant="body2">
                        EquationOfTime: {Round(sunDirection?.equationOfTime)}
                    </Typography>
                    <Typography variant="body2">
                        TimeCorrectionFactor:{" "}
                        {Round(sunDirection?.timeCorrectionFactor)}
                    </Typography>
                    <Typography variant="body2">
                        LocalSolarTime: {Round(sunDirection?.localSolarTime)}{" "}
                        {sunDirection?.localSolarTime &&
                            "(" +
                                HourFloatToString(
                                    sunDirection?.localSolarTime
                                ) +
                                ")"}
                    </Typography>
                    <Typography variant="body2">
                        HourAngle: {Round(sunDirection?.hourAngle)}
                    </Typography>
                    <Typography variant="body2">
                        DeclinationAngle:{" "}
                        {Round(sunDirection?.declinationAngle)}
                    </Typography>
                    <Typography variant="body2">
                        Sun Azimuth:{" "}
                        {Round(sunDirection?.direction?.azimuthAngle)}
                    </Typography>
                    <Typography variant="body2">
                        Sun Elevation:{" "}
                        {Round(sunDirection?.direction?.elevationAngle)}
                    </Typography>
                    <Typography variant="body2">
                        SPanel Efficiency: {Round(solarPanelEfficiently)} %
                    </Typography>
                </Box>
            )}
        </>
    );
};
