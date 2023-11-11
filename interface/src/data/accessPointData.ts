export interface accessPointData {
    ssid: string;
    rssi: number;
    authmode: number;
    channel: number;
}

export interface accessPointDataWrapper {
    accessPoints: accessPointData[];
}
