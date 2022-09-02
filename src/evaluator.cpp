#include "evaluator.h"

#include "canvas.h"
#include "cost.h"
#include "problem.h"
#include "solution.h"

int64_t Evaluator::Result::DScore() const {
  return correct ? score : (1ll << 31);
}

Evaluator::Result Evaluator::Apply(const Problem &p, const Solution &s) {
  Canvas c(p.Target().dx, p.Target().dy);
  for (const auto &m : s.Moves()) {
    c.Apply(m);
  }
  double isl_cost = c.isl_cost;
  double sim_cost = Similarity(p.Target(), c.image);
  return Result(true, int64_t(isl_cost + sim_cost));
}
