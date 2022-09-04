#include "adjusters/simple.h"

#include "evaluator.h"
#include "move.h"

#include "common/assert_exception.h"

#include <iostream>
#include <vector>

using namespace adj;

namespace {
template <class TValue>
bool CheckValue(TValue& value, int min_value, int max_value, double& best_score,
                const Problem& p, std::vector<Move>& moves) {
  bool bfound = false;
  int ivalue = int(value);
  for (int dv : {-1, 1}) {
    for (bool b = true; b;) {
      b = false;
      ivalue += dv;
      if ((ivalue < min_value) || (ivalue > max_value)) break;
      try {
        value = TValue(ivalue);
        auto score = Evaluator::Apply(p, moves).FScore();
        if (score < best_score) {
          best_score = score;
          bfound = b = true;
        }
      } catch (AssertException) {
      }
    }
    ivalue -= dv;
    value = TValue(ivalue);
  }
  return bfound;
}
}  // namespace

Solution Simple::Check(const Problem& p, const Solution& s) {
  auto best_score = Evaluator::Apply(p, s).FScore();
  auto new_s = s.Moves();
  int minx = 0, maxx = p.Target().dx, miny = 0, maxy = p.Target().dy;
  for (bool bfound = true; bfound;) {
    bfound = false;
    for (unsigned i = 0; i < new_s.size(); ++i) {
      auto& m = new_s[i];
      auto type = m.type;
      if (type == Move::LINE_CUT) {
        if (m.x)
          bfound = CheckValue(m.x, 1, maxx, best_score, p, new_s) || bfound;
        if (m.y)
          bfound = CheckValue(m.y, 1, maxy, best_score, p, new_s) || bfound;
      } else if (type == Move::POINT_CUT) {
        bfound = CheckValue(m.x, minx, maxx, best_score, p, new_s) || bfound;
        bfound = CheckValue(m.y, miny, maxy, best_score, p, new_s) || bfound;
      } else if (type == Move::COLOR) {
        for (unsigned j = 0; j < 4; ++j)
          bfound = CheckValue(m.color.rgba[j], 0, 255, best_score, p, new_s) ||
                   bfound;
      }
      try {
        m.type = Move::SKIP;
        auto score = Evaluator::Apply(p, new_s).FScore();
        if (score < best_score) {
          best_score = score;
          bfound = true;
          continue;
        }
      } catch (AssertException) {
      }
      m.type = type;
    }
  }
  std::cout << "Done with problem " << s.GetId() << std::endl;
  return Solution(s.GetId(), new_s);
  // return s;
}
