#pragma once

#include "common/base.h"

#include <string>

namespace solvers {
namespace ext {
template <class TSolver>
inline void RunOne(TSolver& solver, const std::string& problem_id) {
  using TProblem = typename TSolver::TProblem;
  using TSolution = typename TSolver::TSolution;
  using TEvaluator = typename TSolver::TEvaluator;
  TProblem p;
  if (!p.Load(problem_id)) {
    assert(false);
    return;
  }
  auto solver_name = solver.Name();
  TSolution s;
  if (!solver.SkipSolutionRead()) {
    s.Load(problem_id, solver_name);
  }
  bool new_solution = false;
  if (s.Empty()) {
    s = solver.Solve(p);
    new_solution = true;
  }
  auto r = TEvaluator::Apply(p, s);
  if (!r.correct) return;
  if (new_solution && !solver.SkipSolutionWrite()) {
    TSolution scache;
    bool ok = scache.Load(problem_id, solver_name);
    if (ok) {
      auto rcache = TEvaluator::Apply(p, scache);
      if (TEvaluator::Compare(r, rcache)) {
        std::cout << "New solution for problem: " << problem_id << "\t["
                  << rcache.DScore() << " -> " << r.DScore() << "]"
                  << std::endl;
        s.Save(solver_name);
      }
    } else {
      std::cout << "Solution in " << solver_name << " is unreadable!"
                << std::endl;
    }
  }
  if (!solver.SkipBest()) {
    TSolution sbest;
    bool ok = sbest.Load(problem_id, "best");
    if (ok) {
      auto rbest = TEvaluator::Apply(p, sbest);
      if (TEvaluator::Compare(r, rbest)) {
        std::cout << "New best solution for problem: " << problem_id << "\t["
                  << rbest.DScore() << " -> " << r.DScore() << "]" << std::endl;
        s.Save("best");
      }
    } else {
      std::cout << "Solution in best is unreadable!" << std::endl;
    }
  }
}
}  // namespace ext
}  // namespace solvers
