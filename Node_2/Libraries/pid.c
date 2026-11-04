#include "pid.h"

void pid_init(PIDController *pid, float kp, float ki, float kd, float out_min, float out_max) {
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->out_min = out_min;
    pid->out_max = out_max;
    pid_reset(pid);
}

void pid_reset(PIDController *pid) {
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
}

float pid_update(PIDController *pid, float setpoint, float measurement, float dt) {
    // 1. Calculate error
    float error = setpoint - measurement;

    // 2. Proportional term
    float p_term = pid->Kp * error;

    // 3. Integral term with Anti-Windup clamping
    pid->integral += error * dt;
    float i_term = pid->Ki * pid->integral;

    if (i_term > pid->out_max) {
        i_term = pid->out_max;
        pid->integral = pid->out_max / (pid->Ki > 0.0f ? pid->Ki : 1.0f);
    } else if (i_term < pid->out_min) {
        i_term = pid->out_min;
        pid->integral = pid->out_min / (pid->Ki > 0.0f ? pid->Ki : 1.0f);
    }

    // 4. Derivative term
    float derivative = (dt > 0.0f) ? (error - pid->prev_error) / dt : 0.0f;
    float d_term = pid->Kd * derivative;

    pid->prev_error = error;

    // 5. Total output clamped to limits
    float output = p_term + i_term + d_term;
    if (output > pid->out_max) output = pid->out_max;
    if (output < pid->out_min) output = pid->out_min;

    return output;
}