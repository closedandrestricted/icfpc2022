#include "solvers/greedy_split3.h"

#include "block.h"
#include "cost.h"
#include "optimization/color.h"
#include "solvers/greedy_split.h"

#include "common/assert_exception.h"
#include "common/vector/make.h"

#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

using namespace src_solvers;

GreedySplit3::GreedySplit3(unsigned) {}

std::vector<Move> GreedySplit3::SolveI(
    const Image& target, const Image& current,
    const std::vector<Block>& current_blocks) {
  // Precalc data
  auto vvv = nvector::Make<double>(target.dx + 1, target.dy + 1, 8, 0.);
  for (unsigned x = 1; x <= target.dx; ++x) {
    for (unsigned y = 1; y <= target.dy; ++y) {
      for (unsigned i = 0; i < 4; ++i) {
        double v = target(x, y).rgba[i];
        for (unsigned j = 2 * i; j < 2 * i + 2; ++j) {
          vvv[x][y][j] =
              vvv[x - 1][y][j] + vvv[x][y - 1][j] - vvv[x - 1][y - 1][j] + v;
          v *= v;
        }
      }
    }
  }

  auto Score = [&](const Block& b) {
    if (b.Size() == 0) return 0.;
    std::vector<double> v(8, 0.0);
    for (unsigned i = 0; i < 8; ++i) {
      v[i] = vvv[b.x1][b.y1][i] - vvv[b.x0][b.y1][i] - vvv[b.x1][b.y0][i] +
             vvv[b.x0][b.y0][i];
    }
    double s = 0;
    for (unsigned i = 0; i < 8; i += 2) {
      double s0 = b.Size(), s1 = v[i], s2 = v[i + 1];
      s += s2 - s1 * s1 / s0;
    }
    Assert(s > -1e-6);
    return s;
  };

  const double C = 100.;
  std::queue<Block> qb;
  std::vector<Block> vbd;
  std::vector<Move> s;
  for (auto& b : current_blocks) qb.push(b);
  for (; !qb.empty(); qb.pop()) {
    auto b = qb.front();
    auto sim_cost = Similarity(target, current, b);
    auto vp = opt::Color::Points(b, target);
    auto color_cost = opt::Color::Cost(vp, opt::Color::Median(vp));
    if ((std::min(sim_cost, color_cost) > C * target.Size() / b.Size()) &&
        (b.Size() > 1)) {
      // Split required
      double best_score = Score(b);
      unsigned best_split_x = 0, best_split_y = 0;
      Block bb1, bb2;
      if (b.x1 - b.x0 > b.y1 - b.y0) {
        best_split_x = (b.x0 + b.x1) / 2;
        Block b1{b.x0, best_split_x, b.y0, b.y1, b.id + ".0"};
        Block b2{best_split_x, b.x1, b.y0, b.y1, b.id + ".1"};
      } else {
        best_split_y = (b.y0 + b.y1) / 2;
        Block b1{b.x0, b.x1, b.y0, best_split_y, b.id + ".0"};
        Block b2{b.x0, b.x1, best_split_y, b.y1, b.id + ".1"};
      }
      for (unsigned x = b.x0 + 1; x < b.x1; ++x) {
        Block b1{b.x0, x, b.y0, b.y1, b.id + ".0"};
        Block b2{x, b.x1, b.y0, b.y1, b.id + ".1"};
        double score = Score(b1) + Score(b2);
        if (score < best_score) {
          best_score = score;
          bb1 = b1;
          bb2 = b2;
          best_split_x = x;
          best_split_y = 0;
        }
      }
      for (unsigned y = b.y0 + 1; y < b.y1; ++y) {
        Block b1{b.x0, b.x1, b.y0, y, b.id + ".0"};
        Block b2{b.x0, b.x1, y, b.y1, b.id + ".1"};
        double score = Score(b1) + Score(b2);
        if (score < best_score) {
          best_score = score;
          bb1 = b1;
          bb2 = b2;
          best_split_x = 0;
          best_split_y = y;
        }
      }
      s.push_back(Move(Move::LINE_CUT, b.id, best_split_x, best_split_y));
      qb.push(bb1);
      qb.push(bb2);
    } else {
      vbd.push_back(b);
    }
  }
  auto s2 = GreedySplit::SolveI(target, current, vbd);
  s.insert(s.end(), s2.begin(), s2.end());
  return s;
}

std::vector<Move> GreedySplit3::SolveI(const Image& target,
                                       const Canvas& canvas) {
  return SolveI(target, canvas.GetImage(), canvas.GetBlocks());
}

Solution GreedySplit3::Solve(const Problem& p) {
  std::vector<Move> s = SolveI(p.Target(), p.InitialCanvas());
  std::cout << "Done with " << p.Id() << std::endl;
  return Solution(p.Id(), s);
}
