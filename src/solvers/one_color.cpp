#include "solvers/one_color.h"

#include "block.h"
#include "utils/color_for_block.h"

using namespace src_solvers;

Solution OneColor::Solve(const Problem& p) {
  auto& i = p.Target();
  Block b{0, i.dx, 0, i.dy, ""};
  auto c = ColorMSE(b, i);
  return Solution(p.Id(), {Move(Move::COLOR, "0", c)});
}
