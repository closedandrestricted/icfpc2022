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
  Move(Type _type, const std::string& id, const Pixel& _color)
      : type(_type), block_id1(id), x(0), y(0), color(_color) {}

  std::string Encode() const;
  bool Decode(const std::string& s);
};
