#include "solvers/one_color.h"

#include "block.h"
#include "cost.h"
#include "optimization/color.h"

#include <vector>

using namespace src_solvers;

Solution OneColor::Solve(const Problem& p) {
  auto& i = p.Target();
  auto& c = p.InitialCanvas();
  std::vector<Move> s;
  for (auto& it : c.blocks) {
    auto& b = it.second;
    auto cur_cost = Similarity(i, c.image, b);
    auto vp = opt::Color::Points(b, i);
    auto bc = opt::Color::MinCost(vp);
    auto new_sim_cost = opt::Color::Cost(vp, bc);
    auto new_isl_cost = BaseCost(Move::COLOR) * i.Size() / b.Size();
    if (cur_cost > new_isl_cost + new_sim_cost)
      s.push_back(Move(Move::COLOR, b.id, bc));
  }
  return Solution(p.Id(), s);
}
