#include "cost.h"

#include "common/base.h"

#include <algorithm>
#include <cmath>

double BaseCost(Move::Type type) {
  switch (type) {
    case Move::SKIP:
      return 0;
    case Move::LINE_CUT:
      return 7;
    case Move::POINT_CUT:
      return 10;
    case Move::COLOR:
      return 5;
    case Move::SWAP:
      return 3;
    case Move::MERGE:
      return 1;
    default:
      assert(false);
      return 0;
  }
}

double Cost(const Canvas &canvas, const Move &move) {
  if (move.type == Move::SKIP) return 0.0;
  auto &b = canvas.Get(move.block_id1);
  if (move.type == Move::MERGE) {
    auto &b2 = canvas.Get(move.block_id2);
    return round(BaseCost(move.type) * canvas.Size() /
                 (std::max(b.Size(), b2.Size())));
  } else {
    return round(BaseCost(move.type) * canvas.Size() / b.Size());
  }
}

double Similarity(const Image &i1, const Image &i2) {
  assert((i1.dx == i2.dx) && (i1.dy == i2.dy));
  double s = 0;
  for (unsigned i = 0; i < i1.dx * i1.dy; ++i)
    s += Distance(i1.pixels[i], i2.pixels[i]);
  return round(s / 200.0);
}

double Similarity(const Image &i1, const Image &i2, const Block &b) {
  double s = 0;
  for (unsigned x = b.x0; x < b.x1; ++x) {
    for (unsigned y = b.y0; y < b.y1; ++y) s += Distance(i1(x, y), i2(x, y));
  }
  return s / 200.0;
}
