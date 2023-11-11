export interface IFetchUrlParameters {
    serverUrl?: string;
    subUrl?: string;
    request?: string;
    parameter?: HttpQueryParameter | Record<string, string>;
    method?: "GET" | "POST" | "DELETE" | "PUT" | "PATCH";
    headers?: { [key: string]: string };
    data?: any;
}

export interface IHttpOptions {
    headers?: { [key: string]: string };
    method?: "GET" | "POST" | "DELETE" | "PUT" | "PATCH";
    body?: string;
}

export interface IFetchClientProps {
    baseUrl?: string;
    children: React.ReactNode;
}

export type HttpQueryParameter = Record<string, string | number>;
