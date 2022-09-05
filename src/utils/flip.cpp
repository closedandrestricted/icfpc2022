#include "utils/flip.h"

#include <algorithm>

Block FlipXY(const Block& b) { return {b.y0, b.y1, b.x0, b.x1, b.id}; }

Image FlipXY(const Image& image) {
  Image i2;
  i2.Init(image.dy, image.dx);
  for (unsigned x = 0; x < image.dx; ++x) {
    for (unsigned y = 0; y < image.dy; ++y) i2(y, x) = image(x, y);
  }
  return i2;
}

Move FlipXY(const Move& m) {
  Move m2(m);
  std::swap(m2.x, m2.y);
  return m2;
}
