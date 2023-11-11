import React, { createContext, useEffect, useState } from "react";
import mqtt from "precompiled-mqtt";
import MqttConnection from "./MqttConnection";
import MqttSubscriber from "./MqttSubscriber";
import MqttMessages from "./MqttMessages";
import MqttPublish from "./MqttPublish";
import { useTitle } from "../../layouts";

export const MqttQosOption = createContext<{ label: string; value: number }[]>(
    []
);
const mqttQosOption = [
    {
        label: "0",
        value: 0,
    },
    {
        label: "1",
        value: 1,
    },
    {
        label: "2",
        value: 2,
    },
];

export interface Message {
    id: number;
    topic: string;
    message: string;
}

export interface PublishMessage {
    topic: string;
    qos: mqtt.QoS;
    payload: string;
}

export const MqttTestGui = () => {
    useTitle("Mqtt Test Gui");

    const [client, setClient] = useState<mqtt.MqttClient>();
    const [topics, setTopics] = useState<string[]>([]);
    const [messages, setMessages] = useState<Message[]>([]);
    const [connectStatus, setConnectStatus] = useState("Connect");

    const mqttConnect = (host: string, mqttOption?: mqtt.IClientOptions) => {
        setConnectStatus("Connecting");
        setClient(mqtt.connect(host, mqttOption));
    };

    useEffect(() => {
        let idCounter = 1;
        if (client) {
            client.on("connect", () => {
                setConnectStatus("Connected");
                setTopics([]);
            });
            client.on("error", (err) => {
                console.error("Connection error: ", err);
                client.end();
            });
            client.on("reconnect", () => {
                setConnectStatus("Reconnecting");
                setTopics([]);
            });
            client.on("close", () => {
                setConnectStatus("Connect");
                setTopics([]);
            });
            client.on("message", (topic, message) => {
                const payload: Message = {
                    id: idCounter++,
                    topic,
                    message: message.toString(),
                };
                console.log(payload);

                setMessages((messages) => [...messages, payload]);
            });
        }
    }, [client]);

    const mqttDisconnect = () => {
        if (client) {
            client.end();
        }
    };

    const mqttPublish = ({ topic, qos, payload }: PublishMessage) => {
        if (client) {
            client.publish(topic, payload, { qos }, (error) => {
                if (error) {
                    console.log("Publish error: ", error);
                }
            });
        }
    };

    const mqttSub = ({ topic, qos }: { topic: string; qos: mqtt.QoS }) => {
        if (client) {
            client.subscribe(topic, { qos }, (error) => {
                if (error) {
                    console.log("Subscribe to topics error", error);
                    return;
                }
                setTopics((prev) => {
                    if (!prev.find((t) => t === topic)) return [...prev, topic];
                    return prev;
                });
            });
        }
    };

    const mqttUnSub = ({ topic }: { topic: string }) => {
        if (client) {
            client.unsubscribe(topic, undefined, (error) => {
                if (error) {
                    console.log("Unsubscribe error", error);
                    return;
                }
                setTopics((prev) => prev.filter((e) => e !== topic));
            });
        }
    };

    return (
        <div>
            <MqttConnection
                connect={mqttConnect}
                disconnect={mqttDisconnect}
                connectionStatus={connectStatus}
            />

            <MqttQosOption.Provider value={mqttQosOption}>
                <MqttSubscriber
                    sub={mqttSub}
                    unSub={mqttUnSub}
                    topics={topics}
                />
                <MqttPublish publish={mqttPublish} />
            </MqttQosOption.Provider>

            <MqttMessages messages={messages} />
        </div>
    );
};
