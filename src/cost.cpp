#include "cost.h"

#include "common/base.h"

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
  auto &b = canvas.Get(move.block_id1);
  if (move.type == Move::MERGE) {
    auto &b2 = canvas.Get(move.block_id2);
    return round(BaseCost(move.type) * canvas.Size() / (b.Size() + b2.Size()));
  } else {
    return round(BaseCost(move.type) * canvas.Size() / b.Size());
  }
}

double Similarity(const Image &i1, const Image &i2) {
  assert((i1.dx == i2.dx) && (i1.dy == i2.dy));
  double s = 0;
  for (unsigned i = 0; i < i1.dx * i1.dy; ++i)
    s += Distance(i1.pixels[i], i2.pixels[i]);
  return s / 200.0;
}
