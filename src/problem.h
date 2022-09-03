#pragma once

#include "canvas.h"
#include "image.h"

#include "common/base.h"
#include "common/solvers/problem.h"

#include <string>

class Problem : public solvers::Problem {
 protected:
  Image target;
  Canvas initial;

 protected:
  bool LoadI(const std::string& _id, const std::string& filename);

 public:
  const Image& Target() const { return target; }
  const Canvas& InitialCanvas() const { return initial; }

  static std::string GetFileName(const std::string& _id);
  bool Load(const std::string& _id);
};
