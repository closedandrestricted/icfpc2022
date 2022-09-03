#pragma once

#include "block.h"
#include "problem.h"
#include "solution.h"
#include "solvers/base.h"

#include <memory>
#include <string>

// Please don't use, it's not working.
namespace src_solvers {
class InvertedGreedy : public Base {
 public:
  using TBase = Base;

 protected:
  class SBlock {
   public:
    unsigned x0, x1, y0, y1;
    unsigned id;
    double sim_cost;
    Pixel color;

    unsigned Size() const { return (x1 - x0) * (y1 - y0); }
  };

 protected:
  static double SimCost(const Image& target, const Block& b, const Pixel& c);
  static double Score(const Image& target, const SBlock& b0, const SBlock& b1);

 public:
  InvertedGreedy(unsigned) {}

  PSolver Clone() const override {
    return std::make_shared<InvertedGreedy>(*this);
  }

  bool SkipSolutionRead() const override { return true; }

  std::string Name() const override { return "inv_greedy"; }

  Solution Solve(const Problem& p) override;
};
}  // namespace src_solvers
