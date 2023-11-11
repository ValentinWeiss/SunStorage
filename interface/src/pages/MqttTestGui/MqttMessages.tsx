import React, { useState } from "react";
import mqtt from "precompiled-mqtt";
import { Typography, Button, Grid, Paper, TextField } from "@mui/material";
import { Message } from "./MqttTestGui";

interface ConnectionInformation {
    host: string;
    clientId: string;
    port: number;
    username?: string;
    password?: string;
}

const MqttMessages = ({ messages }: { messages: Message[] }) => {
    return (
        <Paper elevation={10} sx={{ padding: "10px", minHeight: "100px" }}>
            <Typography variant="h4" gutterBottom>
                Messages
            </Typography>
            {messages?.map((t) => {
                return (
                    <div key={t.id}>
                        <Typography
                            sx={{ display: "inline-block", fontWeight: "bold" }}
                            variant="body2"
                        >
                            {t.topic}:
                        </Typography>
                        <span> </span>
                        <Typography
                            sx={{ display: "inline-block" }}
                            variant="body2"
                        >
                            {t.message}
                        </Typography>
                    </div>
                );
            })}
        </Paper>
    );
};

export default MqttMessages;
