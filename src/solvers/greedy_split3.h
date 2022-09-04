#pragma once

#include "block.h"
#include "canvas.h"
#include "image.h"
#include "problem.h"
#include "solution.h"
#include "solvers/base.h"

#include <memory>
#include <string>
#include <vector>

namespace src_solvers {
class GreedySplit3 : public Base {
 public:
  using TBase = Base;

 public:
  GreedySplit3(unsigned) {}

  PSolver Clone() const override {
    return std::make_shared<GreedySplit3>(*this);
  }

  bool SkipSolutionRead() const override { return true; }

  std::string Name() const override { return "greedy_split3"; }

  std::vector<Move> SolveI(const Image& target, const Image& current,
                           const std::vector<Block>& current_blocks);

  std::vector<Move> SolveI(const Image& target, const Canvas& canvas);

  Solution Solve(const Problem& p) override;
};
}  // namespace src_solvers
