import * as THREE from "three";
import { Direction } from "..";

export const radiansPerDegree = Math.PI / 180;

export function GetPositionFromDirection(
    direction: Direction,
    length: number = 1
): THREE.Vector3 {
    const azimuthRad = direction.azimuthAngle * radiansPerDegree;
    const elevationRad = direction.elevationAngle * radiansPerDegree;

    const x = Math.cos(elevationRad) * Math.sin(azimuthRad) * -1 * length;
    const y = Math.sin(elevationRad) * length;
    const z = Math.cos(elevationRad) * Math.cos(azimuthRad) * -1 * length;

    return new THREE.Vector3(Round(x, 3), Round(y, 3), Round(z, 3));
}

export function GetAngle(d1?: Direction, d2?: Direction): number {
    if (!d1 || !d2) return 0;

    const v1 = GetPositionFromDirection(d1);
    const v2 = GetPositionFromDirection(d2);

    // due to inaccuracy the cpu can calc values slightly > 1 or < -1
    // Math.acos don't like this and return NaN here
    var x = v1.dot(v2) / (v1.length() * v2.length());
    if (x > 1) x = 1;
    if (x < -1) x = -1;
    return Math.acos(x) / radiansPerDegree;
}

export function Round(num?: number, decimals: number = 4) {
    if (!num) return 0;
    const factor = Math.pow(10, decimals);
    return Math.round((num + Number.EPSILON) * factor) / factor;
}
