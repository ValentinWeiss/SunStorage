import { GetBaseUrl, URLJoin } from "../../utils";

export enum SseMessageType {
    Test = 0,
    Echo = 1,
}

export interface SseMessage {
    type: SseMessageType;
    data: any;
}

export type SseEvent = (data: any) => void;

export class HttpSseClient {
    private subscriptions: Map<SseMessageType, SseEvent[]> = new Map();
    private source?: EventSource;

    Connect(baseUrl?: string) {
        if (this.source) this.source.close();
        const url = GetBaseUrl(baseUrl);
        this.source = new EventSource(URLJoin(url, "/api/sse"));

        this.source.onmessage = (event) => {
            var msg = JSON.parse(event.data) as SseMessage;

            if (this.subscriptions.has(msg.type)) {
                this.subscriptions.get(msg.type)!.forEach((fn) => {
                    fn(msg.data);
                });
            }
        };

        this.source.onerror = (event) => {
            console.log(event);
        };
    }

    Disconnect(): void {
        this.source?.close();
    }

    Unsubscribe(eventType: SseMessageType, callback: SseEvent) {
        const evSub = this.subscriptions.get(eventType);
        if (evSub) {
            const index = evSub.indexOf(callback, 0);
            if (index > -1) {
                evSub.splice(index, 1);
            }

            if (evSub.length === 0) this.subscriptions.delete(eventType);
        }
    }

    Subscribe(eventType: SseMessageType, callback: SseEvent) {
        if (!this.subscriptions.has(eventType)) {
            this.subscriptions.set(eventType, []);
        }
        const evSub = this.subscriptions.get(eventType);
        if (evSub!.indexOf(callback, 0) > -1)
            // if already subscribed
            return;

        this.subscriptions.get(eventType)?.push(callback);
    }
}
