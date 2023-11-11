import { createContext } from "react";
import { IMqttContext } from "./MqttTypes";

export default createContext<IMqttContext>({} as IMqttContext);
