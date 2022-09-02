#include "evaluator.h"
#include "pixel.h"
#include "problem.h"
#include "solution.h"

#include <iostream>

int main() {
  std::cout << "Test" << std::endl;
  Problem p;
  p.Load("1");
  Pixel pxl{255, 255, 255, 255};
  Solution s("1", std::vector<Move>{Move(Move::COLOR, "0", pxl)});
  auto r = Evaluator::Apply(p, s);
  std::cout << r.correct << "\t" << r.score << std::endl;
  s.Save("test");
  return 0;
}
