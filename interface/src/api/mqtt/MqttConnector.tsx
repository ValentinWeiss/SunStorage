import React, { useEffect, useState, useMemo, useRef } from "react";
import { connect, MqttClient } from "precompiled-mqtt";
import MqttContext from "./MqttContext";
import { Error, ConnectorProps, IMqttContext } from "./MqttTypes";

export default function MqttConnector({
    children,
    brokerUrl,
    options = { keepalive: 0 },
}: ConnectorProps) {
    const clientValid = useRef(false);
    const [connectionStatus, setStatus] = useState<string | Error>("Offline");
    const [client, setClient] = useState<MqttClient | null>(null);

    useEffect(() => {
        if (!client && !clientValid.current) {
            clientValid.current = true;
            setStatus("Connecting");
            console.debug(`attempting to connect to ${brokerUrl}`);
            const mqtt = connect(brokerUrl, options);
            mqtt.on("connect", () => {
                console.debug("Mqtt connect");
                setStatus("Connected");
                setClient(mqtt);
            });
            mqtt.on("reconnect", () => {
                console.debug("Mqtt reconnect");
                setStatus("Reconnecting");
            });
            mqtt.on("error", (err) => {
                console.error(`Mqtt Connection error: ${err}`);
                setStatus(err.message);
            });
            mqtt.on("offline", () => {
                console.debug("Mqtt offline");
                setStatus("Offline");
            });
            mqtt.on("end", () => {
                console.debug("Mqtt end");
                setStatus("Offline");
            });
        }
    }, [client, clientValid, brokerUrl, options]);

    useEffect(
        () => () => {
            if (client) {
                console.debug("closing mqtt client");
                client.end(true);
                setClient(null);
                clientValid.current = false;
            }
        },
        [client, clientValid]
    );

    const value: IMqttContext = useMemo<IMqttContext>(
        () => ({
            connectionStatus,
            client,
        }),
        [connectionStatus, client]
    );

    return (
        <MqttContext.Provider value={value}>{children}</MqttContext.Provider>
    );
}
