#pragma once

#include <string>

class Block {
 public:
  unsigned x0, x1, y0, y1;
  std::string id;

  double Size() const { return (x1 - x0) * (y1 - y0); }
};
