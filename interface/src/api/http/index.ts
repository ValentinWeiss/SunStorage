import { ESP32FetchClient } from "./clients";

export * from "./HttpTypes";
export * from "./clients";

export const HttpInit = async (baseUrl?: string): Promise<void> => {
    ESP32FetchClient.Init(baseUrl);
};
