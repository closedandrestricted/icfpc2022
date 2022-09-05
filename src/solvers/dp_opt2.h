#pragma once

#include "problem.h"
#include "solution.h"
#include "solvers/base.h"

#include <memory>
#include <string>
#include <vector>

namespace src_solvers {
class DPOpt2 : public Base {
 public:
  using TBase = Base;
  unsigned max_xy;

 public:
  static std::vector<Move> SolveIY(const Image& target, const Canvas& canvas,
                                   unsigned nx, unsigned ny);

 public:
  DPOpt2(unsigned _max_xy) : max_xy(_max_xy) {}

  PSolver Clone() const override { return std::make_shared<DPOpt2>(*this); }

  bool SkipSolutionRead() const override { return true; }

  std::string Name() const override { return "dp2_" + std::to_string(max_xy); }

  std::vector<Move> SolveI(const Image& target, const Canvas& canvas);

  Solution Solve(const Problem& p) override;
};
}  // namespace src_solvers
