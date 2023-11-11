import React from "react";
import mqtt from "precompiled-mqtt";
import { useMqttState, MqttConnector, useMqttSubscription } from "../api/mqtt";
import { Typography } from "@mui/material";
import { useTitle } from "../layouts";

const MqttModule = () => {
    useTitle("Mqtt Lib Test");

    const { client, connectionStatus } = useMqttState();
    const { message } = useMqttSubscription([
        "testtopic/react",
        "testtopic/react2",
    ]);

    React.useEffect(() => {
        if (message) console.log(message);
    }, [message]);

    //client?.publish("testtopic/react", "Testmsg1");

    return (
        <Typography variant="h4" gutterBottom>
            {"Status:" + connectionStatus}
        </Typography>
    );
};

export function MqttLibTest() {
    var url = "ws://broker.emqx.io:8083/mqtt";
    var options: mqtt.IClientOptions = {
        clientId: `mqttjs_${Math.random().toString(16).substr(2, 8)}`,
        username: undefined,
        password: undefined,
    };

    return (
        <MqttConnector brokerUrl={url} options={options}>
            <MqttModule />
        </MqttConnector>
    );
}
