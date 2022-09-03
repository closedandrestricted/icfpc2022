#pragma once

#include "problem.h"
#include "solution.h"
#include "solvers/base.h"

#include <memory>
#include <string>

namespace src_solvers {
class GreedySplit2 : public Base {
 public:
  using TBase = Base;

 public:
  GreedySplit2(unsigned) {}

  PSolver Clone() const override {
    return std::make_shared<GreedySplit2>(*this);
  }

  bool SkipSolutionRead() const override { return true; }

  std::string Name() const override { return "greedy_split2"; }

  Solution Solve(const Problem& p) override;
};
}  // namespace src_solvers
