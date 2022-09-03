#pragma once

#include "evaluator.h"
#include "problem.h"
#include "settings.h"
#include "solution.h"

#include <iostream>
#include <string>

// Check solution from cache with adjuster.
template <class TAdjuster>
void CheckWithAdjuster(const std::string& solver_name) {
  TAdjuster adj;
  for (unsigned i = 1; i <= last_problem; ++i) {
    Problem p;
    if (!p.Load(std::to_string(i))) continue;
    Solution s;
    if (!s.Load(std::to_string(i), solver_name)) continue;
    auto snew = adj.Check(p, s);
    if (snew != s) {
      auto score_old = Evaluator::Apply(p, s).DScore();
      auto score_new = Evaluator::Apply(p, snew).DScore();
      std::cout << "New solution from adjuster: " << score_old << "\t"
                << score_new << std::endl;
      if (score_new < score_old) {
        snew.Save(solver_name);
      } else {
        assert(false);
      }
    }
  }
}
