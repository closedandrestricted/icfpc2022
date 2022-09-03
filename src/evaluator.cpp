#include "evaluator.h"

#include "canvas.h"
#include "cost.h"
#include "problem.h"
#include "solution.h"

int64_t Evaluator::Result::DScore() const {
  return correct ? score : (1ll << 31);
}

Evaluator::Result Evaluator::Apply(const Problem &p,
                                   const std::vector<Move> &moves) {
  Canvas c(p.InitialCanvas());
  for (const auto &m : moves) {
    c.Apply(m);
  }
  return Result(true, int64_t(c.isl_cost),
                int64_t(Similarity(p.Target(), c.image)));
}

Evaluator::Result Evaluator::Apply(const Problem &p, const Solution &s) {
  return Apply(p, s.Moves());
}
