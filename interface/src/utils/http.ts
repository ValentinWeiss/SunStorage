import { enqueueSnackbar } from "notistack";
import { HttpFetchError } from ".";
import { HttpQueryParameter } from "../api/http";

export const GetBaseUrl = (url?: string): string => {
    if (url) return url;
    if (process.env.REACT_APP_BASE_URL)
        return process.env.REACT_APP_BASE_URL as string;
    return "";
};

export const URLJoin = (...args: (string | undefined)[]) => {
    let url = args
        .join("/")
        .replace(/[\/]+/g, "/")
        .replace(/^(.+):\//, "$1://")
        .replace(/^file:/, "file:/")
        .replace(/\/(\?|&|#[^!])/g, "$1")
        .replace(/\?/g, "&")
        .replace("&", "?");

    if (url.endsWith("/")) url = url.slice(0, -1);

    return url;
};

export const createHttpQueryString = (
    params: HttpQueryParameter | undefined
): string => {
    if (params == null) return "";

    let queryParams: string[] = [];

    let object = params as any;
    for (let prop of Object.getOwnPropertyNames(object)) {
        if (object[prop] != null)
            queryParams.push(`${prop}=${encodeURIComponent(object[prop])}`);
    }

    return `?${queryParams.join("&")}`;
};

export const httpResponseToJson = async (response: Response): Promise<any> => {
    if (!response.ok) throw new HttpFetchError(response);
    var json = await response.json();
    return json;
};

export const httpResponseThrowIfInvalid = (response: Response): void => {
    if (!response.ok) throw new HttpFetchError(response);
};

export const handleFetchError = (err: any) => {
    if (err instanceof HttpFetchError) {
        console.error(err.response);
        enqueueSnackbar(err.toString());
    } else if (err instanceof Error) {
        console.error(err);
        enqueueSnackbar(err.toString());
    } else {
        console.error(err);
        enqueueSnackbar(err.toString());
        throw err;
    }
};
