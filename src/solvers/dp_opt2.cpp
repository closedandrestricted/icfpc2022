#include "solvers/dp_opt2.h"

#include "block.h"
#include "cost.h"
#include "dp_proxy.h"
#include "evaluator.h"
#include "optimization/color.h"
#include "optimization/compression.h"

#include "common/assert_exception.h"
#include "common/timer.h"
#include "common/vector/enumerate.h"
#include "common/vector/make.h"
#include "common/vector/unique.h"

#include <algorithm>
#include <queue>
#include <unordered_map>
#include <vector>

using namespace src_solvers;

std::vector<Move> DPOpt2::SolveIY(const Image& target, const Canvas& canvas,
                                  unsigned nx, unsigned ny) {
  unsigned pid = canvas.pid;
  double tsize = target.Size();
  Assert(canvas.BSize() == 1);
  auto canvas_block = canvas.GetBlocks()[0];
  Assert(canvas_block.Size() == target.Size());
  opt::Compression cmpr;
  cmpr.Init(target);
  Timer t;

  std::vector<unsigned> vx;
  auto vy = cmpr.CompressY(canvas_block, ny, false);

  // Run initial DP
  t.Start();
  std::vector<std::pair<double, double>> dpx(nx);
  std::vector<std::pair<std::pair<double, double>, unsigned>> dpy(vy.size());
  dpy[0] = {{0., 0.}, 0};
  for (unsigned j1 = 1; j1 < vy.size(); ++j1) {
    double best_score_j = 1e16;
    double sim_score_j = 0.;
    unsigned best_j0 = 0;
    for (unsigned j0 = j1; j0-- > 0;) {
      vx = cmpr.CompressX(Block{0, target.dx, vy[j0], vy[j1], ""}, nx, true);
      double score_j = 0;
      dpx[0] = {0., 0.};
      dpx.back() = {1e15, 1e15};
      for (unsigned i1 = 1; i1 < vx.size(); ++i1) {
        double best_score_i = 1e16;
        double total_sim_score = 0.0;
        for (unsigned i0 = i1; i0-- > 0;) {
          Block b{vx[i0], vx[i1], vy[j0], vy[j1], ""};
          double score_sim = opt::Color::ApproxCost(b, target);
          if (best_score_i <= score_sim) break;
          double score_color = Cost(pid, Move::COLOR, tsize, vx[i1] * vy[j1]);
          double score_split_merge =
              Cost(pid, Move::LINE_CUT, target.dy, vy[j1]) +
              Cost(pid, Move::MERGE, tsize,
                   vy[j1] * std::max(vx[i1], target.dx - vx[i1]));
          auto score_i0 =
              score_sim + score_color + score_split_merge + dpx[i0].first;
          if (best_score_i > score_i0) {
            best_score_i = score_i0;
            total_sim_score = score_sim + dpx[i0].second;
          }
        }
        dpx[i1] = {best_score_i, total_sim_score};
        // if (total_sim_score > best_score_j) break;
      }
      if (dpx.back().second > best_score_j) break;
      double score_split_merge = Cost(pid, Move::LINE_CUT, tsize, tsize) +
                                 Cost(pid, Move::MERGE, target.dy,
                                      std::max(vy[j1], target.dy - vy[j1]));
      score_j = dpx.back().first + dpy[j0].first.first + score_split_merge;
      if (best_score_j > score_j) {
        best_score_j = score_j;
        sim_score_j = dpx.back().second + dpy[j0].first.second;
        best_j0 = j0;
      }
    }
    dpy[j1] = {{best_score_j, sim_score_j}, best_j0};
  }
  t.Stop();
  std::cout << "DP fininshed.\tExpected Score = " << dpy.back().first.first
            << "\tSim = " << dpy.back().first.second
            << "\tTime = " << t.GetMilliseconds() << std::endl;

  // Build solution
  // TODO:
  //   1. Check both direction
  //   3. Use left-down part for construction + swap
  t.Start();
  nx *= 3;
  std::vector<Move> s;
  unsigned cindex = canvas.index;
  std::string sindex = canvas_block.id;
  double expected_score2 = 0.;
  std::vector<std::pair<double, unsigned>> dpx2(nx);
  for (unsigned j1 = vy.size() - 1; j1 > 0;) {
    unsigned j0 = dpy[j1].second;
    vx = cmpr.CompressX(Block{0, target.dx, vy[j0], vy[j1], ""}, nx, true);
    std::string id_initial_cut_y;
    if (vy[j1] != target.dy) {
      s.push_back(Move(Move::LINE_CUT, sindex, 0, vy[j1]));
      expected_score2 += Cost(pid, Move::LINE_CUT, tsize, tsize);
      id_initial_cut_y = sindex + ".1";
      sindex = sindex + ".0";
    }

    dpx2[0] = {0., 0};
    for (unsigned i1 = 1; i1 < vx.size(); ++i1) {
      double best_score_i = 1e16;
      unsigned best_i0 = 0;
      for (unsigned i0 = i1; i0-- > 0;) {
        Block b{vx[i0], vx[i1], vy[j0], vy[j1], ""};
        double score_sim = opt::Color::ApproxCost(b, target);
        if (best_score_i <= score_sim) break;
        double score_color = Cost(pid, Move::COLOR, tsize, vx[i1] * vy[j1]);
        double score_split_merge =
            Cost(pid, Move::LINE_CUT, target.dy, vy[j1]) +
            Cost(pid, Move::MERGE, tsize,
                 vy[j1] * std::max(vx[i1], target.dx - vx[i1]));
        auto score_i0 =
            score_sim + score_color + score_split_merge + dpx2[i0].first;
        if (best_score_i > score_i0) {
          best_score_i = score_i0;
          best_i0 = i0;
        }
      }
      dpx2[i1] = {best_score_i, best_i0};
    }
    expected_score2 += dpx2.back().first;

    for (unsigned i1 = vx.size() - 1; i1 > 0;) {
      unsigned i0 = dpx2[i1].second;
      if (vx[i1] == target.dx) {
        s.push_back(Move(Move::COLOR, sindex,
                         opt::Color::Median(opt::Color::Points(
                             {vx[i0], vx[i1], vy[j0], vy[j1], ""}, target))));
      } else {
        s.push_back(Move(Move::LINE_CUT, sindex, vx[i1], 0));
        s.push_back(Move(Move::COLOR, sindex + ".0",
                         opt::Color::Median(opt::Color::Points(
                             {vx[i0], vx[i1], vy[j0], vy[j1], ""}, target))));
        s.push_back(Move(Move::MERGE, sindex + ".0", sindex + ".1"));
        sindex = std::to_string(++cindex);
      }
      i1 = i0;
    }

    if (vy[j1] != target.dy) {
      s.push_back(Move(Move::MERGE, sindex, id_initial_cut_y));
      expected_score2 += Cost(pid, Move::MERGE, target.dy,
                              std::max(vy[j1], target.dy - vy[j1]));
      sindex = std::to_string(++cindex);
    }
    j1 = j0;
  }
  t.Stop();
  std::cout << "CS fininshed.\tExpected Score = " << expected_score2
            << "\tTime = " << t.GetMilliseconds() << std::endl;

  return s;
}

std::vector<Move> DPOpt2::SolveI(const Image& target,
                                 const Canvas& original_canvas) {
  Canvas canvas = original_canvas;
  auto sbase = DPProxy::MergeBlocks(canvas);
  for (auto& m : sbase) canvas.Apply(m);
  unsigned nxy = unsigned(sqrt(double(max_xy)));
  auto s = SolveIY(target, canvas, nxy, nxy);
  sbase.insert(sbase.end(), s.begin(), s.end());
  return sbase;
}

Solution DPOpt2::Solve(const Problem& p) {
  std::vector<Move> s = SolveI(p.Target(), p.InitialCanvas());
  auto final_score = Evaluator::Apply(p, s).FScore();
  std::cout << "Final cost = " << final_score << std::endl;
  std::cout << "Done with " << p.Id() << std::endl;
  return Solution(p.Id(), s);
}
