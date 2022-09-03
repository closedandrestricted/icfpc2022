#pragma once

#include "problem.h"
#include "solution.h"
#include "solvers/base.h"

#include <memory>
#include <string>

namespace src_solvers {
class GreedySplit : public Base {
 public:
  using TBase = Base;

 public:
  GreedySplit(unsigned) {}

  PSolver Clone() const override {
    return std::make_shared<GreedySplit>(*this);
  }

  // bool SkipSolutionRead() const override { return true; }

  std::string Name() const override { return "greedy_split"; }

  Solution Solve(const Problem& p) override;
};
}  // namespace src_solvers
