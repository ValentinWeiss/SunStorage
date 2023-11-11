import { ElementWrapper } from "../";
import { useFetch } from "../../hooks";
import { handleFetchError } from "../../utils";
import { ESP32FetchClient } from "../../api/http/clients/ESP32FetchClient";
import { systemData } from "../../data";

export function SystemInfo() {
    const { data: systemData } = useFetch<systemData>(
        ESP32FetchClient.GetSystemInfo,
        {} as systemData,
        handleFetchError,
        true
    );

    return (
        <>
            <ElementWrapper header="System Info">
                <table>
                    <tbody>
                        <tr>
                            <td>Available Heap: </td>
                            <td>{systemData.availableHeap}</td>
                        </tr>
                        <tr>
                            <td>Available Internal Heap: </td>
                            <td>{systemData.availableInternalHeap}</td>
                        </tr>
                        <tr>
                            <td>Cores: </td>
                            <td>{systemData.cores}</td>
                        </tr>
                        <tr>
                            <td>Features: </td>
                            <td>{systemData.features}</td>
                        </tr>
                        <tr>
                            <td>Mac: </td>
                            <td>{systemData.mac}</td>
                        </tr>
                        <tr>
                            <td>Minimum Heap: </td>
                            <td>{systemData.minimumHeap}</td>
                        </tr>
                        <tr>
                            <td>Model: </td>
                            <td>{systemData.model}</td>
                        </tr>
                        <tr>
                            <td>Version: </td>
                            <td>{systemData.version}</td>
                        </tr>
                    </tbody>
                </table>
            </ElementWrapper>
        </>
    );
}
