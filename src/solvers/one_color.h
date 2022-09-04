#pragma once

#include "problem.h"
#include "solution.h"
#include "solvers/base.h"

#include <memory>
#include <string>

namespace src_solvers {
class OneColor : public Base {
 public:
  using TBase = Base;

 public:
  OneColor(unsigned);

  PSolver Clone() const override { return std::make_shared<OneColor>(*this); }

  bool SkipSolutionRead() const override { return true; }

  std::string Name() const override { return "one_color"; }

  Solution Solve(const Problem& p) override;
};
}  // namespace src_solvers
