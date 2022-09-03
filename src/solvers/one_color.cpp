#include "solvers/one_color.h"

#include "block.h"
#include "optimization/color.h"

using namespace src_solvers;

Solution OneColor::Solve(const Problem& p) {
  auto& i = p.Target();
  if (p.InitialCanvas().BSize() > 1) return Solution(p.Id(), {});
  Block b{0, i.dx, 0, i.dy, ""};
  auto c = opt::Color::MinCost(opt::Color::Points(b, i));
  return Solution(p.Id(), {Move(Move::COLOR, "0", c)});
}
