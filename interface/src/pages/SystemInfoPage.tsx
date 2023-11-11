import { useTitle } from "../layouts";
import { SystemInfo } from "../components";

export function SystemInfoPage() {
    useTitle("System Information");
    return (
        <>
            <SystemInfo />
        </>
    );
}
