#include "problem.h"

#include "settings.h"

bool Problem::LoadI(const std::string& _id, const std::string& filename) {
  id = _id;
  if (!target.Load(filename)) return false;
  initial.Init(target.dx, target.dy);
  auto filename2 = filename.substr(0, filename.size() - 3) + "initial.json";
  return initial.Load(filename2);
}

std::string Problem::GetFileName(const std::string& _id) {
  return ProblemFilename(_id);
}

bool Problem::Load(const std::string& _id) {
  return LoadI(_id, GetFileName(_id));
}
