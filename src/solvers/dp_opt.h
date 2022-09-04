#pragma once

#include "problem.h"
#include "solution.h"
#include "solvers/base.h"

#include <memory>
#include <string>

namespace src_solvers {
class DPOpt : public Base {
 public:
  using TBase = Base;
  unsigned max_xy;

 public:
  DPOpt(unsigned _max_xy) : max_xy(_max_xy) {}

  PSolver Clone() const override { return std::make_shared<DPOpt>(*this); }

  bool SkipSolutionRead() const override { return true; }

  std::string Name() const override { return "dp_" + std::to_string(max_xy); }

  Solution Solve(const Problem& p) override;
};
}  // namespace src_solvers
