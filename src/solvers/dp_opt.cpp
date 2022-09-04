#include "solvers/dp_opt.h"

#include "block.h"
#include "cost.h"
#include "evaluator.h"
#include "optimization/color.h"

#include "common/assert_exception.h"
#include "common/vector/enumerate.h"
#include "common/vector/make.h"
#include "common/vector/unique.h"

#include <algorithm>
#include <queue>
#include <unordered_map>
#include <vector>

using namespace src_solvers;

Solution DPOpt::Solve(const Problem& p) {
  auto& target = p.Target();
  auto& canvas = p.InitialCanvas();
  auto& current = canvas.GetImage();
  auto initial_blocks = canvas.GetBlocks();

  std::vector<unsigned> vhsx, vhsy;
  for (auto& b : initial_blocks) {
    vhsx.push_back(b.x0);
    vhsx.push_back(b.x1);
    vhsy.push_back(b.y0);
    vhsy.push_back(b.y1);
  }
  nvector::UniqueUnsorted(vhsx);
  nvector::UniqueUnsorted(vhsy);
  if (vhsx.size() * vhsy.size() > max_xy) return Solution(p.Id(), {});

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

  auto Score = [&](unsigned x0, unsigned x1, unsigned y0, unsigned y1) {
    if ((x1 <= x0) || (y1 <= y0)) return 0.;
    std::vector<double> v(8, 0.0);
    for (unsigned i = 0; i < 8; ++i) {
      v[i] = vvv[x1][y1][i] - vvv[x0][y1][i] - vvv[x1][y0][i] + vvv[x0][y0][i];
    }
    double s = 0;
    for (unsigned i = 0; i < 8; i += 2) {
      double s0 = (x1 - x0) * (y1 - y0), s1 = v[i], s2 = v[i + 1];
      s += s2 - s1 * s1 / s0;
    }
    Assert(s > -1e-6);
    return s;
  };

  // Compress
  // TODO: Speed up to avoid calculation over same space
  unsigned step = 1;
  auto vx = nvector::Enumerate<unsigned>(0, target.dx + 1, step);
  auto vy = nvector::Enumerate<unsigned>(0, target.dy + 1, step);
  for (; vx.size() * vy.size() > max_xy;) {
    double best_score = 1e32;
    unsigned bi = 0, bj = 0;
    for (unsigned i = 1; i + 1 < vx.size(); ++i) {
      auto x = vx[i];
      if (std::find(vhsx.begin(), vhsx.end(), x) != vhsx.end()) continue;
      double score = 0.0;
      for (unsigned j = 1; j < vy.size(); ++j) {
        double d = Score(vx[i - 1], vx[i + 1], vy[j - 1], vy[j]) -
                   Score(vx[i - 1], vx[i], vy[j - 1], vy[j]) -
                   Score(vx[i], vx[i + 1], vy[j - 1], vy[j]);
        Assert(d > -1e-3);
        score += d;
      }
      if (score < best_score) {
        best_score = score;
        bi = i;
        bj = 0;
      }
    }
    for (unsigned j = 1; j + 1 < vy.size(); ++j) {
      auto y = vy[j];
      if (std::find(vhsy.begin(), vhsy.end(), y) != vhsy.end()) continue;
      double score = 0.0;
      for (unsigned i = 1; i < vx.size(); ++i) {
        double d = Score(vx[i - 1], vx[i], vy[j - 1], vy[j + 1]) -
                   Score(vx[i - 1], vx[i], vy[j - 1], vy[j]) -
                   Score(vx[i - 1], vx[i], vy[j], vy[j + 1]);
        Assert(d > -1e-3);
        score += d;
      }
      if (score < best_score) {
        best_score = score;
        bi = 0;
        bj = j;
      }
    }
    Assert((bi > 0) || (bj > 0));
    if (bi > 0) {
      vx.erase(vx.begin() + bi);
    } else {
      vy.erase(vy.begin() + bj);
    }
  }
  std::cout << "Problem " << p.Id() << "\tcompression finished." << std::endl;

  // DP
  class DPV {
   public:
    double min_cost_keep_color;
    double min_cost_recolor;
    double cost_to_color;
    Pixel color;
    unsigned i0, j0;  // keep color
    unsigned i1, j1;  // recolor
  };

  auto vdp =
      nvector::Make<DPV>(vx.size(), vx.size(), vy.size(), vy.size(), DPV());
  for (unsigned id = 1; id < vx.size(); ++id) {
    for (unsigned i0 = 0; i0 + id < vx.size(); ++i0) {
      unsigned i1 = i0 + id;
      for (unsigned jd = 1; jd < vy.size(); ++jd) {
        for (unsigned j0 = 0; j0 + jd < vy.size(); ++j0) {
          unsigned j1 = j0 + jd;
          auto& dpv = vdp[i0][i1][j0][j1];
          Block b{vx[i0], vx[i1], vy[j0], vy[j1], ""};
          dpv.min_cost_keep_color = Similarity(target, current, b);
          auto vp = opt::Color::Points(b, target);
          dpv.color = opt::Color::Median(vp);
          dpv.cost_to_color =
              BaseCost(p.Index(), Move::COLOR) * target.Size() / b.Size();
          dpv.min_cost_recolor =
              dpv.cost_to_color + opt::Color::Cost(vp, dpv.color);
          dpv.i0 = dpv.j0 = dpv.i1 = dpv.j1 = 0;

          double split_cost =
              BaseCost(p.Index(), Move::LINE_CUT) * target.Size() / b.Size();

          double min_cost_after_split = split_cost + dpv.cost_to_color;
          if ((dpv.min_cost_keep_color <= min_cost_after_split) &&
              (dpv.min_cost_recolor <= min_cost_after_split))
            continue;

          for (unsigned is = i0 + 1; is < i1; ++is) {
            auto& dpv1 = vdp[i0][is][j0][j1];
            auto& dpv2 = vdp[is][i1][j0][j1];

            double cost_recolor =
                split_cost + dpv.cost_to_color + dpv1.min_cost_recolor +
                dpv2.min_cost_recolor -
                std::max(dpv1.cost_to_color, dpv2.cost_to_color);
            if (dpv.min_cost_recolor > cost_recolor) {
              dpv.min_cost_recolor = cost_recolor;
              dpv.i1 = is;
              dpv.j1 = 0;
            }

            double cost_keep_color =
                split_cost +
                std::min(dpv1.min_cost_keep_color, dpv1.min_cost_recolor) +
                std::min(dpv2.min_cost_keep_color, dpv2.min_cost_recolor);
            if (dpv.min_cost_keep_color > cost_keep_color) {
              dpv.min_cost_keep_color = cost_keep_color;
              dpv.i0 = is;
              dpv.j0 = 0;
            }
          }
          for (unsigned js = j0 + 1; js < j1; ++js) {
            auto& dpv1 = vdp[i0][i1][j0][js];
            auto& dpv2 = vdp[i0][i1][js][j1];

            double cost_recolor =
                split_cost + dpv.cost_to_color + dpv1.min_cost_recolor +
                dpv2.min_cost_recolor -
                std::max(dpv1.cost_to_color, dpv2.cost_to_color);
            if (dpv.min_cost_recolor > cost_recolor) {
              dpv.min_cost_recolor = cost_recolor;
              dpv.i1 = 0;
              dpv.j1 = js;
            }

            double cost_keep_color =
                split_cost +
                std::min(dpv1.min_cost_keep_color, dpv1.min_cost_recolor) +
                std::min(dpv2.min_cost_keep_color, dpv2.min_cost_recolor);
            if (dpv.min_cost_keep_color > cost_keep_color) {
              dpv.min_cost_keep_color = cost_keep_color;
              dpv.i0 = 0;
              dpv.j0 = js;
            }
          }
        }
      }
    }
  }
  std::cout << "Problem " << p.Id() << "\tdp finished." << std::endl;

  // Construct initial solution
  // Blocks use indexes instead of raw coordinates
  std::vector<Move> s;
  std::unordered_map<std::string, unsigned> mcolormove;
  std::unordered_map<std::string, double> msize;
  std::vector<std::string> vib;
  std::queue<std::pair<Block, bool>> qib;
  double expected_cost = 0.;
  for (auto& ib : initial_blocks) {
    auto itx0 = std::find(vx.begin(), vx.end(), ib.x0);
    auto itx1 = std::find(vx.begin(), vx.end(), ib.x1);
    auto ity0 = std::find(vy.begin(), vy.end(), ib.y0);
    auto ity1 = std::find(vy.begin(), vy.end(), ib.y1);
    Assert((itx0 != vx.end()) && (itx1 != vx.end()) && (ity0 != vy.end()) &&
           (ity1 != vy.end()));
    Block iba{unsigned(itx0 - vx.begin()), unsigned(itx1 - vx.begin()),
              unsigned(ity0 - vy.begin()), unsigned(ity1 - vy.begin()), ib.id};
    for (qib.push({iba, true}); !qib.empty(); qib.pop()) {
      auto qf = qib.front();
      auto& b = qf.first;
      vib.push_back(b.id);
      msize[b.id] = (vx[b.x1] - vx[b.x0]) * (vy[b.y1] - vy[b.y0]);
      auto& dpv = vdp[b.x0][b.x1][b.y0][b.y1];
      if (b == iba) {
        expected_cost +=
            std::min(dpv.min_cost_keep_color, dpv.min_cost_recolor);
      }
      if (qf.second && (dpv.min_cost_keep_color <= dpv.min_cost_recolor)) {
        if ((dpv.i0 == 0) && (dpv.j0 == 0)) {
          // Do nothing
        } else {
          mcolormove[b.id] = s.size();
          s.push_back(Move(Move::SKIP, b.id, Pixel()));
          if (dpv.i0) {
            s.push_back(Move(Move::LINE_CUT, b.id, vx[dpv.i0], 0));
            qib.push({{b.x0, dpv.i0, b.y0, b.y1, b.id + ".0"}, true});
            qib.push({{dpv.i0, b.x1, b.y0, b.y1, b.id + ".1"}, true});
          } else {
            s.push_back(Move(Move::LINE_CUT, b.id, 0, vy[dpv.j0]));
            qib.push({{b.x0, b.x1, b.y0, dpv.j0, b.id + ".0"}, true});
            qib.push({{b.x0, b.x1, dpv.j0, b.y1, b.id + ".1"}, true});
          }
        }
      } else {
        mcolormove[b.id] = s.size();
        if ((dpv.i1 == 0) && (dpv.j1 == 0)) {
          s.push_back(Move(Move::COLOR, b.id, dpv.color));
        } else {
          s.push_back(Move(Move::SKIP, b.id, dpv.color));
          if (dpv.i1) {
            s.push_back(Move(Move::LINE_CUT, b.id, vx[dpv.i1], 0));
            qib.push({{b.x0, dpv.i1, b.y0, b.y1, b.id + ".0"}, false});
            qib.push({{dpv.i1, b.x1, b.y0, b.y1, b.id + ".1"}, false});
          } else {
            s.push_back(Move(Move::LINE_CUT, b.id, 0, vy[dpv.j1]));
            qib.push({{b.x0, b.x1, b.y0, dpv.j1, b.id + ".0"}, false});
            qib.push({{b.x0, b.x1, dpv.j1, b.y1, b.id + ".1"}, false});
          }
        }
      }
    }
  }

  // Adjust COLOR moves
  std::reverse(vib.begin(), vib.end());
  for (auto& id : vib) {
    auto it = mcolormove.find(id);
    if (it == mcolormove.end()) continue;
    auto& m = s[it->second];
    if (m.type == Move::COLOR) continue;
    Assert(m.type == Move::SKIP);
    auto it1 = mcolormove.find(id + ".0");
    auto it2 = mcolormove.find(id + ".1");
    if ((it1 == mcolormove.end()) || (it2 == mcolormove.end())) continue;
    auto& m1 = s[it1->second];
    auto& m2 = s[it2->second];
    if ((m1.type != Move::COLOR) || (m2.type != Move::COLOR)) continue;
    bool b12 = (msize[it1->first] <= msize[it2->first]);
    m.type = Move::COLOR;
    if (b12) {
      m.color = m1.color;
      m1.type = Move::SKIP;
    } else {
      m.color = m2.color;
      m2.type = Move::SKIP;
    }
  }

  // Clean
  std::vector<Move> s2;
  for (auto& m : s) {
    if (m.type != Move::SKIP) s2.push_back(m);
  }
  s.swap(s2);

  auto final_score = Evaluator::Apply(p, s).DScore();
  std::cout << "Expected cost = " << expected_cost
            << "\tfinal = " << final_score << std::endl;
  return Solution(p.Id(), s);
}
