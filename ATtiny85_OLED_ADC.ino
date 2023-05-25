#include <inttypes.h>
#include "SBC-OLED01.h"

typedef SBS_OLED01 OLED;
static byte constexpr ADDRESS = 0x3C;

static byte const input_pins[] = { 3, 4, 5 };
static byte const adc_ids[] = { 3, 2, 0 };

template <uint8_t margin>
class glyph {
  public:
    static uint8_t constexpr SEGS = 8;
    static uint8_t constexpr WIDTH = margin + SEGS + margin;

  private:
    byte const seg0;
    byte const seg1;
    byte const seg2;
    byte const seg3;
    byte const seg4;
    byte const seg5;
    byte const seg6;
    byte const seg7;

#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wreturn-type"
    const byte* locateSegAt(uint8_t x) const {
      switch (x) {
        case 0: return &seg0;
        case 1: return &seg1;
        case 2: return &seg2;
        case 3: return &seg3;
        case 4: return &seg4;
        case 5: return &seg5;
        case 6: return &seg6;
        case 7: return &seg7;
      }
    }
#   pragma GCC diagnostic pop

    constexpr static bool pixel(char c) {
      return c != ' ';
    }

    constexpr static byte extractSegAt(int x, const char* col_per_row) {
      return 0
             | pixel(col_per_row[x + SEGS * 0]) << 0
             | pixel(col_per_row[x + SEGS * 1]) << 1
             | pixel(col_per_row[x + SEGS * 2]) << 2
             | pixel(col_per_row[x + SEGS * 3]) << 3
             | pixel(col_per_row[x + SEGS * 4]) << 4
             | pixel(col_per_row[x + SEGS * 5]) << 5
             | pixel(col_per_row[x + SEGS * 6]) << 6
             | pixel(col_per_row[x + SEGS * 7]) << 7;
    }

  public:
    constexpr glyph(const char* col_per_row)
      : seg0(extractSegAt(0, col_per_row))
      , seg1(extractSegAt(1, col_per_row))
      , seg2(extractSegAt(2, col_per_row))
      , seg3(extractSegAt(3, col_per_row))
      , seg4(extractSegAt(4, col_per_row))
      , seg5(extractSegAt(5, col_per_row))
      , seg6(extractSegAt(6, col_per_row))
      , seg7(extractSegAt(7, col_per_row))
    {}

    // Assumes this instance is in PROGMEM.
    // Assumes set_page_address over two pages is set up.
    void sendOnQuarter(OLED::BareChat& chat) const {
      chat.sendN(2 * margin, 0);
      for (uint8_t x = 0; x < SEGS; ++x) {
        byte seg = pgm_read_byte(locateSegAt(x));
        chat.send(seg << 4);
        chat.send(seg >> 4);
      }
      chat.sendN(2 * margin, 0);
    }
};

typedef glyph<0> narrow_glyph;
typedef glyph<1> normal_glyph;


static normal_glyph const digit_glyphs[10] PROGMEM = {
  {
    " ###### "
    "##    ##"
    "##    ##"
    "##    ##"
    "##    ##"
    "##    ##"
    "##    ##"
    " ###### "
  }, {
    "    ### "
    "   #### "
    "  ## ## "
    " ##  ## "
    "     ## "
    "     ## "
    "     ## "
    "     ## "
  }, {
    " ###### "
    "##    ##"
    "      ##"
    "     ## "
    "    ##  "
    "   ##   "
    " ##     "
    "########"
  }, {
    " ###### "
    "##    ##"
    "      ##"
    "   #### "
    "      ##"
    "      ##"
    "##    ##"
    " ###### "
  }, {
    "   #### "
    "  ## ## "
    " ##  ## "
    "##   ## "
    "##   ## "
    "########"
    "     ## "
    "     ## "
  }, {
    "########"
    "##      "
    "##      "
    "####### "
    "      ##"
    "      ##"
    "##    ##"
    " ###### "
  }, {
    "  ##### "
    " ##   ##"
    "##      "
    "# ##### "
    "##    ##"
    "##    ##"
    "##    ##"
    " ###### "
  }, {
    "########"
    "      ##"
    "     ## "
    "    ##  "
    "   ##   "
    "   ##   "
    "   ##   "
    "   ##   "
  }, {
    " ###### "
    "##    ##"
    "##    ##"
    " ###### "
    "##    ##"
    "##    ##"
    "##    ##"
    " ###### "
  }, {
    " ###### "
    "##    ##"
    "##    ##"
    " ###### "
    "      ##"
    "      ##"
    "##   ## "
    " #####  "
  }
};

