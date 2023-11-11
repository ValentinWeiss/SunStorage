import React, { useState } from "react";
import mqtt from "precompiled-mqtt";
import { Typography, Button, Grid, Paper, TextField } from "@mui/material";

interface ConnectionInformation {
    host: string;
    clientId: string;
    port: number;
    username?: string;
    password?: string;
}

const MqttConnection = ({
    connect,
    disconnect,
    connectionStatus,
}: {
    connect: (host: string, mqttOption?: mqtt.IClientOptions) => void;
    disconnect: () => void;
    connectionStatus: string;
}) => {
    const record: ConnectionInformation = {
        host: "broker.emqx.io",
        clientId: `mqttjs_${Math.random().toString(16).substr(2, 8)}`,
        port: 8083,
    };

    const [inputs, setInputs] = useState(record);

    const handleChange = (e: React.ChangeEvent<HTMLInputElement>): void => {
        setInputs((prevState) => ({
            ...prevState,
            [e.target.name]: e.target.value,
        }));
    };

    const handleSubmit = (e: React.FormEvent<HTMLFormElement>): void => {
        e.preventDefault();

        const { host, clientId, port, username, password } = inputs;
        const url = `ws://${host}:${port}/mqtt`;
        const options: mqtt.IClientOptions = {
            keepalive: 30,
            protocolId: "MQTT",
            protocolVersion: 4,
            clean: true,
            reconnectPeriod: 1000,
            connectTimeout: 30 * 1000,
            will: {
                topic: "WillMsg",
                payload: "Connection Closed abnormally..!",
                qos: 0,
                retain: false,
            },
            rejectUnauthorized: false,
            clientId: clientId,
            username: username,
            password: password,
        };

        connect(url, options);
    };

    const handleDisconnect = (e: React.MouseEvent<HTMLButtonElement>): void => {
        disconnect();
    };

    return (
        <Paper elevation={10} sx={{ padding: "10px", minHeight: "100px" }}>
            <Typography variant="h4" gutterBottom>
                Connection
            </Typography>
            <form onSubmit={handleSubmit}>
                <Grid>
                    <TextField
                        type="text"
                        label="Host"
                        value={inputs.host}
                        name="host"
                        onChange={handleChange}
                        required
                    />
                    <TextField
                        type="text"
                        label="Client ID"
                        value={inputs.clientId}
                        name="clientId"
                        onChange={handleChange}
                        required
                    />
                    <TextField
                        type="number"
                        label="Port"
                        value={inputs.port}
                        name="port"
                        onChange={handleChange}
                        required
                    />
                    <TextField
                        type="text"
                        label="Username"
                        value={inputs.username}
                        name="username"
                        onChange={handleChange}
                    />
                    <TextField
                        type="password"
                        label="Password"
                        value={inputs.password}
                        name="password"
                        onChange={handleChange}
                    />
                    <Button type="submit" variant="contained" color="primary">
                        {connectionStatus}
                    </Button>
                    <Button
                        type="button"
                        variant="contained"
                        color="error"
                        onClick={handleDisconnect}
                    >
                        Disconnect
                    </Button>
                </Grid>
            </form>
        </Paper>
    );
};

export default MqttConnection;
