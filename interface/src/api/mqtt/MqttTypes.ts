import { MqttClient, IClientOptions } from "precompiled-mqtt";

export interface Error {
    name: string;
    message: string;
    stack?: string;
}

export interface ConnectorProps {
    brokerUrl: string;
    options?: IClientOptions;
    children: React.ReactNode;
}

export interface IMqttContext {
    connectionStatus: string | Error;
    client?: MqttClient | null;
}

export interface IMessageStructure {
    [key: string]: string;
}

export interface IMessage {
    topic: string;
    message?: string | IMessageStructure;
}

export interface IUseSubscription {
    topic: string | string[];
    client?: MqttClient | null;
    message?: IMessage;
    connectionStatus: string | Error;
}
