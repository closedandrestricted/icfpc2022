#pragma once

#include "evaluator.h"
#include "problem.h"
#include "settings.h"
#include "solution.h"
#include "common/thread_pool.h"

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
      auto score_old = Evaluator::Apply(p, s).FScore();
      auto score_new = Evaluator::Apply(p, snew).FScore();
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

template <class TAdjuster>
void CheckWithAdjusterMT(const std::string& solver_name, unsigned nthreads) {
  ThreadPool tp(nthreads);
  for (unsigned i = 1; i <= last_problem; ++i) {
    tp.EnqueueTask(std::make_shared<std::packaged_task<void()>>([&, i] {
      TAdjuster adj;
      Problem p;
      if (!p.Load(std::to_string(i))) return;
      Solution s;
      if (!s.Load(std::to_string(i), solver_name)) return;
      auto snew = adj.Check(p, s);
      if (snew != s) {
        auto score_old = Evaluator::Apply(p, s).FScore();
        auto score_new = Evaluator::Apply(p, snew).FScore();
        std::cout << "New solution from adjuster: " << score_old << "\t"
                  << score_new << std::endl;
        if (score_new < score_old) {
          snew.Save(solver_name);
        } else {
          assert(false);
        }
      }
    }));
  }
}