static narrow_glyph const pin_glyphs[2] PROGMEM = {
  {
    "###   # "
    "#  #    "
    "#  #  # "
    "###   # "
    "#     # "
    "#     # "
    "#     # "
    "#      #"
  }, {
    "        "
    "        "
    "  # ##  "
    "  ##  # "
    "  #   # "
    "  #   # "
    "  #   # "
    "  #   # "
  }
};

static narrow_glyph const minus_glyph PROGMEM = {
  "        "
  "        "
  "        "
  " ###### "
  " ###### "
  "        "
  "        "
  "        "
};

static narrow_glyph const plus_glyph PROGMEM = {
  "        "
  "        "
  "    #   "
  "    #   "
  "  ##### "
  "    #   "
  "    #   "
  "        "
};

static narrow_glyph const colon_glyph PROGMEM = {
  "        "
  "   ##   "
  "   ##   "
  "        "
  "        "
  "   ##   "
  "   ##   "
  "        "
};

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

static void reportError(OLED::Status status) {
  if (status.errorlevel) {
    delay(300);
    flashN(status.errorlevel);
    delay(300);
    flashN(status.location);
    delay(900);
  }
}

OLED::BareChat startChatAtQuarter(uint8_t quarter, uint8_t width) {
  return OLED::Chat(ADDRESS, 20)
         .set_page_address(quarter * 2, quarter * 2 + 1)
         .set_column_address(0, width - 1)
         .start_data();
}

static void sendSignOnQuarter(int signedNumber, OLED::BareChat& chat) {
  if (signedNumber < 0) {
    minus_glyph.sendOnQuarter(chat);
  } if (signedNumber > 0) {
    plus_glyph.sendOnQuarter(chat);
  } else {
    chat.sendN(narrow_glyph::WIDTH * 2, 0);
  }
}

static void sendNumberOnQuarter(uint16_t number, OLED::BareChat& chat) {
  uint8_t const p1 = number / 100;
  uint8_t const p2 = number % 100;
  digit_glyphs[p1 / 10].sendOnQuarter(chat);
  digit_glyphs[p1 % 10].sendOnQuarter(chat);
  digit_glyphs[p2 / 10].sendOnQuarter(chat);
  digit_glyphs[p2 % 10].sendOnQuarter(chat);
}

static OLED::Status displayPinValue(uint8_t quarter, uint8_t pin, int value) {
  auto chat = startChatAtQuarter(quarter, 4 * narrow_glyph::WIDTH + 5 * normal_glyph::WIDTH);
  pin_glyphs[0].sendOnQuarter(chat);
  pin_glyphs[1].sendOnQuarter(chat);
  digit_glyphs[pin].sendOnQuarter(chat);
  colon_glyph.sendOnQuarter(chat);
  sendSignOnQuarter(value, chat);
  sendNumberOnQuarter(abs(value), chat);
  return chat.stop();
}

static OLED::Status displayNumber(uint8_t quarter, int number) {
  auto chat = startChatAtQuarter(quarter, 4 * normal_glyph::WIDTH);
  sendNumberOnQuarter(number, chat);
  return chat.stop();
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
  pinMode(1, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
}

void loop() {
  static int counter = 0;
  reportError(displayNumber(0, counter));
  if (++counter > 9999) counter = 0;
  for (uint8_t i = 0; i < sizeof input_pins; ++i) {
    reportError(displayPinValue(1 + i, input_pins[i], analogRead(adc_ids[i])));
  }
  delay(200);
}
