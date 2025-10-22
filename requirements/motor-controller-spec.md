# Motor Controller Specification

## REQ-001: PWM Motor Control
- Requirement: System shall control motor speed via PWM 0-100%
- Safety Level: ASIL-B
- Accuracy: ±2%
- Response time: < 50ms
- Verification: TEST-UT-001, TEST-IT-001, TEST-HITL-001

## REQ-002: Temperature Monitoring
- Requirement: System shall monitor motor temperature via ADC
- Range: 0°C to 150°C
- Accuracy: ±5°C
- Sample rate: 100ms
- Verification: TEST-UT-002, TEST-IT-002, TEST-HITL-002

## REQ-003: Safe Shutdown on Over-Temperature
- Requirement: When temperature > 120°C, motor PWM shall immediately go to 0%
- Safety Level: ASIL-C (safety critical)
- Response time: < 5ms (hard real-time)
- Verification: TEST-UT-003, TEST-IT-003, TEST-HITL-003

## REQ-004: Watchdog Heartbeat
- Requirement: System shall send CAN heartbeat every 100ms
- Timeout: If no CAN message in 200ms, trigger safe shutdown
- Verification: TEST-UT-004, TEST-IT-004, TEST-HITL-004

## REQ-005: State Transitions
- System states: IDLE -> RUNNING -> SHUTDOWN
- RUNNING only allowed if temp < 110°C
- Cannot transition SHUTDOWN -> RUNNING (requires reset)
- Verification: TEST-IT-005, TEST-HITL-005