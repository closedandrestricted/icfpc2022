#include "solvers/dp_proxy.h"

#include "block.h"
#include "cost.h"
#include "evaluator.h"
#include "solvers/dp_opt.h"

#include "common/assert_exception.h"
#include "common/vector/make.h"
#include "common/vector/unique.h"

#include <algorithm>
#include <string>
#include <vector>

using namespace src_solvers;

std::vector<Move> DPProxy::MergeBlocks(const Canvas& canvas) {
  unsigned pid = canvas.pid;
  auto& image = canvas.GetImage();
  auto initial_blocks = canvas.GetBlocks();
  if (initial_blocks.size() == 1) return {};

  std::vector<unsigned> vhsx, vhsy;
  for (auto& b : initial_blocks) {
    vhsx.push_back(b.x0);
    vhsx.push_back(b.x1);
    vhsy.push_back(b.y0);
    vhsy.push_back(b.y1);
  }
  nvector::UniqueUnsorted(vhsx);
  nvector::UniqueUnsorted(vhsy);

  unsigned nx = vhsx.size() - 1, ny = vhsy.size() - 1;
  Assert(initial_blocks.size() == nx * ny);
  Assert(nx == ny);
  unsigned dxy = image.dx / nx;
  for (unsigned i = 0; i < nx; ++i) {
    Assert(vhsx[i + 1] - vhsx[i] == dxy);
    Assert(vhsy[i + 1] - vhsy[i] == dxy);
  }

  double base_score = 0.0;
  for (unsigned i = 1; i < nx; ++i)
    base_score += ny * Cost(pid, Move::MERGE, image.Size(), dxy * dxy * i);
  for (unsigned i = 1; i < ny; ++i)
    base_score += Cost(pid, Move::MERGE, image.Size(), image.dx * dxy * i);

  unsigned best_l = 0;
  double best_score = base_score;
  for (unsigned l = 1; l < nx; ++l) {
    double score = 0;
    for (unsigned i = 1; i < nx; ++i)
      score += l * Cost(pid, Move::MERGE, image.Size(), dxy * dxy * i);
    for (unsigned i = 1; i < l; ++i)
      score += Cost(pid, Move::MERGE, image.Size(), image.dx * i);
    for (unsigned i = 2; i <= nx; ++i)
      score += Cost(pid, Move::LINE_CUT, image.Size(), dxy * dxy * i * l);
    for (unsigned i = l; i < ny; ++i)
      score += nx * Cost(pid, Move::MERGE, image.Size(), dxy * dxy * i);
    for (unsigned i = 1; i < nx; ++i)
      score += Cost(pid, Move::MERGE, image.Size(), image.dy * dxy * i);
    if (best_score < score) {
      best_score = score;
      best_l = l;
    }
  }
  Assert(best_l > 0);

  auto vvid = nvector::Make<std::string>(nx, ny, "");
  for (auto& b : initial_blocks) {
    vvid[b.x0 / dxy][b.y0 / dxy] = b.id;
  }

  unsigned bgindex = canvas.index;
  std::vector<Move> moves;
  for (unsigned i = 0; i < best_l; ++i) {
    for (unsigned j = 1; j < nx; ++j) {
      moves.push_back(Move(Move::MERGE, vvid[0][i], vvid[j][i]));
      vvid[0][i] = std::to_string(++bgindex);
    }
  }
  for (unsigned i = 1; i < best_l; ++i) {
    moves.push_back(Move(Move::MERGE, vvid[0][0], vvid[0][i]));
    vvid[0][0] = std::to_string(++bgindex);
  }
  for (unsigned i = nx - 1; i > 0; --i) {
    moves.push_back(Move(Move::LINE_CUT, vvid[0][0], i * dxy, 0));
    vvid[i][0] = vvid[0][0] + ".1";
    vvid[0][0] = vvid[0][0] + ".0";
  }
  for (unsigned i = 0; i < nx; ++i) {
    for (unsigned j = best_l; j < ny; ++j) {
      moves.push_back(Move(Move::MERGE, vvid[i][0], vvid[i][j]));
      vvid[i][0] = std::to_string(++bgindex);
    }
  }
  for (unsigned i = 1; i < nx; ++i) {
    moves.push_back(Move(Move::MERGE, vvid[0][0], vvid[i][0]));
    vvid[0][0] = std::to_string(++bgindex);
  }

  return moves;
}

std::vector<Move> DPProxy::SolveI(const Image& target,
                                  const Canvas& initial_canvas) {
  Canvas canvas = initial_canvas;
  auto moves = MergeBlocks(canvas);
  for (auto& m : moves) canvas.Apply(m);
  DPOpt dp(max_xy);
  auto s2 = dp.SolveI(target, canvas);
  moves.insert(moves.end(), s2.begin(), s2.end());
  return moves;
}

Solution DPProxy::Solve(const Problem& p) {
  auto moves = SolveI(p.Target(), p.InitialCanvas());
  auto final_score = Evaluator::Apply(p, moves).FScore();
  std::cout << "Final cost = " << final_score << std::endl;
  std::cout << "Done with " << p.Id() << std::endl;
  return Solution(p.Id(), moves);
}
