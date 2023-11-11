import {
    createContext,
    FC,
    PropsWithChildren,
    useContext,
    useEffect,
    useState,
} from "react";
import { GridType, ISimulationContext, SimulationMode, SunDirection } from ".";
import { ESPStateData, positioningConfiguration } from "../../data";
import { ESPStateContext, IESPStateFetchData } from "../../context/ESPStateContext";

export const SimulationContext = createContext<ISimulationContext>(
    {} as ISimulationContext
);

export const SimulationContextProvider: FC<PropsWithChildren> = ({
    children,
}: PropsWithChildren) => {
    const { data: {positioning: position}, fetch } = useContext<IESPStateFetchData>(ESPStateContext);

    const [showSimulationInfo, setShowSimulationInfo] =
        useState<boolean>(false);
    const [animationSpeed, setAnimationSpeed] = useState<number>(50);
    const [gridType, setGridType] = useState<GridType>(GridType.Simulation);
    const [gridSize, setGridSize] = useState<number>(15);
    const [simulationMode, setSimulationMode] = useState<SimulationMode>(
        SimulationMode.Device
    );

    const [positioning, setPositioning] = useState<positioningConfiguration>({
        azimuth: 0,
        elevation: 0,
        latitude: 0,
        longitude: 0,
        manualMode: true,
    });

    const [date, setDate] = useState<Date>(new Date());
    const [timeZoneOffsetInHours, setTimeZoneOffsetInHours] =
        useState<number>(1);

    const [sunDirection, setSunDirection] = useState<
        SunDirection | undefined
    >();

    useEffect(() => {
        if (position) {
            setPositioning(position);
        }
    }, [position]);

    return (
        <SimulationContext.Provider
            value={{
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
                sunDirection,
                setSunDirection,
            }}
        >
            {children}
        </SimulationContext.Provider>
    );
};
