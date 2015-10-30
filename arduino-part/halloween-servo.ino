#include <Servo.h>
#include <Adafruit_NeoPixel.h>

#define ARM_LEFT_PIN 5
#define ARM_RIGHT_PIN 6
#define EYES_PIN 10
#define EYES_LEDS 2

//#define DEBUGFN Serial.println(__PRETTY_FUNCTION__)
#define DEBUGFN

struct Color {
  unsigned char r, g, b;
};

struct Color colors[] = {
  {255, 0, 0},
  {0, 255, 0},
  {0, 0, 255},
  {255, 0, 255},
  {255, 255, 0},
  {0, 255, 255},
  {160, 0, 255},
  {255, 160, 0}
};

#define COLORS_NUMBER (sizeof(colors)/3)

class Hands {
private:
  Servo left;
  Servo right;
  int leftPin;
  int rightPin;
  unsigned long timeStart;
  unsigned long timeEnd;
  bool isOn, wasOn;
  int upDown;
public:
  Hands(int leftPin, int rightPin): leftPin(leftPin), rightPin(rightPin), timeStart(0), isOn(false), wasOn(false), upDown(0) {
    
  }

  ~Hands() {
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
    left.detach();
    right.detach();
  }

private:

  void open() {
    DEBUGFN;
    left.attach(leftPin);
    right.attach(rightPin);
  }

  void close() {
    DEBUGFN;
    left.detach();
    right.detach();
    isOn = false;
    wasOn = false;
  }

  void waveUp() {
    DEBUGFN;
    upDown = 1;
    left.write(90+30);
    right.write(90-30);
  }

  void waveDown() {
    DEBUGFN;
    upDown = -1;
    left.write(90-30);
    right.write(90+30);
  }

  void moveDown() {
    DEBUGFN;
    upDown = 0;
    left.write(0);
    right.write(180);
  }

public:
  void start() {
    DEBUGFN;
    timeStart = millis();
    isOn = true;
    wasOn = true;
    open();
    waveUp();
  }

  void stop() {
    DEBUGFN;
    if (isOn) {
      isOn = false;
      timeEnd = millis();
      moveDown();
    }
  }

  void init() {
    DEBUGFN;
    open();
    moveDown();
    close();
  }

  void update() {
    unsigned long curTime = millis();
    unsigned long totalDelta;
    int result;
    
    if (!isOn && !wasOn) {
      // Nothing to do.
      return;
    } else if (!isOn && wasOn) {
      // Moving down hands.
      totalDelta = curTime - timeEnd;
      if (totalDelta > 800) {
        // Assume the hands moved down.
        close();
      }
      return;
    }
    
    
    totalDelta = curTime - timeStart;
    
    if (totalDelta < 1000) {
      // Arms not up yet.
      return;
    } else {
      // Arms ready to wave.
      result = (totalDelta / 300) % 2;
      if (result == 0 && upDown != 1) {
        waveUp();
      } else if (result == 1 && upDown != -1) {
        waveDown();
      }
    }
  }
};

class Eyes {
private:
  Adafruit_NeoPixel pixels;
  unsigned long timeStart;
  bool isOn, wasOn;
  int periodLength;
  int currentColor;
  int lastPeriod;
  int leds;
public:
  Eyes(int pin, int leds)
  : pixels(leds, pin, NEO_GRB + NEO_KHZ800),
    timeStart(0), isOn(false), wasOn(false),
    currentColor(0), lastPeriod(0), leds(leds)
  {
    
  }

  void init() {
    pixels.begin();
    pixels.show();
    periodLength = 800;
  }

  void turnOff() {
    for(int i = 0; i < leds; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();
  }

  void start() {
    if (!wasOn) {
      // Update time start only if previous animation ended.
      timeStart = millis();
      currentColor = random(COLORS_NUMBER);
    }
    
    isOn = true;
    wasOn = true;
  }

  void stop() {
    isOn = false;
  }

  void update() {
    unsigned long curTime = millis() - timeStart;
    unsigned long period = curTime / periodLength;
    unsigned long periodPos = curTime % periodLength;
    float result = sin(periodPos / float(periodLength) * PI);

    if (!wasOn && !isOn) {
      return;
    }

    if (period > lastPeriod) {
      currentColor = random(COLORS_NUMBER);
    }
    
    
    uint32_t c = pixels.Color(
        colors[currentColor].r * result,
        colors[currentColor].g * result,
        colors[currentColor].b * result
      );
   
//    uint32_t c = pixels.Color(0, 255, 255);
    for(int i = 0; i < leds; i++) {
      pixels.setPixelColor(i, c);
    }
    pixels.show();

    /*
    Serial.print(period); Serial.print(" ");
    Serial.print(periodPos); Serial.print(" ");
    Serial.print(result); Serial.print(" - ");
    Serial.print(currentColor); Serial.print(") ");
    Serial.print(colors[currentColor].r * result); Serial.print(" ");
    Serial.print(colors[currentColor].g * result); Serial.print(" ");
    Serial.println(colors[currentColor].b * result);
    */

    if (!isOn && lastPeriod < period) {
      // Finishing animation.
      turnOff();
      wasOn = false;
      isOn = false;
      return;
    }
  
    lastPeriod = period;
  }
  
};


Hands hands(ARM_LEFT_PIN, ARM_RIGHT_PIN);
Eyes eyes(EYES_PIN, EYES_LEDS);

void setup() {
  Serial.begin(9600);
  hands.init();
  eyes.init();
}

void loop() {
  int c;
  while (Serial.available()) {
    c = Serial.read();
    if (c == '1') {
      hands.start();
      eyes.start();
      digitalWrite(13, HIGH);
    } else if (c == '0') {
      hands.stop();
      eyes.stop();
      digitalWrite(13, LOW);
    }
  }

  hands.update();
  eyes.update();
  delay(1);
}


