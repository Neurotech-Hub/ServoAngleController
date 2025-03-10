#include <Servo.h>

Servo myservo;                  // Create servo object
String inputString = "";        // String to hold incoming data
boolean stringComplete = false; // Whether the string is complete

// Add these global variables at the top with other declarations
const int STEP_DELAY = 15;               // Delay between each degree of movement (milliseconds)
const int MIN_ANGLE = 60;                // Minimum allowed angle
const int MAX_ANGLE = 120;               // Maximum allowed angle
const unsigned long MOVE_DURATION = 200; // Total time for movement in milliseconds
int currentAngle = 90;                   // Keep track of current position, start in middle

// Ease-in-out function (returns value between 0 and 1)
float easeInOut(float t)
{
  // Using sine function for smooth acceleration/deceleration
  return (1 - cos(t * PI)) / 2;
}

void setup()
{
  Serial.begin(9600); // Initialize serial communication
  myservo.attach(9);  // Attach servo to pin 9
  Serial.println("Enter angle (0-180) or comma-separated angles:");
}

void moveToAngle(int targetAngle)
{
  // Update constrain to use new limits
  targetAngle = constrain(targetAngle, MIN_ANGLE, MAX_ANGLE);

  int startAngle = currentAngle;
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
    currentAngle = startAngle + (angleDistance * easedProgress);
    myservo.write(currentAngle);

    delay(5); // Small delay for smooth movement
  }

  // Ensure we end exactly at target
  currentAngle = targetAngle;
  myservo.write(currentAngle);

  Serial.print("Reached angle: ");
  Serial.println(currentAngle);
}

void loop()
{
  // Process input when a complete string is received
  if (stringComplete)
  {
    // Remove newline
    inputString.trim();

    if (inputString.indexOf(',') == -1)
    {
      // Single angle mode
      int angle = inputString.toInt();
      if (angle >= MIN_ANGLE && angle <= MAX_ANGLE)
      {
        moveToAngle(angle);
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
          moveToAngle(angle);
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

    // Update prompt message with new limits
    inputString = "";
    stringComplete = false;
    Serial.print("\nEnter angle (");
    Serial.print(MIN_ANGLE);
    Serial.print("-");
    Serial.print(MAX_ANGLE);
    Serial.println(") or comma-separated angles:");
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