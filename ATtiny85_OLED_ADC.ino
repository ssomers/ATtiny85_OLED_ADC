#include <inttypes.h>
#include "OLED.h"
#include "Glyph.h"
#include "GlyphsOnQuarter.h"

static byte constexpr ADDRESS = 0x3C;

static byte constexpr input_pins[] = { PIN3, PIN4, PIN5 };

static inline int8_t pin_to_adc_channel(byte pin) {
  switch (pin) {
    case PIN2: return 1;
    case PIN3: return 3;
    case PIN4: return 2;
    case PIN5: return 0;
    default: return -1;
  }
}

static void flashN(uint8_t number) {
  while (number >= 5) {
    number -= 5;
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(700);
    digitalWrite(LED_BUILTIN, LOW);
  }
  while (number >= 1) {
    number -= 1;
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
  }
}

static void reportError(I2C::Status status) {
  if (status.errorlevel) {
    delay(300);
    flashN(status.errorlevel);
    delay(300);
    flashN(status.location);
    delay(900);
  }
}

static void displayPinValue(uint8_t quarter, uint8_t pin, int value) {
  uint8_t constexpr content_width = 4 * Glyph::SEGS + GlyphsOnQuarter::WIDTH_4DIGIT_NUMBER;
  uint8_t const displayed_width = quarter == 0 ? OLED::WIDTH : content_width;
  auto chat = OLED::QuarterChat{ADDRESS, quarter, 0, uint8_t(displayed_width - 1)};
  GlyphsOnQuarter::sendOnQuarter(chat, Glyph::pin[0]);
  GlyphsOnQuarter::sendOnQuarter(chat, Glyph::pin[1]);
  GlyphsOnQuarter::sendOnQuarter(chat, Glyph::digit[pin]);
  GlyphsOnQuarter::sendOnQuarter(chat, Glyph::colon);
  GlyphsOnQuarter::send4digitNumberOnQuarter(chat, value);
  if (quarter == 0) {
    chat.sendSpacing(OLED::WIDTH - content_width - 2);
    static bool toggle = 0;
    toggle ^= 1;
    for (uint8_t _ = 0; _ < 2; ++_) {
      chat.send(0b1111 << (toggle * 2), 0);
    }
  }
  reportError(chat.stop());
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  USI_TWI_Master_Initialise();
  auto err = OLED::Chat(ADDRESS, 0)
             .init()
             .set_addressing_mode(OLED::VerticalAddressing)
             .set_column_address()
             .set_page_address()
             .set_enabled()
             .start_data()
             .sendN(OLED::BYTES, 0x0)
             .stop();
  digitalWrite(LED_BUILTIN, LOW);
  reportError(err);
  for (uint8_t i = 0; i < sizeof input_pins; ++i) {
    pinMode(input_pins[i], INPUT);
  }
}

void loop() {
  for (uint8_t i = 0; i < sizeof input_pins; ++i) {
    int value = analogRead(pin_to_adc_channel(input_pins[i]));
    displayPinValue(i, input_pins[i], value);
  }
  delay(200);
}
