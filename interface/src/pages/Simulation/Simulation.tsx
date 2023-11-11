import { FC, Suspense, useState, useContext, useEffect } from "react";
import { Canvas, useFrame } from "@react-three/fiber";
import { OrbitControls, Text } from "@react-three/drei";
import * as THREE from "three";
import { useTitle } from "../../layouts";
import {
    ISimulationContext,
    SimulationContext,
    CalculateSunDirection,
    GetPositionFromDirection,
    SimulationInfo,
} from ".";
import { Grid, Sun, SolarPanel } from "./SimulationObjects";
import { Box } from "@mui/material";

const SimulationContent: FC = () => {
    const {
        animationSpeed,
        gridSize: size,
        date,
        setDate,
        timeZoneOffsetInHours,
        sunDirection,
        setSunDirection,
        positioning,
        setPositioning,
    } = useContext<ISimulationContext>(SimulationContext);

    const [sunPosition, setSunPosition] = useState<THREE.Vector3>(
        new THREE.Vector3(0, 0, 0)
    );

    const [simulationDate, setSimulationDate] = useState<Date>(new Date());

    useEffect(() => {
        setSimulationDate(date);
    }, [date]);

    const UpdateSunPosition = () => {
        if (
            (!positioning?.longitude && positioning?.longitude !== 0) ||
            (!positioning?.latitude && positioning?.latitude !== 0)
        )
            return;

        setSunDirection(
            CalculateSunDirection(
                {
                    latitude: positioning.latitude,
                    longitude: positioning.longitude,
                },
                simulationDate,
                timeZoneOffsetInHours
            )
        );

        if (!sunDirection) return;

        if (!positioning?.manualMode)
            setPositioning((prevState) => ({
                ...prevState,
                azimuth: sunDirection.direction.azimuthAngle,
                elevation: sunDirection.direction.elevationAngle,
            }));

        setSunPosition(
            GetPositionFromDirection(sunDirection.direction, size / 2)
        );
    };

    useFrame((_, delta) => {
        if (animationSpeed !== 0) {
            setSimulationDate(
                new Date(
                    simulationDate.getTime() + animationSpeed * 60000 * delta
                )
            );
        }

        UpdateSunPosition();
    });

    return (
        <>
            <OrbitControls />
            <ambientLight intensity={10} />
            <Grid />
            <Sun sunPosition={sunPosition} />
            <SolarPanel
                direction={{
                    azimuthAngle: positioning!.azimuth!,
                    elevationAngle: positioning!.elevation!,
                }}
            />
        </>
    );
};

export const Simulation: FC = () => {
    useTitle("Simulation");

    return (
        <Box
            sx={{
                width: "100%",
                height: "100%",
            }}
        >
            <SimulationInfo />
            <Canvas camera={{ position: [0, 5, 13] }} shadows>
                {/* <Suspense fallback={<Text>Loading</Text>}> */}
                <SimulationContent />
                {/* </Suspense> */}
            </Canvas>
        </Box>
    );
};
