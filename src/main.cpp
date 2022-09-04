#include "adjusters/simple.h"
#include "settings.h"
#include "solvers/dp_opt.h"
#include "solvers/dp_proxy.h"
#include "solvers/greedy_split.h"
#include "solvers/greedy_split2.h"
#include "solvers/greedy_split3.h"
#include "solvers/greedy_swap.h"
#include "solvers/one_color.h"
#include "utils/check_with_adjuster.h"
#include "utils/evaluate_solution.h"

#include "common/files/command_line.h"
#include "common/solvers/ext/run_n.h"

#include <memory>

void InitCommaneLine(files::CommandLine& cmd) {
  cmd.AddArg("mode", "eval");
  cmd.AddArg("solution", "best");
  cmd.AddArg("solver", "one_color");
  cmd.AddArg("solver2", "one_color");
  cmd.AddArg("timelimit", 125);
  cmd.AddArg("nthreads", 4);
  cmd.AddArg("first_problem", 1);
  cmd.AddArg("last_problem", last_problem);
}

src_solvers::Base::PSolver CreateSolver(const files::CommandLine& cmd,
                                        const std::string& solver_name) {
  auto timelimit = cmd.GetInt("timelimit");
  if (solver_name == "one_color") {
    return std::make_shared<src_solvers::OneColor>(timelimit);
  } else if (solver_name == "greedy_split") {
    return std::make_shared<src_solvers::GreedySplit>(timelimit);
  } else if (solver_name == "greedy_split2") {
    return std::make_shared<src_solvers::GreedySplit2>(timelimit);
  } else if (solver_name == "greedy_split3") {
    return std::make_shared<src_solvers::GreedySplit3>(timelimit);
  } else if (solver_name == "dp") {
    return std::make_shared<src_solvers::DPOpt>(timelimit);
  } else if (solver_name == "dpp") {
    return std::make_shared<src_solvers::DPProxy>(timelimit);
  } else if (solver_name == "greedy_swap") {
    return std::make_shared<src_solvers::GreedySwap>(
        timelimit, CreateSolver(cmd, cmd.GetString("solver2")));
  } else {
    std::cerr << "Unknown solver type: " << solver_name << std::endl;
    exit(-1);
  }
}

int main(int argc, char** argv) {
  files::CommandLine cmd;
  InitCommaneLine(cmd);
  cmd.Parse(argc, argv);

  const auto mode = cmd.GetString("mode");
  if (mode == "eval") {
    EvaluateSolution(cmd.GetString("solution"));
  } else if (mode == "adjust") {
    CheckWithAdjuster<adj::Simple>(cmd.GetString("solution"));
  } else if (mode == "update") {
    UpdateBest(cmd.GetString("solution"));
  } else if (mode == "run") {
    auto solver_name = cmd.GetString("solver");
    auto s = CreateSolver(cmd, solver_name);
    int nthreads = cmd.GetInt("nthreads");
    if (nthreads <= 0)
      solvers::ext::RunN<src_solvers::Base>(*s, cmd.GetInt("first_problem"),
                                            cmd.GetInt("last_problem"));
    else
      solvers::ext::RunNMT<src_solvers::Base>(*s, cmd.GetInt("first_problem"),
                                              cmd.GetInt("last_problem"),
                                              nthreads);
  } else {
    std::cerr << "Unknown mode " << mode << std::endl;
  }

  return 0;
}
