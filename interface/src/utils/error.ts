import { enqueueSnackbar } from "notistack";
import { HttpFetchError } from "./ErrorTypes/HttpFetchError";
import { ArgumentError } from "./ErrorTypes";

export const ThrowIfNegative = (num: number, varName: string): void => {
    if (num < 0) {
        throw new ArgumentError(`argument ${varName} is negative: ${num}`);
    }
};
export const ThrowIfNullOrUndefined = (prop: any, propName: string): void => {
    if (prop == null) {
        throw new ArgumentError(
            `argument ${propName} is null or undefined: ${prop}`
        );
    }
};

export const DefaultHttpErrorHandler = (err: any): void => {
    if (err instanceof HttpFetchError) {
        console.error(err.response);
        enqueueSnackbar(err.toString());
    } else if (err instanceof Error) {
        console.error(err);
        enqueueSnackbar(err.toString());
    } else {
        console.error(err);
        enqueueSnackbar(err.toString());
    }
};
