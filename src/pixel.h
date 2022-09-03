#pragma once

#include <array>
#include <cmath>
#include <cstdint>

class Pixel {
 public:
  std::array<uint8_t, 4> rgba;

  bool operator==(const Pixel& p) const { return rgba == p.rgba; }
};

inline double Distance(const Pixel& p1, const Pixel& p2) {
  int s = 0;
  for (unsigned i = 0; i < 4; ++i) {
    int d = int(p1.rgba[i]) - int(p2.rgba[i]);
    s += d * d;
  }
  return sqrt(double(s));
}
