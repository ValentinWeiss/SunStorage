export const GetCurrentUtcDateString = (timeZero: boolean = false): string => {
    var date = new Date();
    if (timeZero) date.setUTCHours(0, 0, 0, 0);

    return date.toISOString();
};

export const GetCurrentLocalDateString = (
    addSeconds: boolean = false,
    addMilliseconds: boolean = false
): string => {
    return DateStringToLocalIsoDateString(
        GetCurrentUtcDateString(),
        addSeconds,
        addMilliseconds
    );
};

export const DateStringToIsoDateString = (dateString: string): string => {
    return new Date(dateString).toISOString();
};

export const DateStringToLocalIsoDateString = (
    dateString: string,
    addSeconds: boolean = false,
    addMilliseconds: boolean = false
): string => {
    var date = new Date(dateString);
    var result: string = `${date.getFullYear()}-${pad(
        date.getMonth() + 1
    )}-${pad(date.getDate())}T${pad(date.getHours())}:${pad(
        date.getMinutes()
    )}`;
    if (addSeconds || addMilliseconds) {
        result += `:${pad(date.getSeconds())}`;
    }
    if (addMilliseconds) {
        result += `.${(date.getMilliseconds() / 1000).toFixed(3).slice(2, 5)}`;
    }

    return result;
};

export const HourFloatToString = (hours?: number): string => {
    if (!hours) return "";

    return (
        pad(Math.floor(hours)) +
        ":" +
        pad(Math.floor((hours - Math.floor(hours)) * 60)) +
        " Uhr"
    );
};

const pad = (number: number): string => {
    if (number < 10) {
        return "0" + number;
    }
    return number.toString();
};

export enum DateFormatTypes {
    longDateMin,
    longDateSec,
    longDateMs,
    shortDateMin,
    shortDateSec,
    shortDateMs,
}

const dateFormatOptions: Record<DateFormatTypes, Intl.DateTimeFormatOptions> = {
    [DateFormatTypes.longDateMin]: {
        weekday: "short",
        year: "numeric",
        month: "short",
        day: "2-digit",
        hour: "2-digit",
        minute: "2-digit",
    },
    [DateFormatTypes.longDateSec]: {
        weekday: "short",
        year: "numeric",
        month: "short",
        day: "2-digit",
        hour: "2-digit",
        minute: "2-digit",
        second: "2-digit",
    },
    [DateFormatTypes.longDateMs]: {
        weekday: "short",
        year: "numeric",
        month: "short",
        day: "2-digit",
        hour: "2-digit",
        minute: "2-digit",
        second: "2-digit",
        fractionalSecondDigits: 3,
    },
    [DateFormatTypes.shortDateMin]: {
        year: "2-digit",
        month: "2-digit",
        day: "2-digit",
        hour: "2-digit",
        minute: "2-digit",
    },
    [DateFormatTypes.shortDateSec]: {
        year: "2-digit",
        month: "2-digit",
        day: "2-digit",
        hour: "2-digit",
        minute: "2-digit",
        second: "2-digit",
    },
    [DateFormatTypes.shortDateMs]: {
        year: "2-digit",
        month: "2-digit",
        day: "2-digit",
        hour: "2-digit",
        minute: "2-digit",
        second: "2-digit",
        fractionalSecondDigits: 3,
    },
};

export const formatDateString = (
    dateString: string,
    format: DateFormatTypes,
    locale: string = "de"
): string => {
    let date = new Date(dateString);
    return date.toLocaleDateString(locale, dateFormatOptions[format]);
};

export const formatDate = (
    date: Date,
    format: DateFormatTypes,
    locale: string = "de"
): string => {
    return date.toLocaleDateString(locale, dateFormatOptions[format]);
};

export const formatCurrDate = (
    format: DateFormatTypes,
    locale: string = "de"
): string => {
    return new Date().toLocaleDateString(locale, dateFormatOptions[format]);
};

export const formatDurationSec = (durationSec: number): string => {
    if (durationSec === 0) {
        return " ";
    }
    const roundTowardsZero = durationSec > 0 ? Math.floor : Math.ceil;
    return (
        ", " +
        roundTowardsZero(durationSec / 86400) +
        "d " +
        (roundTowardsZero(durationSec / 3600) % 24) +
        "h " +
        (roundTowardsZero(durationSec / 60) % 60) +
        "m " +
        (roundTowardsZero(durationSec) % 60) +
        "s"
    );
};

export const formatDurationMs = (durationMs: number): string => {
    if (durationMs === 0) {
        return " ";
    }
    const roundTowardsZero = durationMs > 0 ? Math.floor : Math.ceil;
    return (
        ", " +
        roundTowardsZero(durationMs / (86400 * 1000)) +
        "d " +
        (roundTowardsZero(durationMs / (3600 * 1000)) % 24) +
        "h " +
        (roundTowardsZero(durationMs / (60 * 1000)) % 60) +
        "m " +
        (roundTowardsZero(durationMs / 1000) % 60) +
        "s " +
        (roundTowardsZero(durationMs) % 1000) +
        "ms"
    );
};

export const formatDateYYYYMMDDHHmmss = (date: Date): string => {
    const year = date.getUTCFullYear();
    const month = String(date.getUTCMonth() + 1).padStart(2, "0");
    const day = String(date.getUTCDate()).padStart(2, "0");
    const hours = String(date.getUTCHours()).padStart(2, "0");
    const minutes = String(date.getUTCMinutes()).padStart(2, "0");
    const seconds = String(date.getUTCSeconds()).padStart(2, "0");

    return `${year}${month}${day}${hours}${minutes}${seconds}`;
};

export const getDateFromYYYYMMDDHHmmss = (dateString: string): Date => {
    const year = +dateString.substring(0, 4);
    const month = +dateString.substring(4, 6);
    const day = +dateString.substring(6, 8);
    const hours = +dateString.substring(8, 10);
    const minutes = +dateString.substring(10, 12);
    const seconds = +dateString.substring(12, 14);

    return new Date(Date.UTC(year, month - 1, day, hours, minutes, seconds));
};
