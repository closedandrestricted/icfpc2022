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

   public:
    Result() {}
    Result(bool _correct, int64_t _score) : TBase(_correct, _score) {}

    int64_t DScore() const;
  };

  static Result Apply(const Problem &p, const Solution &s);
};
