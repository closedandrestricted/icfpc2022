#include "solvers/inverted_greedy.h"

#include "block.h"
#include "cost.h"
#include "optimization/color.h"

#include "common/heap.h"
#include "common/stl/hash/array.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace src_solvers;

double InvertedGreedy::SimCost(const Image& target, const Block& b,
                               const Pixel& c) {
  auto vp = opt::Color::Points(b, target);
  auto cc = opt::Color::MinCost(vp, c);
  return opt::Color::Cost(vp, cc);
}

double InvertedGreedy::Score(const Image& target, const SBlock& b0,
                             const SBlock& b1) {
  Block b{b0.x0, b1.x1, b0.y0, b1.y1, ""};
  auto sim_cost =
      SimCost(target, b, (b0.Size() < b1.Size() ? b1.color : b0.color));
  auto msize = std::max(b0.Size(), b1.Size());
  auto split_cost = BaseCost(Move::LINE_CUT) * target.Size() / b.Size();
  auto color_cost = BaseCost(Move::COLOR) * target.Size() / msize;
  return split_cost + color_cost + b0.sim_cost + b1.sim_cost - sim_cost;
}

Solution InvertedGreedy::Solve(const Problem& p) {
  const auto& target = p.Target();
  std::vector<SBlock> vblocks;
  vblocks.reserve(2 * target.Size());
  vblocks.push_back({0, 0, 0, 0, 0, 0., {}});
  std::vector<std::unordered_map<std::array<unsigned, 3>, unsigned>> vhm(4);
  for (unsigned x = 0; x < target.dx; ++x) {
    for (unsigned y = 0; y < target.dy; ++y) {
      SBlock b{x, x + 1, y, y + 1, unsigned(vblocks.size()), 0., target(x, y)};
      vblocks.push_back(b);
      vhm[0][{b.x1, b.y0, b.y1}] = b.id;
      vhm[1][{b.x0, b.y0, b.y1}] = b.id;
      vhm[2][{b.x0, b.x1, b.y1}] = b.id;
      vhm[3][{b.x0, b.x1, b.y0}] = b.id;
    }
  }
  HeapMaxOnTop<std::pair<double, std::pair<unsigned, unsigned>>> h;
  for (unsigned x = 0; x < target.dx; ++x) {
    for (unsigned y = 0; y < target.dy; ++y) {
      auto id2 = x * target.dy + y + 1;
      if (x > 0) {
        auto id1 = id2 - target.dy;
        auto score = Score(target, vblocks[id1], vblocks[id2]);
        if (score >= 0) h.Add({score, {id1, id2}});
      }
      if (y > 0) {
        auto id1 = id2 - 1;
        auto score = Score(target, vblocks[id1], vblocks[id2]);
        if (score >= 0) h.Add({score, {id1, id2}});
      }
    }
  }
  for (; !h.Empty();) {
    auto t = h.Extract();
    auto id1 = t.second.first, id2 = t.second.second;
    auto &b1 = vblocks[id1], &b2 = vblocks[id2];
    if ((b1.id == 0) || (b2.id == 0)) continue;
    b1.id = 0;
    b2.id = 0;
    Block bb{b1.x0, b2.x1, b1.y0, b2.y1, ""};
    auto vp = opt::Color::Points(bb, target);
    auto cc =
        opt::Color::MinCost(vp, (b1.Size() < b2.Size() ? b2.color : b1.color));
    auto sim_cost = opt::Color::Cost(vp, cc);
    SBlock bnew{bb.x0,    bb.x1, bb.y0, bb.y1, unsigned(vblocks.size()),
                sim_cost, cc};
    vblocks.push_back(bnew);
    vhm[0][{bnew.x1, bnew.y0, bnew.y1}] = bnew.id;
    vhm[1][{bnew.x0, bnew.y0, bnew.y1}] = bnew.id;
    vhm[2][{bnew.x0, bnew.x1, bnew.y1}] = bnew.id;
    vhm[3][{bnew.x0, bnew.x1, bnew.y0}] = bnew.id;
    if (bnew.x0 > 0) {
      auto idt = vhm[0][{bnew.x0, bnew.y0, bnew.y1}];
      if (vblocks[idt].id != 0) {
        auto score = Score(target, vblocks[idt], bnew);
        if (score >= 0) h.Add({score, {idt, bnew.id}});
      }
    }
    if (bnew.x1 < target.dx) {
      auto idt = vhm[1][{bnew.x1, bnew.y0, bnew.y1}];
      if (vblocks[idt].id != 0) {
        auto score = Score(target, bnew, vblocks[idt]);
        if (score >= 0) h.Add({score, {bnew.id, idt}});
      }
    }
    if (bnew.y0 > 0) {
      auto idt = vhm[2][{bnew.x0, bnew.x1, bnew.y0}];
      if (vblocks[idt].id != 0) {
        auto score = Score(target, vblocks[idt], bnew);
        if (score >= 0) h.Add({score, {idt, bnew.id}});
      }
    }
    if (bnew.y1 < target.dy) {
      auto idt = vhm[3][{bnew.x0, bnew.x1, bnew.y1}];
      if (vblocks[idt].id != 0) {
        auto score = Score(target, bnew, vblocks[idt]);
        if (score >= 0) h.Add({score, {bnew.id, idt}});
      }
    }
  }
  std::vector<SBlock> vactive;
  for (auto& b : vblocks) {
    if (b.id != 0) vactive.push_back(b);
  }
  std::cout << "Problem " << p.Id() << " finished." << std::endl;
  std::cout << "Final blocks = " << 2 * target.Size() - vblocks.size() << "\t"
            << vactive.size() << std::endl;
  return Solution(p.Id(), {});
}
