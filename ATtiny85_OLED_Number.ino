#include <inttypes.h>
#include "SBC-OLED01.h"

typedef SBS_OLED01 OLED;
static byte constexpr ADDRESS = 0x3C;

static byte const input_pins[4] = { 1, 3, 4, 5 };

template <uint8_t margin>
class glyph_shape {
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
    const byte* segAt(uint8_t x) const {
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

  public:
    constexpr glyph_shape(const char* col_per_row)
      : seg0(0
             | (col_per_row[0 * SEGS + 0] & 1) << 0
             | (col_per_row[1 * SEGS + 0] & 1) << 1
             | (col_per_row[2 * SEGS + 0] & 1) << 2
             | (col_per_row[3 * SEGS + 0] & 1) << 3
             | (col_per_row[4 * SEGS + 0] & 1) << 4
             | (col_per_row[5 * SEGS + 0] & 1) << 5
             | (col_per_row[6 * SEGS + 0] & 1) << 6
             | (col_per_row[7 * SEGS + 0] & 1) << 7)
      , seg1(0
             | (col_per_row[0 * SEGS + 1] & 1) << 0
             | (col_per_row[1 * SEGS + 1] & 1) << 1
             | (col_per_row[2 * SEGS + 1] & 1) << 2
             | (col_per_row[3 * SEGS + 1] & 1) << 3
             | (col_per_row[4 * SEGS + 1] & 1) << 4
             | (col_per_row[5 * SEGS + 1] & 1) << 5
             | (col_per_row[6 * SEGS + 1] & 1) << 6
             | (col_per_row[7 * SEGS + 1] & 1) << 7)
      , seg2(0
             | (col_per_row[0 * SEGS + 2] & 1) << 0
             | (col_per_row[1 * SEGS + 2] & 1) << 1
             | (col_per_row[2 * SEGS + 2] & 1) << 2
             | (col_per_row[3 * SEGS + 2] & 1) << 3
             | (col_per_row[4 * SEGS + 2] & 1) << 4
             | (col_per_row[5 * SEGS + 2] & 1) << 5
             | (col_per_row[6 * SEGS + 2] & 1) << 6
             | (col_per_row[7 * SEGS + 2] & 1) << 7)
      , seg3(0
             | (col_per_row[0 * SEGS + 3] & 1) << 0
             | (col_per_row[1 * SEGS + 3] & 1) << 1
             | (col_per_row[2 * SEGS + 3] & 1) << 2
             | (col_per_row[3 * SEGS + 3] & 1) << 3
             | (col_per_row[4 * SEGS + 3] & 1) << 4
             | (col_per_row[5 * SEGS + 3] & 1) << 5
             | (col_per_row[6 * SEGS + 3] & 1) << 6
             | (col_per_row[7 * SEGS + 3] & 1) << 7)
      , seg4(0
             | (col_per_row[0 * SEGS + 4] & 1) << 0
             | (col_per_row[1 * SEGS + 4] & 1) << 1
             | (col_per_row[2 * SEGS + 4] & 1) << 2
             | (col_per_row[3 * SEGS + 4] & 1) << 3
             | (col_per_row[4 * SEGS + 4] & 1) << 4
             | (col_per_row[5 * SEGS + 4] & 1) << 5
             | (col_per_row[6 * SEGS + 4] & 1) << 6
             | (col_per_row[7 * SEGS + 4] & 1) << 7)
      , seg5(0
             | (col_per_row[0 * SEGS + 5] & 1) << 0
             | (col_per_row[1 * SEGS + 5] & 1) << 1
             | (col_per_row[2 * SEGS + 5] & 1) << 2
             | (col_per_row[3 * SEGS + 5] & 1) << 3
             | (col_per_row[4 * SEGS + 5] & 1) << 4
             | (col_per_row[5 * SEGS + 5] & 1) << 5
             | (col_per_row[6 * SEGS + 5] & 1) << 6
             | (col_per_row[7 * SEGS + 5] & 1) << 7)
      , seg6(0
             | (col_per_row[0 * SEGS + 6] & 1) << 0
             | (col_per_row[1 * SEGS + 6] & 1) << 1
             | (col_per_row[2 * SEGS + 6] & 1) << 2
             | (col_per_row[3 * SEGS + 6] & 1) << 3
             | (col_per_row[4 * SEGS + 6] & 1) << 4
             | (col_per_row[5 * SEGS + 6] & 1) << 5
             | (col_per_row[6 * SEGS + 6] & 1) << 6
             | (col_per_row[7 * SEGS + 6] & 1) << 7)
      , seg7(0
             | (col_per_row[0 * SEGS + 7] & 1) << 0
             | (col_per_row[1 * SEGS + 7] & 1) << 1
             | (col_per_row[2 * SEGS + 7] & 1) << 2
             | (col_per_row[3 * SEGS + 7] & 1) << 3
             | (col_per_row[4 * SEGS + 7] & 1) << 4
             | (col_per_row[5 * SEGS + 7] & 1) << 5
             | (col_per_row[6 * SEGS + 7] & 1) << 6
             | (col_per_row[7 * SEGS + 7] & 1) << 7)
    {}

    // Assumes this is in PROGMEM.
    // Assumes set_page_address over two pages is set up.
    void sendOverDoublePage(OLED::BareChat& chat) const {
      chat.sendN(2 * margin, 0);
      for (uint8_t x = 0; x < SEGS; ++x) {
        byte seg = pgm_read_byte(segAt(x));
        chat.send(seg << 4);
        chat.send(seg >> 4);
      }
      chat.sendN(2 * margin, 0);
    }
};

typedef glyph_shape<0> narrow_glyph_shape;
typedef glyph_shape<1> normal_glyph_shape;


static normal_glyph_shape const digit_shapes[10] PROGMEM = {
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

static narrow_glyph_shape const pin_shapes[2] PROGMEM = {
  {
    "###     "
    "#  #  # "
    "#  #    "
    "###   # "
    "#     # "
    "#     # "
    "#     # "
    "#      #"
  }, {
    "        "
    "        "
    " # ##   "
    " ##  #  "
    " #   #  "
    " #   #  "
    " #   #  "
    " #   #  "
  }
};

static narrow_glyph_shape const colon_shape PROGMEM = {
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
    delay(300);
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

static OLED::Status displayNumber(uint8_t quarter, uint8_t id, uint8_t number) {
  auto chat = OLED::Chat(ADDRESS, 20)
              .set_column_address(0, 3 * narrow_glyph_shape::WIDTH + 4 * normal_glyph_shape::WIDTH - 1)
              .set_page_address(quarter * 2, quarter * 2 + 1)
              .start_data();
  uint8_t d1 = number / 100;
  uint8_t d2 = (number / 10) % 10;
  uint8_t d3 = number % 10;
  pin_shapes[0].sendOverDoublePage(chat);
  pin_shapes[1].sendOverDoublePage(chat);
  digit_shapes[id].sendOverDoublePage(chat);
  colon_shape.sendOverDoublePage(chat);
  digit_shapes[d1].sendOverDoublePage(chat);
  digit_shapes[d2].sendOverDoublePage(chat);
  digit_shapes[d3].sendOverDoublePage(chat);
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
  for (uint8_t i = 0; i < sizeof input_pins; ++i) {
    pinMode(input_pins[i], INPUT);
  }
}

void loop() {
  for (uint8_t i = 0; i < sizeof input_pins; ++i) {
    reportError(displayNumber(i, input_pins[i], analogRead(input_pins[i])));
  }
  delay(200);
}
