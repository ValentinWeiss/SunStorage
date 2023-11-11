#include "suntracking.h"

#define DEG2RAD (float)(M_PI / 180)
#define RAD2DEG (float)(180 / M_PI)

void calcServoPos(float xGyro, float yGyro, float zGyro, float azimuth, float elevation, float *angleRotate, float *angleTilt) {
    // variables
    float min = INFINITY;
    float xSun = 0.0;
    float ySun = 0.0;
    float zSun = 0.0;

    if (xGyro > 0.01 || yGyro > 0.01) {  // for performance and zero division
        // mirror vector against z axis
        xGyro = -xGyro;
        yGyro = -yGyro;

        float norm = sqrtf(xGyro * xGyro + yGyro * yGyro + zGyro * zGyro);
        xGyro /= norm;
        yGyro /= norm;
        zGyro /= norm;
#ifdef DEBUG
        printf("gyro: %f %f %f\n", xGyro, yGyro, zGyro);
#endif
        // get orthogonal vector (if x y z = 0 then vector is parallel to z axis -> skip all and just use azi and ele directly)
        float xOrtho = yGyro * 1;
        float yOrtho = -xGyro * 1;
        float zOrtho = 0;
#ifdef DEBUG
        printf("ortho: %f %f %f\n", xOrtho, yOrtho, zOrtho);
#endif
        // transform azimuth from deg to x, y with 4 quadrants
        if (azimuth > 270) {
            xSun = -tanf(azimuth * DEG2RAD);
            ySun = -1;
        } else if (azimuth > 180) {
            ySun = -tanf(azimuth * DEG2RAD);
            xSun = -1;
        } else if (azimuth > 90) {
            xSun = -1;
            ySun = -tanf(azimuth * DEG2RAD);
        } else {
            ySun = tanf(azimuth * DEG2RAD);
            xSun = 1;
        }

        // normalize 2d vec xSun ySun for zSun calulation
        float len = sqrtf(xSun * xSun + ySun * ySun);
        xSun = xSun * (1 / len);
        ySun = ySun * (1 / len);

        // transform elevation from deg to z
        if (elevation < 89.9) {
            zSun = tanf(elevation * DEG2RAD);
        }
        if (elevation > 89.9 && elevation < 90.1) {
            zSun = 1;
            xSun = 0;
            ySun = 0;
        }
        if (elevation > 90.1) {
            zSun = tanf((elevation - 90) * DEG2RAD);
        }
#ifdef DEBUG
        printf("sun: %f %f %f\n", xSun, ySun, zSun);
#endif
        // find optimal rotate angle
        float xOpt;
        float yOpt;
        float zOpt;

        for (int i = 0; i < 180; i++) {
            // rotate orthogonal vector around a-vector
            float angle = i;
            float r11 = (xGyro * xGyro) * (1 - cos(angle * DEG2RAD)) + cos(angle * DEG2RAD);
            float r12 = (yGyro * xGyro) * (1 - cos(angle * DEG2RAD)) + zGyro * sin(angle * DEG2RAD);
            float r13 = (zGyro * xGyro) * (1 - cos(angle * DEG2RAD)) - yGyro * sin(angle * DEG2RAD);
            float r21 = (xGyro * yGyro) * (1 - cos(angle * DEG2RAD)) - zGyro * sin(angle * DEG2RAD);
            float r22 = (yGyro * yGyro) * (1 - cos(angle * DEG2RAD)) + cos(angle * DEG2RAD);
            float r23 = (zGyro * yGyro) * (1 - cos(angle * DEG2RAD)) + xGyro * sin(angle * DEG2RAD);
            float r31 = (xGyro * zGyro) * (1 - cos(angle * DEG2RAD)) + yGyro * sin(angle * DEG2RAD);
            float r32 = (yGyro * zGyro) * (1 - cos(angle * DEG2RAD)) - xGyro * sin(angle * DEG2RAD);
            float r33 = (zGyro * zGyro) * (1 - cos(angle * DEG2RAD)) + cos(angle * DEG2RAD);

            // plane vector
            float xn = xOrtho * r11 + yOrtho * r21 + zOrtho * r31;
            float yn = xOrtho * r12 + yOrtho * r22 + zOrtho * r32;
            float zn = xOrtho * r13 + yOrtho * r23 + zOrtho * r33;

            // calculate if sunvector is in plane
            float val = xSun * xn + ySun * yn + zSun * zn;
            if (fabs(val) < min) {
                min = fabs(val);
                xOpt = xn;
                yOpt = yn;
                zOpt = zn;
            }
            // printf("%i %f %f %f %f\n", i, val, xn, yn, zn);
        }
#ifdef DEBUG
        printf("minDeg: %i real0deg: %f");
        printf("atan2f(x,y): %f atan2f(y,x): %f", atan2f(xOpt, yOpt) * RAD2DEG, atan2f(yOpt, xOpt) * RAD2DEG);
        printf("opt: %f %f %f\n", xOpt, yOpt, zOpt);
#endif
        // new orthogonal vector between gyro-vector and opt-vector
        float xn = yOpt * zGyro - yGyro * zOpt;
        float yn = zOpt * xGyro - zGyro * xOpt;
        float zn = xOpt * yGyro - xGyro * yOpt;
#ifdef DEBUG
        printf("n: %f %f %f\n", xn, yn, zn);
#endif
        // check if direction of plattform is correct
        if ((xSun / xn) < 0 || (ySun / yn) < 0 || (zSun / zn) < 0) {
            // invert direction of vector
            xn = -xn;
            yn = -yn;
            zn = -zn;
        }

        *angleRotate = atan2f(yn, xn) * RAD2DEG;

        // tilt: calc angle between opt-vector and s-vector
        *angleTilt = (acosf((xn * xSun + yn * ySun + zn * zSun) /
                            (sqrtf(xn * xn + yn * yn + zn * zn) *
                             sqrtf(xSun * xSun + ySun * ySun + zSun * zSun)))) *
                     RAD2DEG;
        if (*angleTilt > 90.0) {
            *angleTilt -= 90.0;
        }
#ifdef DEBUG
        printf("anglerot: %f angletilt: %f\n", *angleRotate, *angleTilt);
#endif
    } else {
        *angleRotate = azimuth;
        *angleTilt = elevation;
    }
}
