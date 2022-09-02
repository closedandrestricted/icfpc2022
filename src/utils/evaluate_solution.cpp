#include "utils/evaluate_solution.h"

#include "evaluator.h"
#include "problem.h"
#include "settings.h"
#include "solution.h"

#include "common/solvers/ext/evaluate.h"

#include <iostream>

void EvaluateSolution(const std::string& solver_name) {
  double total = 0;
  for (unsigned i = 1; i <= last_problem; ++i) {
    auto r = solvers::ext::Evaluate<Evaluator, Problem, Solution>(
        std::to_string(i), solver_name);
    total += r.DScore();
    std::cout << "Problem " << std::to_string(1000 + i).substr(1) << "\t"
              << r.correct << "\t" << r.score << "\t" << r.DScore()
              << std::endl;
  }
  std::cout << "Total = " << unsigned(total) << std::endl;
}
