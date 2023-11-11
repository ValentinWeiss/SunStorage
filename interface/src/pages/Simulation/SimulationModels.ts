import { positioningConfiguration } from "../../data";

export interface Location {
    longitude: number;
    latitude: number;
}

export interface Direction {
    azimuthAngle: number;
    elevationAngle: number;
}

export interface SunDirection {
    dayOfYear: number;
    localTime: number;
    equationOfTime: number;
    timeCorrectionFactor: number;
    localSolarTime: number;
    hourAngle: number;
    declinationAngle: number;
    direction: Direction;
}

export enum GridType {
    Test = "Test",
    Simulation = "Simulation",
}

export enum SimulationMode {
    Device = "Device",
    Playground = "Playground",
}

export interface ISimulationContext {
    showSimulationInfo: boolean;
    setShowSimulationInfo: React.Dispatch<React.SetStateAction<boolean>>;

    // how many minutes move the sun per second of simulation
    animationSpeed: number;
    setAnimationSpeed: React.Dispatch<React.SetStateAction<number>>;

    gridType: GridType;
    setGridType: React.Dispatch<React.SetStateAction<GridType>>;

    gridSize: number;
    setGridSize: React.Dispatch<React.SetStateAction<number>>;

    simulationMode: SimulationMode;
    setSimulationMode: React.Dispatch<React.SetStateAction<SimulationMode>>;

    date: Date;
    setDate: React.Dispatch<React.SetStateAction<Date>>;

    timeZoneOffsetInHours: number;
    setTimeZoneOffsetInHours: React.Dispatch<React.SetStateAction<number>>;

    positioning: positioningConfiguration;
    setPositioning: React.Dispatch<
        React.SetStateAction<positioningConfiguration>
    >;

    sunDirection?: SunDirection;
    setSunDirection: React.Dispatch<
        React.SetStateAction<SunDirection | undefined>
    >;
}
