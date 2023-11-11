import React, { useState } from "react";
import mqtt, { QoS } from "precompiled-mqtt";
import { MqttQosOption } from "./MqttTestGui";
import {
    Typography,
    Button,
    Grid,
    Paper,
    TextField,
    Select,
    SelectChangeEvent,
    MenuItem,
} from "@mui/material";

interface SubscribeInformation {
    topic: string;
    qos: QoS;
}

const MqttSubscriber = ({
    sub,
    unSub,
    topics,
}: {
    sub: ({ topic, qos }: { topic: string; qos: mqtt.QoS }) => void;
    unSub: ({ topic }: { topic: string }) => void;
    topics: string[];
}) => {
    const record: SubscribeInformation = {
        topic: "testtopic/react",
        qos: 0,
    };

    const [inputs, setInputs] = useState<SubscribeInformation>(record);
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
        sub(inputs);
    };

    const handleUnsubscribe = (topic: string): void => {
        unSub({ topic });
    };

    return (
        <Paper elevation={10} sx={{ padding: "10px", minHeight: "100px" }}>
            <Typography variant="h4" gutterBottom>
                Subscription
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

                    <Button type="submit" variant="contained" color="primary">
                        Subscribe
                    </Button>
                </Grid>
            </form>
            {topics?.map((t) => {
                return (
                    <div key={t}>
                        <Typography
                            sx={{ display: "inline-block" }}
                            variant="body2"
                        >
                            {t}
                        </Typography>
                        <Button
                            type="button"
                            sx={{ display: "inline-block" }}
                            onClick={(e) => handleUnsubscribe(t)}
                        >
                            Remove
                        </Button>
                    </div>
                );
            })}
        </Paper>
    );
};

export default MqttSubscriber;
