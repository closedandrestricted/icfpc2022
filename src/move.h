#pragma once

#include "pixel.h"

#include <string>

class Move {
 public:
  enum Type { SKIP, LINE_CUT, POINT_CUT, COLOR, SWAP, MERGE, END };

  Type type;
  std::string block_id1, block_id2;
  unsigned x{};
  unsigned y{};
  Pixel color;

  Move() : type(END) {}
  Move(Type _type) : type(_type) {}
  Move(Type _type, const std::string& id, unsigned _x, unsigned _y)
      : type(_type), block_id1(id), x(_x), y(_y) {}
  Move(Type _type, const std::string& id, const Pixel& _color)
      : type(_type), block_id1(id), color(_color) {}
  Move(Type _type, const std::string& id1, const std::string& id2)
      : type(_type), block_id1(id1), block_id2(id2) {}

  std::string Encode() const;
  bool Decode(const std::string& s);

  bool operator==(const Move& m) const {
    return (type == m.type) && (block_id1 == m.block_id1) &&
           (block_id2 == m.block_id2) && (x == m.x) && (y == m.y) &&
           (color == m.color);
  }
};
