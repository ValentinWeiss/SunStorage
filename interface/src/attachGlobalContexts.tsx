import { LocalizationProvider } from "@mui/x-date-pickers";
import { AdapterDateFns } from "@mui/x-date-pickers/AdapterDateFns";
import { FC, PropsWithChildren } from "react";
import {
    ESPStateContext,
    ESPStateContextProvider,
} from "./context/ESPStateContext";
import { SimulationContextProvider } from "./pages";

export const AttachGlobalContexts: FC<PropsWithChildren> = ({
    children,
}: PropsWithChildren) => {
    return (
        <LocalizationProvider dateAdapter={AdapterDateFns}>
            <ESPStateContextProvider>
                <SimulationContextProvider>
                    {children}
                </SimulationContextProvider>
            </ESPStateContextProvider>
        </LocalizationProvider>
    );
};
