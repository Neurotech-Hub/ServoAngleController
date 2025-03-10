# Servo Angle Controller

Arduino-based servo controller with smooth motion and serial interface.

## Hardware Setup
- Arduino board
- Servo motor connected to:
  - Pin 9 (signal)
  - 5V (power)
  - GND (ground)

## Usage
1. Upload code to Arduino
2. Open Serial Monitor at 9600 baud with "Newline" ending

### Commands
- Single angle: Enter number between 60-120
  ```
  90
  ```
- Multiple angles: Enter comma-separated values
  ```
  60,90,120
  ```

### Features
- Angle range: 60° to 120°
- Smooth acceleration/deceleration
- 200ms movement duration
- Input validation with error messages

## Notes
- Servo starts at center position (90°)
- Invalid angles will be reported in Serial Monitor 