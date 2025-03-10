#include <Servo.h>

Servo servos[3];                // Create array of servo objects
String inputString = "";        // String to hold incoming data
boolean stringComplete = false; // Whether the string is complete
boolean demoRunning = true;     // Flag for demo mode - start in demo mode by default

// Add these global variables at the top with other declarations
const int STEP_DELAY = 15;               // Delay between each degree of movement (milliseconds)
const int MIN_ANGLE = 60;                // Minimum allowed angle
const int MAX_ANGLE = 120;               // Maximum allowed angle
const unsigned long MOVE_DURATION = 200; // Total time for movement in milliseconds
int currentAngles[3] = {90, 90, 90};     // Keep track of current positions, start in middle
const int servoPins[3] = {9, 10, 11};    // Pins for the servos

// Ease-in-out function (returns value between 0 and 1)
float easeInOut(float t)
{
  // Using sine function for smooth acceleration/deceleration
  return (1 - cos(t * PI)) / 2;
}

void setup()
{
  Serial.begin(9600);  // Initialize serial communication
  servos[0].attach(9); // Attach servos to pins
  servos[1].attach(10);
  servos[2].attach(11);
  Serial.println("Starting in demo mode (send any character to exit)");
  Serial.println("After exiting demo, enter angle (60-120), comma-separated angles, or 'd' for demo:");
}

void moveToAngle(int targetAngle, int servoIndex)
{
  // Update constrain to use new limits
  targetAngle = constrain(targetAngle, MIN_ANGLE, MAX_ANGLE);

  int startAngle = currentAngles[servoIndex];
  int angleDistance = targetAngle - startAngle;

  unsigned long startTime = millis();

  // Animate until duration is complete
  while (millis() - startTime < MOVE_DURATION)
  {
    // Calculate progress (0 to 1)
    float progress = (float)(millis() - startTime) / MOVE_DURATION;

    // Apply easing function
    float easedProgress = easeInOut(progress);

    // Calculate new position
    currentAngles[servoIndex] = startAngle + (angleDistance * easedProgress);
    servos[servoIndex].write(currentAngles[servoIndex]);

    delay(5); // Small delay for smooth movement
  }

  // Ensure we end exactly at target
  currentAngles[servoIndex] = targetAngle;
  servos[servoIndex].write(currentAngles[servoIndex]);

  Serial.print("Servo ");
  Serial.print(servoIndex);
  Serial.print(" reached angle: ");
  Serial.println(currentAngles[servoIndex]);
}

void moveAllServos(int targetAngle)
{
  for (int i = 0; i < 3; i++)
  {
    moveToAngle(targetAngle, i);
  }
}

