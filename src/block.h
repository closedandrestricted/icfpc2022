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

  using TDim = std::pair<unsigned, unsigned>;
  TDim Dim() const { return std::make_pair(Width(), Height()); }

  bool operator==(const Block& b) const {
    return (x0 == b.x0) && (x1 == b.x1) && (y0 == b.y0) && (y1 == b.y1) &&
           (id == b.id);
  }
};
