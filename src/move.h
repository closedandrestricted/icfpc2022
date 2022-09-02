#pragma once

#include "pixel.h"

#include <string>

class Move {
 public:
  enum Type { SKIP, LINE_CUT, POINT_CUT, COLOR, SWAP, MERGE, END };

  Type type;
  std::string block_id1, block_id2;
  unsigned x, y;
  Pixel color;

  Move() : type(END), x(0), y(0) {}

  std::string Encode() const;
  bool Decode(const std::string& s);
};
