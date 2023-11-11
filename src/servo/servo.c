#include "servo.h"

#include <esp_log.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"

#define DEBUG

static inline uint32_t angle_to_compare(int angle) {
    return (uint32_t)(angle * 7.38 + 820);  //(2150.0f - 820.0f) / (180) ; with 120° 11.0833
}

mcpwm_cmpr_handle_t comparatorRotate = NULL;
mcpwm_cmpr_handle_t comparatorTilt = NULL;

esp_err_t initServo() {
    esp_err_t error = ESP_OK;
    mcpwm_timer_handle_t timerRotate = NULL;
    mcpwm_timer_config_t timer_configRotate = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,
        .period_ticks = 4000,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    mcpwm_new_timer(&timer_configRotate, &timerRotate);
    mcpwm_oper_handle_t operRotate = NULL;
    mcpwm_operator_config_t operator_configRotate = {
        .group_id = 0,
    };
    mcpwm_new_operator(&operator_configRotate, &operRotate);
    mcpwm_operator_connect_timer(operRotate, timerRotate);

    mcpwm_comparator_config_t comparator_configRotate = {
        .flags.update_cmp_on_tez = true,
    };
    mcpwm_new_comparator(operRotate, &comparator_configRotate, &comparatorRotate);
    mcpwm_gen_handle_t genRotate = NULL;
    mcpwm_generator_config_t gen_configRotate = {
        .gen_gpio_num = OUTPUT_MOTOR_ROTATE,
    };
    mcpwm_new_generator(operRotate, &gen_configRotate, &genRotate);
    mcpwm_comparator_set_compare_value(comparatorRotate, angle_to_compare(0));
    mcpwm_generator_set_action_on_timer_event(genRotate, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
    mcpwm_generator_set_action_on_compare_event(genRotate, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparatorRotate, MCPWM_GEN_ACTION_LOW));
    mcpwm_timer_enable(timerRotate);
    mcpwm_timer_start_stop(timerRotate, MCPWM_TIMER_START_NO_STOP);

    mcpwm_timer_handle_t timerTilt = NULL;
    mcpwm_timer_config_t timer_configTilt = {
        .group_id = 1,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,
        .period_ticks = 4000,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    mcpwm_new_timer(&timer_configTilt, &timerTilt);
    mcpwm_oper_handle_t operTilt = NULL;
    mcpwm_operator_config_t operator_configTilt = {
        .group_id = 1,
    };
    mcpwm_new_operator(&operator_configTilt, &operTilt);
    mcpwm_operator_connect_timer(operTilt, timerTilt);

    mcpwm_comparator_config_t comparator_configTilt = {
        .flags.update_cmp_on_tez = true,
    };
    mcpwm_new_comparator(operTilt, &comparator_configTilt, &comparatorTilt);
    mcpwm_gen_handle_t genTilt = NULL;
    mcpwm_generator_config_t gen_configTilt = {
        .gen_gpio_num = OUTPUT_MOTOR_TILT,
    };
    mcpwm_new_generator(operTilt, &gen_configTilt, &genTilt);
    mcpwm_comparator_set_compare_value(comparatorTilt, angle_to_compare(0));
    mcpwm_generator_set_action_on_timer_event(genTilt, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
    mcpwm_generator_set_action_on_compare_event(genTilt, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparatorRotate, MCPWM_GEN_ACTION_LOW));
    mcpwm_timer_enable(timerTilt);
    mcpwm_timer_start_stop(timerTilt, MCPWM_TIMER_START_NO_STOP);

    return error;
}
uint16_t currDutyRotate = 0;
void setDutyRotate(uint16_t duty) {
    while (currDutyRotate != duty) {
        ((currDutyRotate < duty) ? (currDutyRotate++) : (currDutyRotate--));
        mcpwm_comparator_set_compare_value(comparatorRotate, angle_to_compare(currDutyRotate));
        printf("rotate duty %lu\n", angle_to_compare(currDutyRotate));
        ets_delay_us(DELAY_VALUE * 1000);
    }

    printf("rotate duty: %lu\n", angle_to_compare(currDutyRotate));
}

uint16_t currDutyTilt = 0;
void setDutyTilt(uint16_t duty) {
    while (currDutyTilt != duty) {
        ((currDutyTilt < duty) ? (currDutyTilt++) : (currDutyTilt--));
        mcpwm_comparator_set_compare_value(comparatorTilt, angle_to_compare(currDutyTilt));
        printf("tilt duty %lu\n", angle_to_compare(currDutyTilt));
        ets_delay_us(DELAY_VALUE * 1000);
    }
    printf("Tilt duty: %lu\n", angle_to_compare(currDutyTilt));
}

// for physical setup:
/*
if setup looking north (compass y to north aka heading = 0°)
rotate: 0%   -> horizontal from north to south
        50%  -> horizontal from west to east
        100% -> horizontal from south to north
        0° -> 360°

        in simulation
        0° = north -> 90° = west -> 180° south -> 270° = east

tilt:   0%   -> if rotate 50% then looking south
        50%  -> parallel to baseplate
        100% -> if rotate 50% then looking north
        0° -> 180° or -90° -> +90°
        but simulation makes 0% north and 100% south :(
*/
void setSunpos(int16_t rotateAngle, int16_t tiltAngle) {
    // 0 < all angles < 360°
    if (rotateAngle < 0) {
        int16_t temp = abs(rotateAngle);
        temp %= 360;
        rotateAngle = 360 + -temp;
    } else {
        rotateAngle %= 360;
    }

    if (tiltAngle < 0) {
        int16_t temp = abs(tiltAngle);
        temp %= 360;
        tiltAngle = 360 + -temp;
    } else {
        tiltAngle %= 360;
    }

    // 0 < tilt_angle < 180
    if (tiltAngle > 180 && tiltAngle < 270) {
        tiltAngle = 180;
    }
    if (tiltAngle < 0 || tiltAngle > 270) {
        tiltAngle = 0;
    }

    // translations for 180° servos:
    if (rotateAngle > 180) {
        rotateAngle -= 180;
        tiltAngle = 180 - tiltAngle;
    }

    // translate angle to dutycycle
    // reverse control
    // rotateDuty = DUTY_100_PERCENT - RATIO_DEG_2_PERCENT * rotateAngle;
    // tiltDuty = DUTY_100_PERCENT - RATIO_DEG_2_PERCENT * tiltAngle;
    /*
    for 120° servo



    if (rotateAngle > 150){
        rotateAngle = 120;
    }
    if (rotateAngle < 30){
        rotateAngle = 0;
    }
    if (rotateAngle > 30 && rotateAngle < 150){
        rotateAngle -= 30;
    }
    if(tiltAngle > 150){
        tiltAngle = 120;
    }
    if(tiltAngle < 30){
        tiltAngle = 0;
    }
    if (tiltAngle > 30 && tiltAngle < 150){
        tiltAngle -= 30;
    }
    */

    setDutyRotate(180 - rotateAngle);
    setDutyTilt(tiltAngle);
}