#pragma once

#include "move.h"

#include "common/solvers/solution.h"

#include <string>
#include <vector>

class Solution : public solvers::Solution {
 protected:
  std::vector<Move> moves;

 public:
  Solution() {}
  Solution(const std::string& _id, const std::vector<Move>& _moves)
      : moves(_moves) {
    id = _id;
  }

  bool Empty() const { return moves.empty(); }

  const std::vector<Move>& Moves() const { return moves; }

 protected:
  bool LoadI(const std::string& _id, const std::string& filename);
  void SaveI(const std::string& filename) const;

 public:
  static std::string GetFileName(const std::string& _id,
                                 const std::string& solver_name);

  bool Load(const std::string& _id, const std::string& solver_name);
  void Save(const std::string& solver_name);
};
