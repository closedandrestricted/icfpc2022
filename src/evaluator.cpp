#include "evaluator.h"

#include "canvas.h"
#include "cost.h"
#include "problem.h"
#include "solution.h"

int64_t Evaluator::Result::DScore() const {
  return correct ? score : (1ll << 31);
}

double Evaluator::Result::FScore() const {
  return correct ? cost_isl + cost_sim : double(1ll << 31);
}

Evaluator::Result Evaluator::Apply(const Problem &p,
                                   const std::vector<Move> &moves) {
  Canvas c(p.InitialCanvas());
  for (const auto &m : moves) {
    c.Apply(m);
  }
  return Result(true, c.isl_cost, Similarity(p.Target(), c.image));
}

Evaluator::Result Evaluator::Apply(const Problem &p, const Solution &s) {
  return Apply(p, s.Moves());
}
