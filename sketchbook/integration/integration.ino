#include <TM1637Display.h>
#include <Servo.h>
#include "RTClib.h"


#include <Adafruit_LEDBackpack.h>
#include <Adafruit_GFX.h>

struct PowerCtrl {
  int pin;

  void begin() {
    digitalWrite(pin, LOW);
    pinMode(pin, OUTPUT);
  }
  void on() {
    digitalWrite(pin, HIGH);
  }
  void off() {
    digitalWrite(pin, LOW);
  }
};


struct Button {
  uint8_t pin;
  Button(int pin)
    : pin(pin) {}
  static constexpr int threshold = 16;
  bool pressed = false;
  int accum = threshold;

  void begin() {
    pinMode(pin, INPUT_PULLUP);
  }

  bool poll() {
    int v = digitalRead(pin);    
    accum = min(threshold, max(0, accum + v * 2 - 1));
    if (accum == 0) pressed = true;
    if (accum == 1) pressed = false;
    return pressed;
  }
  bool wait(bool state, int timeout_ms) {
    unsigned long walltime = millis() + timeout_ms;
    while (poll() != state && millis() < walltime)
      ;
    return pressed;
  }
};

RTC_DS3231 rtc;
PowerCtrl power{ .pin = 4 };
Button up{ 6 };
Servo myservo;  // create servo object to control a servo

TM1637Display getDisplay() {
  return {
    2, 3
  };
}

void setup() {
  // Enable logging through the serial port.
  Serial.begin(115200);
  Serial.println("Serial ok");

  power.begin();
  Serial.println("Power ok");

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1) delay(10);
  }
  Serial.println("RTC ok");

  up.begin();

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  printDate();
}

int count = 2;

void loop() {
  setDateTimeWorkflow();
  power.on();
  delay(5);

  auto display = getDisplay();
  display.setBrightness(0x0f);

  if (count) {
    count--;
    myservo.attach(9);
    myservo.write(0);
  }

  for (uint64_t i = 1; i < 32; i++) {
    uint64_t segments = 1UL << i;
    display.setSegments(reinterpret_cast<char*>(&segments));
    delay(100);  // wait for a second.
  }

  myservo.write(180);

  delay(1000);
  power.off();
  delay(2000);  // wait for a second
}


// struct Butons {
//   uint8_t up_pin, down_pin, enter_pin;
//   bool up, down, enter;
//   void get_press() {

//   }
// }

void setDateTimeWorkflow() {
  auto to_char = [](int i) {
    return '0' + (i % 10);
  };
  DateTime now = rtc.now();
  power.on();
  auto display = getDisplay();

  const char text[5] = { to_char(now.hour() / 10), to_char(now.hour()),
                         to_char(now.minute() / 10), to_char(now.minute()), 0 };
  Serial.println(text);
  display.setSegments(text);

  Adafruit_7segment disp = Adafruit_7segment();
  disp.begin(0x71);

  auto showTime = [&](bool minutes) {    
    disp.clear();
    disp.writeDigitNum(0, now.hour() / 10);
    disp.writeDigitNum(1, now.hour() % 10);
    disp.writeDigitRaw(2, 0xff);
    if (minutes) {
      disp.writeDigitNum(3, now.minute() / 10);
      disp.writeDigitNum(4, now.minute() % 10);
    }
    disp.writeDisplay();
  };

  while (true) {
    unsigned long time = millis();
    bool on = (time >> 9) & 1;
    showTime(on);
    
    if (up.wait(true, 100)) {
      Serial.println("UP");
      do {
        now = now + TimeSpan(0, 0, 1, 0);
        showTime(true);
      } while (up.wait(false, 100));
    }    
  }
  power.off();
}
void printDate() {
  constexpr char daysOfTheWeek[7][12] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
  };

  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  Serial.print("Temperature: ");
  Serial.print(rtc.getTemperature());
  Serial.println(" C");

  Serial.println();
}
