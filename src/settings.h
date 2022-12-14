#pragma once

#include <string>

namespace {
const std::string path_to_root = "../";
const unsigned last_problem = 40;
}  // namespace

std::string ProblemFilename(const std::string& problem_id);
std::string ProblemFilename(unsigned problem_id);
std::string SolutionFilename(const std::string& problem_id,
                             const std::string& solver_name);
std::string SolutionFilename(unsigned problem_id,
                             const std::string& solver_name);
