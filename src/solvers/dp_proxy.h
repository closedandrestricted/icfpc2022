#pragma once

#include "problem.h"
#include "solution.h"
#include "solvers/base.h"

#include <memory>
#include <string>

namespace src_solvers {
class DPProxy : public Base {
 public:
  using TBase = Base;
  unsigned max_xy;

 public:
  DPProxy(unsigned _max_xy) : max_xy(_max_xy) {}

  PSolver Clone() const override { return std::make_shared<DPProxy>(*this); }

  bool SkipSolutionRead() const override { return true; }

  std::string Name() const override { return "dpp_" + std::to_string(max_xy); }

  static std::vector<Move> MergeBlocks(const Canvas& canvas);

  std::vector<Move> SolveI(const Image& target, const Canvas& canvas);

  Solution Solve(const Problem& p) override;
};
}  // namespace src_solvers
