#pragma once

#include "problem.h"
#include "solution.h"
#include "solvers/base.h"

#include <memory>
#include <string>
#include <vector>

namespace src_solvers {
class DPOpt2F : public Base {
 public:
  using TBase = Base;
  unsigned max_xy;

 public:
  DPOpt2F(unsigned _max_xy) : max_xy(_max_xy) {}

  PSolver Clone() const override { return std::make_shared<DPOpt2F>(*this); }

  bool SkipSolutionRead() const override { return true; }

  std::string Name() const override { return "dp2f_" + std::to_string(max_xy); }

  std::vector<Move> SolveI(const Image& target, const Canvas& canvas);

  Solution Solve(const Problem& p) override;
};
}  // namespace src_solvers
