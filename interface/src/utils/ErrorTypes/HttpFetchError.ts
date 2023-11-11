export class HttpFetchError extends Error {
    response: Response;
    msg?: string;

    constructor(response: Response, msg?: string) {
        super(
            `${msg ? msg + "\n" : ""} ${response.status}${
                response.statusText ? ": " + response.statusText : ""
            }`
        );
        Object.setPrototypeOf(this, HttpFetchError.prototype);
        this.name = "HttpFetchError";
        this.msg = msg;
        this.response = response;
    }
}
