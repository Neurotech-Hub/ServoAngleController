# Servo Angle Controller

Arduino-based multi-servo controller featuring smooth motion, interactive demo modes, and serial interface control.

## Hardware Setup
- Arduino board
- 3 Servo motors connected to:
  - Servo 1: Pin 9 (signal)
  - Servo 2: Pin 10 (signal)
  - Servo 3: Pin 11 (signal)
  - 5V (power)
  - GND (ground)

## Features
- Controls 3 synchronized servo motors
- Smooth motion with easing functions
- Constrained angle range: 60° to 120°
- Interactive demo modes with dynamic patterns
- Real-time serial control interface

### Demo Modes
- Coordinated sequence mode
  - Synchronized wave-like motion
  - Variable speed patterns
  - Automatic mode switching
- Random movement mode
  - Individual servo speed control
  - Dynamic speed variations (Slow/Fast/Super Fast)
  - Random offset patterns

## Usage
1. Upload code to Arduino
2. Open Serial Monitor at 9600 baud with "Newline" ending
3. System starts in demo mode by default

### Commands
- Demo mode: Enter 'd' to start demo
  ```
  d
  ```
- Single angle: Enter number between 60-120 (moves all servos)
  ```
  90
  ```
- Multiple angles: Enter comma-separated values
  ```
  60,90,120
  ```
- Exit demo mode: Send any character while demo is running

### Operation Notes
- All servos start at center position (90°)
- Smooth motion with 200ms movement duration
- Invalid angles will be reported in Serial Monitor
- Demo modes automatically alternate between coordinated and random patterns
- Speed variations in demo mode provide dynamic visual effects

## Technical Details
- Servo angle constraints: 60° to 120°
- Movement duration: 200ms per transition
- Easing function for smooth acceleration/deceleration
- Real-time serial monitoring and feedback
- Multiple demo patterns with automatic transitions 