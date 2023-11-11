import { createContext, FC, PropsWithChildren } from "react";
import { ESP32FetchClient } from "../api/http";
import { ESPStateData } from "../data";
import { useFetch } from "../hooks";
import { handleFetchError } from "../utils";

export interface IESPStateFetchData {
    data: ESPStateData;
    fetch: () => Promise<void>;
}

export const ESPStateContext = createContext<IESPStateFetchData>({} as IESPStateFetchData);

export const ESPStateContextProvider: FC<PropsWithChildren> = ({
    children,
}: PropsWithChildren) => {
    const { data: espStateData, fetchData: fetchEspData } = useFetch<ESPStateData>(
        ESP32FetchClient.GetESPState,
        {} as ESPStateData,
        handleFetchError,
        true
    );

    return (
        <ESPStateContext.Provider value={{data: espStateData, fetch: fetchEspData}}>
            {children}
        </ESPStateContext.Provider>
    );
};
