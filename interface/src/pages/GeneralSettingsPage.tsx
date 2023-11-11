import { useTitle } from "../layouts";
import { ConfigureWifi, GeneralConfig, ShutdownConfig } from "../components";

export function GeneralSettingsPage() {
    useTitle("General Settings");
    return (
        <>
            <GeneralConfig />
            <ShutdownConfig />
            <ConfigureWifi />
        </>
    );
}
