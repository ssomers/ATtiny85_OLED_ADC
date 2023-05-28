#pragma once
#include "Glyph.h"
#include "OLED.h"

struct GlyphsOnQuarter {
  static constexpr uint8_t MARGIN = 1;
  static constexpr uint8_t WIDTH_4DIGIT_NUMBER = 4 * (MARGIN + Glyph::SEGS + MARGIN);

  static void sendOnQuarter(OLED::QuarterChat& chat, Glyph const& glyph, uint8_t margin = 0) {
    chat.sendN(2 * margin, 0);
    for (uint8_t x = 0; x < Glyph::SEGS; ++x) {
      byte seg = glyph.seg(x);
      chat.send(seg << 4, seg >> 4);
    }
    chat.sendN(2 * margin, 0);
  }

  static void send4digitNumberOnQuarter(OLED::QuarterChat& chat, int number) {
    constexpr uint8_t NUM_ERR_GLYPHS = 5;
    static_assert(WIDTH_4DIGIT_NUMBER == NUM_ERR_GLYPHS * Glyph::SEGS);
    if (number < 0) {
      for (uint8_t _ = 0; _ < NUM_ERR_GLYPHS; ++_) {
        sendOnQuarter(chat, Glyph::minus, 0);
      }
      return;
    }
    uint8_t const p1 = number / 100;
    uint8_t const p2 = number % 100;
    if (p1 >= 100) {
      for (uint8_t _ = 0; _ < NUM_ERR_GLYPHS; ++_) {
        sendOnQuarter(chat, Glyph::overflow, 0);
      }
      return;
    }
    sendOnQuarter(chat, Glyph::digit[p1 / 10], MARGIN);
    sendOnQuarter(chat, Glyph::digit[p1 % 10], MARGIN);
    sendOnQuarter(chat, Glyph::digit[p2 / 10], MARGIN);
    sendOnQuarter(chat, Glyph::digit[p2 % 10], MARGIN);
  }
};
