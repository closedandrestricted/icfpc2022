#pragma once

#include "common/solvers/evaluator.h"

#include <string>

class Problem;
class Solution;

class Evaluator : public solvers::Evaluator {
 public:
  class Result : public solvers::Evaluator::Result {
   public:
    using TBase = solvers::Evaluator::Result;

    int64_t cost_isl;
    int64_t cost_sim;

   public:
    Result() {}
    Result(bool _correct, int64_t _score) : TBase(_correct, _score) {}
    Result(bool _correct, int64_t isl, int64_t sim)
        : TBase(_correct, isl + sim), cost_isl(isl), cost_sim(sim) {}

    int64_t DScore() const;
  };

  static Result Apply(const Problem &p, const Solution &s);
};
