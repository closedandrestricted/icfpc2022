#include "problem.h"

#include "settings.h"

bool Problem::LoadI(const std::string& _id, const std::string& filename) {
  id = _id;
  return target.Load(filename);
}

std::string Problem::GetFileName(const std::string& _id) {
  return ProblemFilename(_id);
}

bool Problem::Load(const std::string& _id) {
  return LoadI(_id, GetFileName(_id));
}
