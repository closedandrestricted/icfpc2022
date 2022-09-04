#pragma once

#include <string>
#include <utility>

class Block {
 public:
  unsigned x0, x1, y0, y1;
  std::string id;

  double Size() const { return (x1 - x0) * (y1 - y0); }
  unsigned Width() const { return x1 - x0; };
  unsigned Height() const { return y1 - y0; };
  std::pair<unsigned, unsigned> Dim() const {
    return std::make_pair(Width(), Height());
  }
};
