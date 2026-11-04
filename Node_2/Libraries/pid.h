#ifndef PID_H
#define PID_H

typedef struct {
    // Controller Gains
    float Kp;
    float Ki;
    float Kd;

    // Output Limits
    float out_min;
    float out_max;

    // Internal State
    float integral;
    float prev_error;
} PIDController;

/**
 * @brief Initializes the PID controller parameters and resets internal states.
 */
void pid_init(PIDController *pid, float kp, float ki, float kd, float out_min, float out_max);

/**
 * @brief Computes the PID output based on setpoint, actual measurement, and time step.
 * 
 * @param pid Pointer to PID controller structure.
 * @param setpoint Target position value (in encoder ticks).
 * @param measurement Current measured position (in encoder ticks).
 * @param dt Time delta since last update in seconds (e.g., 0.01s for 10ms loop).
 * @return Controller output value (clamped between out_min and out_max).
 */
float pid_update(PIDController *pid, float setpoint, float measurement, float dt);

/**
 * @brief Resets the integral and previous error terms.
 */
void pid_reset(PIDController *pid);

#endif // PID_H