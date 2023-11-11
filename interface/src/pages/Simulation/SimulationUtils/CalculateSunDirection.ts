import { radiansPerDegree } from "./MathUtils";
import { Location, SunDirection } from "../";

const millisecondsPerSecond = 1000;
const secondsPerMinute = 60;
const minutesPerHour = 60;
const secondsPerHour = secondsPerMinute * minutesPerHour;
const hoursPerDay = 24;
const millisecondsPerDay = hoursPerDay * secondsPerHour * millisecondsPerSecond;
const earthRotationDegreesPerHour = 15;
const daysPerYear = 365.25;
const fullCircleDegrees = 360;
const halfCircleDegrees = 180;
const march22DayOfYear = 81;
const maxDeclinationAngle = 23.45;
const solarNoonHours = 12;

function GetDayOfYear(date: Date) {
    return (
        (Date.UTC(
            date.getUTCFullYear(),
            date.getUTCMonth(),
            date.getUTCDate(),
            date.getUTCHours(),
            date.getUTCMinutes(),
            date.getUTCSeconds(),
            date.getUTCMilliseconds()
        ) -
            Date.UTC(date.getUTCFullYear(), 0, 0)) /
        millisecondsPerDay
    );
}

export const CalculateSunDirection = (
    loc: Location,
    date: Date,
    timeZoneOffsetInHours: number
): SunDirection => {
    const dayOfYear = GetDayOfYear(date);

    const LT =
        date.getHours() +
        date.getMinutes() / minutesPerHour +
        date.getSeconds() / secondsPerHour; // hours

    const deltaTutc = timeZoneOffsetInHours;
    const LSTM = earthRotationDegreesPerHour * deltaTutc;

    const B =
        (fullCircleDegrees / daysPerYear) * (dayOfYear - march22DayOfYear);
    const EoT =
        9.87 * Math.sin(2 * B * radiansPerDegree) -
        7.53 * Math.cos(B * radiansPerDegree) -
        1.5 * Math.sin(B * radiansPerDegree); // minutes

    const TC = 4 * (loc.longitude - LSTM) + EoT; // minutes

    var LST = LT + TC / minutesPerHour; // hours

    // NOTE: LST cannot be negative
    // but it can happens when TC is big and LT small (new day)
    // only small negative numbers tho
    // HOTFIX: Normalize value hours
    if (LST < 0) LST += hoursPerDay;
    if (LST > 24) LST -= hoursPerDay;

    var HRA = earthRotationDegreesPerHour * (LST - solarNoonHours);

    // HRA is also used for azimuth -> hra need to be between [180, -180]
    // here something like 188° can happen -> normalize angle
    if (HRA > halfCircleDegrees) HRA -= fullCircleDegrees;
    if (HRA < -halfCircleDegrees) HRA += fullCircleDegrees;

    const phi = loc.latitude;

    const delta =
        Math.asin(
            Math.sin(maxDeclinationAngle * radiansPerDegree) *
                Math.sin(B * radiansPerDegree)
        ) / radiansPerDegree;

    const elevation =
        Math.asin(
            Math.sin(phi * radiansPerDegree) *
                Math.sin(delta * radiansPerDegree) +
                Math.cos(phi * radiansPerDegree) *
                    Math.cos(delta * radiansPerDegree) *
                    Math.cos(HRA * radiansPerDegree)
        ) / radiansPerDegree;

    let azimuth =
        Math.acos(
            (Math.sin(delta * radiansPerDegree) *
                Math.cos(phi * radiansPerDegree) -
                Math.cos(delta * radiansPerDegree) *
                    Math.sin(phi * radiansPerDegree) *
                    Math.cos(HRA * radiansPerDegree)) /
                Math.cos(elevation * radiansPerDegree)
        ) / radiansPerDegree;

    if (HRA <= 0) {
        azimuth = fullCircleDegrees - azimuth;
    }

    return {
        dayOfYear,
        localTime: LT,
        equationOfTime: EoT,
        timeCorrectionFactor: TC,
        localSolarTime: LST,
        hourAngle: HRA,
        declinationAngle: delta,
        direction: { azimuthAngle: azimuth, elevationAngle: elevation },
    };
};
