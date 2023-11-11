import { useContext } from "react";
import MqttContext from "./MqttContext";
import { IMqttContext as Context } from "./MqttTypes";

export default function useMqttState() {
    const { connectionStatus, client } = useContext<Context>(MqttContext);

    return {
        connectionStatus,
        client,
    };
}
