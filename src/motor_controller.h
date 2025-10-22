#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <stdint.h>

/* REQ-001, REQ-002, REQ-003, REQ-004, REQ-005 */
typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_SHUTDOWN
} motor_state_t;

typedef struct {
    uint8_t pwm_duty;           /* REQ-001: 0-100% */
    int16_t temperature_c;      /* REQ-002: Temperature in Celsius */
    motor_state_t state;        /* REQ-005: Current state */
    uint32_t last_heartbeat_ms; /* REQ-004: Last CAN heartbeat */
} motor_controller_t;

/* REQ-001: Set motor speed 0-100% */
void motor_set_speed(uint8_t duty);

/* REQ-002: Read motor temperature */
int16_t motor_read_temperature(void);

/* REQ-003: Check temperature and shutdown if needed */
void motor_check_temperature(void);

/* REQ-004: Send CAN heartbeat */
void motor_send_heartbeat(void);

/* REQ-005: Transition to running state */
void motor_request_running(void);

/* REQ-005: Transition to shutdown state */
void motor_request_shutdown(void);

#endif