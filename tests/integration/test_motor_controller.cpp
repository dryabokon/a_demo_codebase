/**
 * TEST-IT-001: PWM + Temperature coordination
 * Verifies: REQ-001 + REQ-002 together
 * Success: Speed changes reflected, temp monitored
 */
void test_motor_speed_and_temp_together(void) {
    motor_request_running();
    motor_set_speed(80);
    
    for (int i = 0; i < 10; i++) {
        int16_t temp = motor_read_temperature();
        TEST_ASSERT_LESS_THAN(120, temp);
        delay_ms(100);
    }
    
    TEST_ASSERT_EQUAL(80, controller.pwm_duty);
}

/**
 * TEST-IT-003: Full shutdown sequence
 * Verifies: REQ-003 (safety critical)
 * Success: Rapid temperature rise triggers immediate shutdown
 */
void test_temperature_emergency_shutdown(void) {
    motor_request_running();
    motor_set_speed(100);
    
    /* Simulate temperature rising */
    for (int t = 25; t <= 125; t += 5) {
        mock_adc_set_temperature(t);
        motor_check_temperature();
        
        if (t >= 120) {
            TEST_ASSERT_EQUAL(STATE_SHUTDOWN, controller.state);
            TEST_ASSERT_EQUAL(0, controller.pwm_duty);
            break;
        }
    }
}

/**
 * TEST-IT-005: Full state machine
 * Verifies: REQ-005
 * Success: All valid transitions work, invalid ones blocked
 */
void test_motor_state_machine(void) {
    /* IDLE -> RUNNING (if cool) */
    mock_adc_set_temperature(25);
    motor_request_running();
    TEST_ASSERT_EQUAL(STATE_RUNNING, controller.state);
    
    /* RUNNING -> SHUTDOWN */
    motor_request_shutdown();
    TEST_ASSERT_EQUAL(STATE_SHUTDOWN, controller.state);
    
    /* SHUTDOWN -> RUNNING (NOT ALLOWED) */
    motor_request_running();
    TEST_ASSERT_EQUAL(STATE_SHUTDOWN, controller.state);
}