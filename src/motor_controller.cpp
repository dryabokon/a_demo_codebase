#include "motor_controller.h"
#include "pwm_driver.h"
#include "adc_driver.h"
#include "can_driver.h"

/* Temperature threshold for shutdown (REQ-003) */
#define TEMP_SHUTDOWN_THRESHOLD_C 120
#define TEMP_RUNNING_THRESHOLD_C  110

/* CAN heartbeat timeout (REQ-004) */
#define CAN_HEARTBEAT_TIMEOUT_MS  200

static motor_controller_t controller = {
    .pwm_duty = 0,
    .temperature_c = 25,
    .state = STATE_IDLE,
    .last_heartbeat_ms = 0
};

/**
 * REQ-001: Set motor PWM speed
 * TEST-UT-001: Unit test range validation
 * TEST-IT-001: Integration test with PWM driver
 * TEST-HITL-001: Hardware verification with oscilloscope
 */
void motor_set_speed(uint8_t duty) {
    if (duty > 100) duty = 100;  /* Clamp to 100% */
    
    if (controller.state == STATE_SHUTDOWN) {
        duty = 0;  /* Force 0% in shutdown state */
    }
    
    controller.pwm_duty = duty;
    pwm_set_duty(duty);  /* REQ-001: Actual PWM output */
}

/**
 * REQ-002: Read motor temperature from ADC
 * TEST-UT-002: Unit test ADC conversion
 * TEST-IT-002: Integration test with temperature sensor
 * TEST-HITL-002: Hardware validation with thermometer
 */
int16_t motor_read_temperature(void) {
    uint16_t adc_raw = adc_read_channel(ADC_TEMP_CHANNEL);
    
    /* Convert ADC to temperature (simplified) */
    /* Actual: Steinhart-Hart or lookup table */
    int16_t temp_c = (adc_raw / 10) - 40;
    
    controller.temperature_c = temp_c;
    return temp_c;
}

/**
 * REQ-003: Safety shutdown on over-temperature
 * TEST-UT-003: Unit test threshold comparison
 * TEST-IT-003: Integration test state machine
 * TEST-HITL-003: Hardware stress test (heating element)
 */
void motor_check_temperature(void) {
    int16_t temp = motor_read_temperature();
    
    /* REQ-003: Hard real-time safety check */
    if (temp > TEMP_SHUTDOWN_THRESHOLD_C) {
        controller.state = STATE_SHUTDOWN;
        motor_set_speed(0);  /* Immediate shutdown */
    }
}

/**
 * REQ-004: Send CAN heartbeat every 100ms
 * TEST-UT-004: Unit test timing
 * TEST-IT-004: Integration test CAN bus
 * TEST-HITL-004: CAN analyzer verification
 */
void motor_send_heartbeat(void) {
    uint32_t now_ms = get_timestamp_ms();
    
    can_message_t msg = {
        .id = 0x100,
        .dlc = 8,
        .data[0] = controller.pwm_duty,
        .data[1] = (controller.temperature_c >> 8) & 0xFF,
        .data[2] = controller.temperature_c & 0xFF,
        .data[3] = controller.state
    };
    
    can_send(&msg);
    controller.last_heartbeat_ms = now_ms;
}

/**
 * REQ-005: Transition to RUNNING state
 * TEST-IT-005: State machine transition test
 * TEST-HITL-005: Hardware state verification
 */
void motor_request_running(void) {
    int16_t temp = motor_read_temperature();
    
    if (temp < TEMP_RUNNING_THRESHOLD_C && 
        controller.state == STATE_IDLE) {
        controller.state = STATE_RUNNING;
    }
}

/**
 * REQ-005: Transition to SHUTDOWN state
 * TEST-IT-005: State machine transition test
 * TEST-HITL-005: Hardware state verification
 */
void motor_request_shutdown(void) {
    controller.state = STATE_SHUTDOWN;
    motor_set_speed(0);
}