void runDemo()
{
  static unsigned long lastUpdate = 0;
  static int servoPhases[3] = {0, 120, 240}; // Phase offset for each servo (in degrees)
  static unsigned long speedChangeTime = 0;
  static int speedMode = 0;                                // 0: slow, 1: fast, 2: super fast
  static const unsigned long SPEED_CHANGE_INTERVAL = 3000; // Switch speed every 3 seconds
  static float individualSpeeds[3] = {1.0, 1.0, 1.0};      // Current speed multipliers
  static float targetSpeeds[3] = {1.0, 1.0, 1.0};          // Target speed multipliers
  static unsigned long lastSpeedUpdate = 0;
  static float randomOffsets[3] = {0, 0, 0}; // Current random offsets
  static float targetOffsets[3] = {0, 0, 0}; // Target random offsets
  static unsigned long lastOffsetUpdate = 0;
  static bool coordinatedMode = false;
  static unsigned long lastModeChange = 0;
  static const unsigned long MODE_CHANGE_INTERVAL = 6000; // Switch between random and coordinated every 6 seconds
  static float coordinatedAngle = 90;
  static float coordinatedPhase = 0;

  // Switch between random and coordinated modes
  if (millis() - lastModeChange >= MODE_CHANGE_INTERVAL)
  {
    lastModeChange = millis();
    coordinatedMode = !coordinatedMode;
    Serial.println(coordinatedMode ? "Demo: Coordinated sequence" : "Demo: Random movement");
    // Reset positions when switching to coordinated mode
    if (coordinatedMode)
    {
      coordinatedPhase = 0;
      coordinatedAngle = 90;
    }
  }

  if (coordinatedMode)
  {
    if (millis() - lastUpdate >= 20)
    { // Update every 20ms
      lastUpdate = millis();

      // Calculate base angle using sine wave
      coordinatedAngle = (sin(radians(coordinatedPhase)) + 1) * (MAX_ANGLE - MIN_ANGLE) / 2 + MIN_ANGLE;

      // Much faster base speed (was 2)
      float baseSpeed = 8;

      // Speed varies based on position in the wave
      float speedMultiplier = 1.0 + sin(radians(coordinatedPhase)) * 0.5; // Varies between 0.5x and 1.5x
      coordinatedPhase += baseSpeed * speedMultiplier;                    // Variable speed wave

      // Phase offset for each servo (90 degrees apart for tighter sequence)
      for (int i = 0; i < 3; i++)
      {
        float servoPhaseOffset = i * 90; // Tighter spacing between servos
        float angle = (sin(radians(coordinatedPhase - servoPhaseOffset)) + 1) * (MAX_ANGLE - MIN_ANGLE) / 2 + MIN_ANGLE;
        servos[i].write(angle);
      }
    }
    return; // Skip the random movement code when in coordinated mode
  }

  // Original random movement code
  // Gradually update random offsets every 1000ms
  if (millis() - lastOffsetUpdate >= 1000)
  {
    lastOffsetUpdate = millis();
    for (int i = 0; i < 3; i++)
    {
      targetOffsets[i] = random(-200, 201) / 100.0;
    }
  }

  // Randomly update target speeds every 500ms
  if (millis() - lastSpeedUpdate >= 500)
  {
    lastSpeedUpdate = millis();
    for (int i = 0; i < 3; i++)
    {
      if (random(100) < 30)
      {                                            // 30% chance to change speed
        targetSpeeds[i] = random(50, 150) / 100.0; // Random speed between 0.5x and 1.5x
      }
    }
  }

  // Check if it's time to change speed mode
  if (millis() - speedChangeTime >= SPEED_CHANGE_INTERVAL)
  {
    speedMode = random(3); // Randomly select mode instead of cycling
    speedChangeTime = millis();

    switch (speedMode)
    {
    case 0:
      Serial.println("Demo: Slow mode");
      break;
    case 1:
      Serial.println("Demo: Fast mode");
      break;
    case 2:
      Serial.println("Demo: SUPER FAST mode");
      break;
    }
  }

  if (millis() - lastUpdate >= 20)
  { // Update every 20ms
    lastUpdate = millis();

    // Base speed settings
    int baseIncrement;
    switch (speedMode)
    {
    case 0:
      baseIncrement = 1; // Slow
      break;
    case 1:
      baseIncrement = 4; // Fast
      break;
    case 2:
      baseIncrement = 12; // Super fast
      break;
    }

    for (int i = 0; i < 3; i++)
    {
      // Smoothly interpolate to target speeds and offsets
      individualSpeeds[i] += (targetSpeeds[i] - individualSpeeds[i]) * 0.1;
      randomOffsets[i] += (targetOffsets[i] - randomOffsets[i]) * 0.1;

      // Apply individual speed multiplier to base increment
      float phaseIncrement = baseIncrement * individualSpeeds[i];

      // Use interpolated random offset
      float angle = (sin(radians(servoPhases[i])) + 1) * (MAX_ANGLE - MIN_ANGLE) / 2 + MIN_ANGLE + randomOffsets[i];

      // Constrain the final angle
      angle = constrain(angle, MIN_ANGLE, MAX_ANGLE);
      servos[i].write(angle);

      // Update phase with individual speed
      servoPhases[i] = (servoPhases[i] + (int)phaseIncrement) % 360;
    }
  }
}

void loop()
{
  if (demoRunning)
  {
    runDemo();

    // Check if we should exit demo mode
    if (Serial.available())
    {
      demoRunning = false;
      Serial.println("\nExiting demo mode");
      Serial.println("Enter angle (60-120), comma-separated angles, or 'd' for demo:");
    }
    return;
  }

  // Process input when a complete string is received
  if (stringComplete)
  {
    // Remove newline
    inputString.trim();

    if (inputString == "d")
    {
      Serial.println("Starting demo mode (send any character to exit)");
      demoRunning = true;
    }
    else if (inputString.indexOf(',') == -1)
    {
      // Single angle mode
      int angle = inputString.toInt();
      if (angle >= MIN_ANGLE && angle <= MAX_ANGLE)
      {
        moveAllServos(angle);
      }
      else
      {
        Serial.print("Invalid angle. Please enter a number between ");
        Serial.print(MIN_ANGLE);
        Serial.print("-");
        Serial.println(MAX_ANGLE);
      }
    }
    else
    {
      // Sequence mode
      String angleStr = inputString;
      while (angleStr.length() > 0)
      {
        int commaIndex = angleStr.indexOf(',');
        int angle;

        if (commaIndex == -1)
        {
          angle = angleStr.toInt();
          angleStr = "";
        }
        else
        {
          angle = angleStr.substring(0, commaIndex).toInt();
          angleStr = angleStr.substring(commaIndex + 1);
        }

        if (angle >= MIN_ANGLE && angle <= MAX_ANGLE)
        {
          moveAllServos(angle);
        }
        else
        {
          Serial.print("Skipping invalid angle: ");
          Serial.print(angle);
          Serial.print(". Valid range is ");
          Serial.print(MIN_ANGLE);
          Serial.print("-");
          Serial.println(MAX_ANGLE);
        }
      }
    }

    // Update prompt message
    inputString = "";
    stringComplete = false;
    Serial.print("\nEnter angle (");
    Serial.print(MIN_ANGLE);
    Serial.print("-");
    Serial.print(MAX_ANGLE);
    Serial.println(") or comma-separated angles, or 'd' for demo:");
  }
}

// Serial event handler
void serialEvent()
{
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    if (inChar == '\n')
    {
      stringComplete = true;
    }
    else
    {
      inputString += inChar;
    }
  }
}