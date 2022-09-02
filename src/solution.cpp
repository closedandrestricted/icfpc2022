#include "solution.h"

#include "settings.h"

#include <fstream>
#include <iostream>

bool Solution::LoadI(const std::string& _id, const std::string& filename) {
  id = _id;
  moves.clear();
  std::ifstream f(filename);
  std::string s;
  bool ok = true;
  while (ok && std::getline(f, s)) {
    moves.push_back({});
    ok = moves.back().Decode(s);
  }
  return ok;
}

void Solution::SaveI(const std::string& filename) const {
  std::ofstream f(filename);
  for (auto m : Moves()) {
    f << m.Encode() << std::endl;
  }
}

std::string Solution::GetFileName(const std::string& _id,
                                  const std::string& solver_name) {
  return SolutionFilename(_id, solver_name);
}

bool Solution::Load(const std::string& _id, const std::string& solver_name) {
  return LoadI(_id, GetFileName(_id, solver_name));
}

void Solution::Save(const std::string& solver_name) {
  SaveI(GetFileName(id, solver_name));
}
