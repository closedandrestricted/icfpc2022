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

double Cost(const Canvas& canvas, const Move& move) {
  auto& b = canvas.Get(move.block_id1);
  return round(BaseCost(move.type) * canvas.Size() / b.Size());
}
