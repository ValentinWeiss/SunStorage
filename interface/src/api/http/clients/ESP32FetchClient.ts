import {
    ConfigureWifiParams,
    batteryConfiguration,
    chargerConfiguration,
    positioningConfiguration,
    displayConfiguration,
    shutdownConfiguration,
    nightShutdownConfiguration,
} from "../../../data";
import { databaseInformationAllParams } from "../../../data/databaseInformation";
import { formatDateYYYYMMDDHHmmss } from "../../../utils";
import { HttpFetchClient } from "./../HttpFetchClient";

export class ESP32FetchClient {
    static client?: HttpFetchClient;

    static Init(baseUrl?: string) {
        ESP32FetchClient.client = new HttpFetchClient(baseUrl, "/api/v1");
    }

    static async GetSystemInfo(): Promise<Response> {
        return await ESP32FetchClient.client!.Get({
            request: "system",
        });
    }

    static async GetWifiSsids(): Promise<Response> {
        return await ESP32FetchClient.client!.Get({
            request: "wifi/list",
        });
    }

    static async SetWifiSettings(
        params: ConfigureWifiParams
    ): Promise<Response> {
        return await ESP32FetchClient.client!.Post({
            request: "wifi/configure",
            data: JSON.stringify(params),
        });
    }

    static async GetESPState(): Promise<Response> {
        return await ESP32FetchClient.client!.Get({
            request: "state",
        });
    }

    static async SetESPState(
        params:
            | chargerConfiguration
            | positioningConfiguration
            | batteryConfiguration
            | displayConfiguration
            | nightShutdownConfiguration
    ): Promise<Response> {
        return await ESP32FetchClient.client!.Post({
            request: "state",
            data: JSON.stringify(params),
        });
    }

    static async ReclaibrateStateOfCharge(): Promise<Response> {
        return await ESP32FetchClient.client!.Get({
            request: "battery/calibrate",
        });
    }

    static async Shutdown(params: shutdownConfiguration): Promise<Response> {
        return await ESP32FetchClient.client!.Post({
            request: "shutdown",
            data: JSON.stringify(params),
        });
    }

    static async GetDatabaseAll(
        params: databaseInformationAllParams
    ): Promise<Response> {
        return await ESP32FetchClient.GetDatabase({
            startTimestamp: formatDateYYYYMMDDHHmmss(params.startTime),
            endTimestamp: params?.endTime
                ? formatDateYYYYMMDDHHmmss(params.endTime)
                : "0",
            getTimestamp: true,
            getGpsLat: true,
            getGpsLon: true,
            getBatteryLevel: true,
            getBatteryTemperature: true,
            getChargerState: true,
            getCompassBearing: true,
            getCurrentIn: true,
            getCurrentOut: true,
            getGpsSatellitesInView: true,
            getGpsUsedSatellites: true,
            getGyroscopeAccelX: true,
            getGyroscopeAccelY: true,
            getGyroscopeAccelZ: true,
            getGyroscopeTemperature: true,
            getMagneticVariation: true,
            getVoltageHigherCell: true,
            getVoltageLowerCell: true,
        });
    }

    static async GetDatabase(object: any): Promise<Response> {
        return await ESP32FetchClient.client!.Post({
            request: "database",
            data: JSON.stringify(object),
        });
    }

    static async SetTimestamp(dateTime: Date): Promise<Response> {
        return await ESP32FetchClient.client!.Post({
            request: "timestamp",
            data: JSON.stringify({
                timestamp: dateTime.getTime(),
            }),
        });
    }
}
