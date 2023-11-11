import { createHttpQueryString, GetBaseUrl, URLJoin } from "../../utils";
import { IFetchUrlParameters, IHttpOptions } from "./HttpTypes";

export class HttpFetchClient {
    private baseUrl?: string;
    private subUrl?: string;

    constructor(baseUrl?: string, subUrl?: string) {
        this.baseUrl = GetBaseUrl(baseUrl);
        if (subUrl) this.subUrl = subUrl;
    }

    GetUrl(params: IFetchUrlParameters): string {
        let baseUrl = params.serverUrl ? params.serverUrl : this.baseUrl;
        if (baseUrl == null) throw new Error("baseUrl is not defined!");
        let subUrl = params.subUrl ? params.subUrl : this.subUrl;

        let url = URLJoin(
            baseUrl,
            subUrl,
            params.request,
            createHttpQueryString(params.parameter)
        );

        return url;
    }

    async Request(params?: IFetchUrlParameters) {
        if (params == null) params = {};
        let url = this.GetUrl(params);
        const options: IHttpOptions = {
            method: params.method ?? "GET",
        };
        if (params.headers) options.headers = params.headers;
        if (params.data) options.body = params.data;

        console.log("Fetching URL: " + url + " with options: ");
        console.log(options);

        return await fetch(url, options);
    }

    async Get(params?: IFetchUrlParameters) {
        if (params == null) params = {};
        return await this.Request({ ...params, method: "GET" });
    }

    async Post(params?: IFetchUrlParameters) {
        if (params == null) params = {};
        return await this.Request({ ...params, method: "POST" });
    }

    async Delete(params?: IFetchUrlParameters) {
        if (params == null) params = {};
        return await this.Request({ ...params, method: "DELETE" });
    }

    async Patch(params?: IFetchUrlParameters) {
        if (params == null) params = {};
        return await this.Request({ ...params, method: "PATCH" });
    }

    async Put(params?: IFetchUrlParameters) {
        if (params == null) params = {};
        return await this.Request({ ...params, method: "PUT" });
    }
}
