import { useContext, useEffect, useCallback, useState } from "react";
import { IClientSubscribeOptions } from "precompiled-mqtt";
import { matches } from "mqtt-pattern";
import MqttContext from "./MqttContext";
import {
    IMqttContext as Context,
    IUseSubscription,
    IMessage,
} from "./MqttTypes";

export default function useMqttSubscription(
    topic: string | string[],
    options: IClientSubscribeOptions = {} as IClientSubscribeOptions
): IUseSubscription {
    const { client, connectionStatus } = useContext<Context>(MqttContext);

    const [message, setMessage] = useState<IMessage | undefined>(undefined);

    /*
    NOTE: Right now we don't care about unsubscribing from topics we only remove message callbacks
    Add this when we have performance problems with too many subscribed topics
    multiple useMqttSubscription hooks can exist per MqttContext
    So we would need to somehow keep track of which hook needs which subscribed topic and only if no hook needs a topic only then unsubscribe
    And right now im too lazy for that
    */
    const subscribe = useCallback(async () => {
        client?.subscribe(topic, options);
    }, [client, options, topic]);

    const callback = useCallback(
        (receivedTopic: string, receivedMessage: any) => {
            if (
                [topic].flat().some((rTopic) => matches(rTopic, receivedTopic))
            ) {
                setMessage({
                    topic: receivedTopic,
                    message: receivedMessage.toString(),
                });
            }
        },
        [topic]
    );

    useEffect(() => {
        if (client?.connected) {
            subscribe();
            client.addListener("message", callback);
        }
        return () => {
            client?.removeListener("message", callback);
        };
    }, [callback, client, subscribe]);

    return {
        client,
        topic,
        message,
        connectionStatus,
    };
}
