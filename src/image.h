#pragma once

#include "pixel.h"

#include "common/base.h"

#include <vector>

class Image {
 public:
  unsigned dx, dy;
  std::vector<Pixel> pixels;

  Pixel& operator()(unsigned x, unsigned y) { return pixels[x * dy + y]; }

  const Pixel& operator()(unsigned x, unsigned y) const {
    return pixels[x * dy + y];
  }

  double Size() const { return dx * dy; }
};

inline double Similarity(const Image& i1, const Image& i2) {
  assert((i1.dx == i2.dx) && (i1.dy == i2.dy));
  double s = 0;
  for (unsigned i = 0; i < i1.dx * i1.dy; ++i)
    s += Distance(i1.pixels[i], i2.pixels[i]);
  return s / 20.0;
}
