#include <SoftPWM.h>   //The library normally uses a 60Hz update rate which is defined in Arduino/hardware/teensy/avr/libraries/SoftPWM/SoftPWM.cpp

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);


#define TWOPI 6.2832
#define PIOVERTWO 1.5708

#define PIN_LED_940 13
#define PIN_LED_850 14
#define PIN_LED_740 15
#define PIN_LED_655 16
#define PIN_LED_627 17
#define PIN_LED_617 18
#define PIN_LED_591 19
#define PIN_LED_568 5
#define PIN_LED_530 6
#define PIN_LED_505 7
#define PIN_LED_470 8
#define PIN_LED_448 9
#define PIN_LED_405 10
#define PIN_LED_367 11
#define PIN_LED_280 12


#define PIN_SEL1 0
#define PIN_SEL2 1
#define PIN_SEL4 2
#define PIN_SEL8 3

#define PIN_BRIGHT1 20
#define PIN_BRIGHT2 21
#define PIN_BRIGHT4 24 //moved from 22
#define PIN_BRIGHT8 25 //moved from 23

uint8_t led_array_pwm[15];
uint8_t led_array_pins[15] = {PIN_LED_280, PIN_LED_367, PIN_LED_405, PIN_LED_448, PIN_LED_470, PIN_LED_505, PIN_LED_530, PIN_LED_568, PIN_LED_591, PIN_LED_617, PIN_LED_627, PIN_LED_655, PIN_LED_740, PIN_LED_850, PIN_LED_940};
const char led_array_wavelenghts[15][6] = {"280nm", "367nm", "405nm", "448nm", "470nm", "505nm", "530nm", "568nm", "591nm", "617nm", "627nm", "655nm", "740nm", "850nm", "940nm"};

float cur_phase = 0;

void setup() {  
  
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  SoftPWMBegin();

  pinMode(PIN_SEL1, INPUT_PULLUP);
  pinMode(PIN_SEL2, INPUT_PULLUP);
  pinMode(PIN_SEL4, INPUT_PULLUP);
  pinMode(PIN_SEL8, INPUT_PULLUP);
  pinMode(PIN_BRIGHT1, INPUT_PULLUP);
  pinMode(PIN_BRIGHT2, INPUT_PULLUP);
  pinMode(PIN_BRIGHT4, INPUT_PULLUP);
  pinMode(PIN_BRIGHT8, INPUT_PULLUP);

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setRotation(2);
  display.setTextSize(1);

  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.setCursor(0, 0);     // Start at top-left corner
  display.println(F("Hello, world!"));

  display.display();



  clear_all_leds();
  delay(1000);
    display.clearDisplay();



  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);     // Start at top-left corner
  display.print("Wavelength:");
  display.setCursor(80, 0);
  display.print("Power:");
  display.setTextSize(2);
  display.display();
}

uint8_t last_brightness=1; //just some random unselectable numbers so the first run of loop will trigger a display refresh.
uint8_t last_selection=64;
bool refresh=false;

void loop() {

  uint8_t brightnessInput = ReadLEDBrightness();
  uint8_t cur_brightness = map(brightnessInput, 0, 15, 10, 255);
  uint8_t cur_selection = ReadLEDSelection();
  String power = String(map(brightnessInput,0,15,0,100)) + "%";

  if(cur_brightness!=last_brightness || cur_selection!=last_selection) refresh=true;

  if(refresh){
    display.fillRect(0, 10, display.width(), display.height(), SSD1306_BLACK);
    display.setCursor(0,10);
  }
  clear_all_leds();

  cur_phase = (cur_phase > 7) ? -10 : cur_phase + 0.08;

  if (cur_selection == 0)
  {
    for (int i = 0; i < 15; i++)
    {
      led_array_pwm[i] = 255 * sine_pulse(cur_phase + i);
    }
   if(refresh){
   display.print("RAINBOW!");
   }
  }

  if (cur_selection > 0 && cur_selection < 16)
  {
    led_array_pwm[cur_selection - 1] = cur_brightness;
      if(refresh){
        display.print(led_array_wavelenghts[cur_selection - 1]);
        display.setCursor(75,10);
        display.print(power.c_str());
      }
  }
  update_leds();

     if(refresh){
        display.display();
        last_selection=cur_selection;
        last_brightness=cur_brightness;
        refresh=false;
     }


 // delay(16);
}




float sine_pulse(float input)  {
  if (input < 0)
  {
    return 0;
  }
  else if (input > TWOPI)
  {
    return 0;
  }
  else
  {
    return (1 + sin(input - PIOVERTWO)) / 2;
  }
}

void update_leds(void)  {
  for (int i = 0; i < 15; i++)
  {
    SoftPWMSet(led_array_pins[i], led_array_pwm[i]);
  }
}


void clear_all_leds(void)  {
  for (int i = 0; i < 15; i++)
  {
    led_array_pwm[i] = 0;
  }
}


uint8_t ReadLEDSelection(void)  {
  uint8_t result = 0;
  result += !digitalRead(PIN_SEL1) << 0;
  result += !digitalRead(PIN_SEL2) << 1;
  result += !digitalRead(PIN_SEL4) << 2;
  result += !digitalRead(PIN_SEL8) << 3;
  return result;
}


uint8_t ReadLEDBrightness(void)  {
  uint8_t result = 0;
  result += !digitalRead(PIN_BRIGHT1) << 0;
  result += !digitalRead(PIN_BRIGHT2) << 1;
  result += !digitalRead(PIN_BRIGHT4) << 2;
  result += !digitalRead(PIN_BRIGHT8) << 3;
  return result;
}
