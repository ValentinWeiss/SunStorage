import { useEffect, useState } from "react";
import { httpResponseToJson } from "../utils";

export const useFetchParam = <T, P extends {}>(
    fetchFunc: (params: P) => Promise<Response>,
    initialValue: T,
    errorCallback?: (err: any) => void,
    initialFetchParams?: P,
    parseData?: (json: any) => T
) => {
    const [data, setData] = useState<T>(initialValue);

    const fetchData = (params: P) =>
        fetchFunc(params)
            .then(async (res) => await httpResponseToJson(res))
            .then((x) => {
                if (parseData) x = parseData(x);
                setData(x as T);
            })
            .catch((x) => {
                if (errorCallback) errorCallback(x);
            });

    useEffect(() => {
        if (initialFetchParams) fetchData(initialFetchParams);
    }, []);

    return { data, fetchData };
};

export const useFetch = <T>(
    fetchFunc: () => Promise<Response>,
    initialValue: T,
    errorCallback?: (err: any) => void,
    fetchOnLoad?: boolean,
    parseData?: (json: any) => T
) => {
    const [data, setData] = useState<T>(initialValue);

    const fetchData = () =>
        fetchFunc()
            .then(async (res) => await httpResponseToJson(res))
            .then((x) => {
                if (parseData) x = parseData(x);
                setData(x as T);
            })
            .catch((x) => {
                if (errorCallback) errorCallback(x);
            });

    useEffect(() => {
        if (fetchOnLoad) fetchData();
    }, []);

    return { data, fetchData };
};
