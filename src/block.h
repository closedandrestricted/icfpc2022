#pragma once

#include <string>

class Block {
 public:
  unsigned x0, x1, y0, y1;
  std::string id;

  double Size() const { return (x1 - x0) * (y1 - y0); }

  bool operator==(const Block& b) const {
    return (x0 == b.x0) && (x1 == b.x1) && (y0 == b.y0) && (y1 == b.y1) &&
           (id == b.id);
  }
};
