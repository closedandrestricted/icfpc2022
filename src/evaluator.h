#pragma once

#include "move.h"
#include "problem.h"
#include "solution.h"

#include "common/base.h"
#include "common/solvers/evaluator.h"

#include <cmath>
#include <string>
#include <vector>

class Problem;
class Solution;

class Evaluator : public solvers::Evaluator {
 public:
  class Result : public solvers::Evaluator::Result {
   public:
    using TBase = solvers::Evaluator::Result;

    double cost_isl;
    double cost_sim;

   public:
    Result() {}
    Result(bool _correct, int64_t _score) : TBase(_correct, _score) {}
    Result(bool _correct, double isl, double sim)
        : TBase(_correct, round(isl) + round(sim)),
          cost_isl(isl),
          cost_sim(sim) {}

    int64_t DScore() const;
    double FScore() const;
  };

  static Result Apply(const Problem &p, const std::vector<Move> &moves);
  static Result Apply(const Problem &p, const Solution &s);
};
