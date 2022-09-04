#include "problem.h"

#include "settings.h"

bool Problem::LoadI(const std::string& _id, const std::string& filename) {
  id = _id;
  if (!target.LoadPNG(filename)) return false;
  int pid = stoi(id);
  if (pid <= 25) {
    initial.Init(target.dx, target.dy);
  } else if (pid <= 35) {
    auto filename2 = filename.substr(0, filename.size() - 3) + "initial.json";
    return initial.LoadJSON(filename2);
  } else {
    auto filename2 = filename.substr(0, filename.size() - 3) + "source.png";
    return initial.LoadPNG(filename2);
  }
  return true;
}

std::string Problem::GetFileName(const std::string& _id) {
  return ProblemFilename(_id);
}

bool Problem::Load(const std::string& _id) {
  return LoadI(_id, GetFileName(_id));
}
