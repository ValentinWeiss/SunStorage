import React, { useState } from "react";
import mqtt, { QoS } from "precompiled-mqtt";
import { MqttQosOption, PublishMessage } from "./MqttTestGui";
import {
    Typography,
    Button,
    Grid,
    Paper,
    TextField,
    Select,
    SelectChangeEvent,
    MenuItem,
    TextareaAutosize,
} from "@mui/material";

interface SubscribeInformation {
    topic: string;
    qos: QoS;
}

const MqttPublish = ({
    publish,
}: {
    publish: (msg: PublishMessage) => void;
}) => {
    const record: PublishMessage = {
        topic: "testtopic/react",
        qos: 0,
        payload: "",
    };

    const [inputs, setInputs] = useState<PublishMessage>(record);
    const qosOptions = React.useContext(MqttQosOption);

    const handleChange = (
        e:
            | React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>
            | SelectChangeEvent<QoS>
    ): void => {
        setInputs((prevState) => ({
            ...prevState,
            [e.target.name]: e.target.value,
        }));
    };

    const handleSubmit = (e: React.FormEvent<HTMLFormElement>): void => {
        e.preventDefault();
        publish(inputs);
    };

    return (
        <Paper elevation={10} sx={{ padding: "10px", minHeight: "100px" }}>
            <Typography variant="h4" gutterBottom>
                Publish
            </Typography>
            <form onSubmit={handleSubmit}>
                <Grid>
                    <TextField
                        type="text"
                        label="Topic"
                        value={inputs.topic}
                        name="topic"
                        onChange={handleChange}
                        required
                    />

                    <Select
                        name="qos"
                        value={inputs.qos}
                        label="QoS"
                        onChange={handleChange}
                    >
                        {qosOptions?.map((option) => {
                            return (
                                <MenuItem
                                    key={option.label}
                                    value={option.value}
                                >
                                    {option.label ?? option.value}
                                </MenuItem>
                            );
                        })}
                    </Select>

                    <TextField
                        label="Payload"
                        type="text"
                        name="payload"
                        value={inputs.payload}
                        onChange={handleChange}
                    />

                    <Button type="submit" variant="contained" color="primary">
                        Publish
                    </Button>
                </Grid>
            </form>
        </Paper>
    );
};

export default MqttPublish;
