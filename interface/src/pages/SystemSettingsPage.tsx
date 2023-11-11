import { useTitle } from "../layouts";
import {
    ChargerConfig,
    DisplayConfig,
    NightShutdownConfig,
    PositionConfig,
} from "../components";

export function SystemSettingsPage() {
    useTitle("System Settings");
    return (
        <>
            <PositionConfig />
            <ChargerConfig />
            <NightShutdownConfig />
            <DisplayConfig />
        </>
    );
}
