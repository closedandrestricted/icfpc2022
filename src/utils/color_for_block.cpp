#include "utils/color_for_block.h"

#include "pixel.h"

#include "common/base.h"

#include <array>

double Cost(const Block& block, const Image& image, const Pixel& color) {
  double s = 0;
  for (unsigned x = block.x0; x < block.x1; ++x) {
    for (unsigned y = block.y0; y < block.y1; ++y)
      s += Distance(image(x, y), color);
  }
  return s;
}

Pixel ColorMSE(const Block& block, const Image& image) {
  assert(block.Size() > 0);
  std::array<unsigned, 4> as{0, 0, 0, 0};
  for (unsigned x = block.x0; x < block.x1; ++x) {
    for (unsigned y = block.y0; y < block.y1; ++y) {
      auto& p = image(x, y);
      for (unsigned i = 0; i < 4; ++i) {
        as[i] += p.rgba[i];
      }
    }
  }
  Pixel p;
  for (unsigned i = 0; i < 4; ++i) {
    p.rgba[i] = (as[i] + block.Size() / 2) / block.Size();
  }
  return p;
}
