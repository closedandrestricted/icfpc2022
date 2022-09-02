#pragma once

#include "pixel.h"

#include "common/base.h"

#include <string>
#include <vector>

class Image {
 public:
  unsigned dx, dy;
  std::vector<Pixel> pixels;

  Pixel &operator()(unsigned x, unsigned y) { return pixels[x * dy + y]; }

  const Pixel &operator()(unsigned x, unsigned y) const {
    return pixels[x * dy + y];
  }

  double Size() const { return dx * dy; }

  void Init(unsigned _dx, unsigned _dy);
  bool Load(const std::string &filename);
};
