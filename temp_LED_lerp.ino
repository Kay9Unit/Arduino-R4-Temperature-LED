#define TEMPERATURE_PIN 0
#define RED_PIN 9
#define GREEN_PIN 10
#define BLUE_PIN 11

int ledPins[3] = {RED_PIN, GREEN_PIN, BLUE_PIN};

void setup()
{ 
  Serial.begin(9600);

  for (auto p : ledPins) pinMode(p, OUTPUT);
}


void loop()
{
  float voltage, degreesC, degreesF;

  // get and convert voltage to degrees
  voltage = getVoltage( );
  degreesC = (voltage - 0.5) * 100.0;
  degreesF = degreesC * (9.0/5.0) + 32.0;

  // print out, for debugging purposes
  Serial.print("voltage: ");
  Serial.print(voltage);
  Serial.print("  deg C: ");
  Serial.print(degreesC);
  Serial.print("  deg F: ");
  Serial.println(degreesF);

  // turn on the LED that is designated
  // for its degree range.
  writeToPinsByTemp(degreesF);

  delay(10);
}

float getVoltage(int pin)
{  
  return (analogRead(pin) * 0.004882814);
}

#define COLDEST 70 // coldest 'allowed' temp. aqua remains for lower temps
#define HOTTEST 80 // hottest 'allowed' temp. pure red remains for higher temps
#define HOT_COLOR 0xFF0000 // pure red in RRGGBB
#define COLD_COLOR 0x00FFFF // cool aqua blue in RRGGBB

void writeToPinsByTemp(float degreesF)
{
  // get the fraction of degree we are at between our min and max
  float step = (degreesF - COLDEST) / (HOTTEST - COLDEST);
  // normalize step to 0-1, when our temperatures exceed our min and max.
  step = step < 0? 0 : step > 1? 1 : step;
  // interpolate between our coldest and hottest colors using step.
  // 0 is blue-est, 1 is red-est
  unsigned int color = lerpColor(COLD_COLOR, HOT_COLOR, step);

  // debug step and color lerping
  Serial.print(step); Serial.print(" "); Serial.println(String(color, HEX));

  // extract individual colors from color hex
  unsigned int red = (color >> 16) & 0xFF;
  unsigned int green = (color >> 8) & 0xFF;
  unsigned int blue = color & 0xFF;

  // send pwm signal to LED pins
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}

int lerpColor(int color1, int color2, float x)
{
  // extract individual colors of our first color
  int r1 = (color1 >> 16) & 0xFF;
  int g1 = (color1 >> 8) & 0xFF;
  int b1 = color1 & 0xFF;

  // extract individual colors of our second color
  int r2 = (color2 >> 16) & 0xFF;
  int g2 = (color2 >> 8) & 0xFF;
  int b2 = color2 & 0xFF;

  // interpolate between our colors based on delta x
  int r = lerp(r1, r2, x);
  int g = lerp(g1, g2, x);
  int b = lerp(b1, b2, x);

  // construct new lerped color hex
  return (r << 16) | (g << 8) | b;
}

int lerp(int start, int end, float x)
{
  // math. I hate math.
  return (1 - x) * start + x * end;
}