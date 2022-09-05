#include "solvers/dp_opt2f.h"

#include "dp_proxy.h"
#include "evaluator.h"
#include "solvers/dp_opt2.h"
#include "utils/flip.h"

#include <algorithm>
#include <vector>

using namespace src_solvers;

std::vector<Move> DPOpt2F::SolveI(const Image& target,
                                  const Canvas& original_canvas) {
  Canvas canvas = original_canvas;
  auto sbase = DPProxy::MergeBlocks(canvas);
  for (auto& m : sbase) canvas.Apply(m);
  unsigned nxy = unsigned(sqrt(double(max_xy)));
  auto s = DPOpt2::SolveIY(FlipXY(target), canvas, nxy, nxy);
  for (auto& m : s) sbase.push_back(FlipXY(m));
  return sbase;
}

Solution DPOpt2F::Solve(const Problem& p) {
  std::vector<Move> s = SolveI(p.Target(), p.InitialCanvas());
  auto final_score = Evaluator::Apply(p, s).FScore();
  std::cout << "Final cost = " << final_score << std::endl;
  std::cout << "Done with " << p.Id() << std::endl;
  return Solution(p.Id(), s);
}
