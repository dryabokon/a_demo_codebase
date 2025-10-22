#include "motor_controller.h"
#include "unity.h"  /* Unit test framework */

/**
 * TEST-UT-001: Motor speed range 0-100%
 * Verifies: REQ-001
 * Success: Duty cycle clamped correctly
 */
void test_motor_set_speed_valid_range(void) {
    motor_set_speed(0);
    TEST_ASSERT_EQUAL(0, controller.pwm_duty);
    
    motor_set_speed(50);
    TEST_ASSERT_EQUAL(50, controller.pwm_duty);
    
    motor_set_speed(100);
    TEST_ASSERT_EQUAL(100, controller.pwm_duty);
}

/**
 * TEST-UT-001: Motor speed clamping
 * Verifies: REQ-001 (safety: clamp invalid values)
 * Success: Values > 100 clamped to 100
 */
void test_motor_set_speed_clamping(void) {
    motor_set_speed(150);
    TEST_ASSERT_EQUAL(100, controller.pwm_duty);
    
    motor_set_speed(200);
    TEST_ASSERT_EQUAL(100, controller.pwm_duty);
}

/**
 * TEST-UT-002: Temperature reading
 * Verifies: REQ-002
 * Success: Temperature within valid range
 */
void test_motor_read_temperature(void) {
    int16_t temp = motor_read_temperature();
    
    TEST_ASSERT_GREATER_OR_EQUAL(0, temp);
    TEST_ASSERT_LESS_OR_EQUAL(150, temp);
}

/**
 * TEST-UT-003: Over-temperature shutdown
 * Verifies: REQ-003 (safety critical)
 * Success: PWM goes to 0% when temp > 120°C
 */
void test_motor_temperature_shutdown(void) {
    /* Mock temperature sensor to return 125°C */
    mock_adc_set_temperature(125);
    
    motor_set_speed(80);
    motor_check_temperature();
    
    TEST_ASSERT_EQUAL(STATE_SHUTDOWN, controller.state);
    TEST_ASSERT_EQUAL(0, controller.pwm_duty);
}

/**
 * TEST-UT-004: CAN heartbeat
 * Verifies: REQ-004
 * Success: Heartbeat sent within timing
 */
void test_motor_heartbeat_timing(void) {
    uint32_t t1 = get_timestamp_ms();
    motor_send_heartbeat();
    uint32_t t2 = get_timestamp_ms();
    
    TEST_ASSERT_TRUE(t2 - t1 < 10);  /* Fast send */
}

/**
 * TEST-UT-005: State transitions
 * Verifies: REQ-005
 * Success: Can only run if temp < 110°C
 */
void test_motor_state_transition(void) {
    /* Start in IDLE */
    TEST_ASSERT_EQUAL(STATE_IDLE, controller.state);
    
    /* Cannot run if too hot */
    mock_adc_set_temperature(115);
    motor_request_running();
    TEST_ASSERT_EQUAL(STATE_IDLE, controller.state);
    
    /* Can run if cool */
    mock_adc_set_temperature(25);
    motor_request_running();
    TEST_ASSERT_EQUAL(STATE_RUNNING, controller.state);
}