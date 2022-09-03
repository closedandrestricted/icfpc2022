#include "solvers/greedy_split.h"

#include "block.h"
#include "cost.h"
#include "optimization/color.h"

#include <algorithm>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace src_solvers;

Solution GreedySplit::Solve(const Problem& p) {
  std::vector<Move> s;
  auto& i = p.Target();
  if (p.InitialCanvas().BSize() > 1) return Solution(p.Id(), s);
  Block b0{0, i.dx, 0, i.dy, "0"};
  std::queue<std::pair<Block, Pixel>> q;
  std::vector<std::string> vid;
  std::unordered_map<std::string, Block> mblocks;
  std::unordered_map<std::string, Pixel> mcolors;
  std::unordered_map<std::string, std::pair<unsigned, unsigned>> msplit;
  for (q.push({b0, opt::Color::MinCost(opt::Color::Points(b0, i))}); !q.empty();
       q.pop()) {
    auto bq = q.front().first;
    auto cq = q.front().second;
    auto cost = opt::Color::Cost(opt::Color::Points(bq, i), cq);
    auto best_cost = cost;
    unsigned best_split_x = 0, best_split_y = 0;
    Block best_b1, best_b2;
    Pixel best_c1, best_c2;
    double split_cost = (BaseCost(Move::LINE_CUT) * i.Size()) / bq.Size();
    for (unsigned x = bq.x0 + 1; x < bq.x1; ++x) {
      unsigned dx = std::max(x - bq.x0, bq.x1 - x);
      double color_cost =
          (BaseCost(Move::COLOR) * i.Size()) / (dx * (bq.y1 - bq.y0));
      double extra_cost = split_cost + color_cost;
      if (extra_cost >= best_cost) continue;
      Block b1{bq.x0, x, bq.y0, bq.y1, bq.id + ".0"};
      Block b2{x, bq.x1, bq.y0, bq.y1, bq.id + ".1"};
      auto vp1 = opt::Color::Points(b1, i);
      auto vp2 = opt::Color::Points(b2, i);
      auto c1 = opt::Color::MinCost(vp1, cq);
      auto c2 = opt::Color::MinCost(vp2, cq);
      double sim_cost = opt::Color::Cost(vp1, c1) + opt::Color::Cost(vp2, c2);
      if (best_cost > sim_cost + extra_cost) {
        best_cost = sim_cost + extra_cost;
        best_split_x = x;
        best_split_y = 0;
        best_b1 = b1;
        best_b2 = b2;
        best_c1 = c1;
        best_c2 = c2;
      }
    }
    for (unsigned y = bq.y0 + 1; y < bq.y1; ++y) {
      unsigned dy = std::max(y - bq.y0, bq.y1 - y);
      double color_cost =
          (BaseCost(Move::COLOR) * i.Size()) / (dy * (bq.x1 - bq.x0));
      double extra_cost = split_cost + color_cost;
      if (extra_cost >= best_cost) continue;
      Block b1{bq.x0, bq.x1, bq.y0, y, bq.id + ".0"};
      Block b2{bq.x0, bq.x1, y, bq.y1, bq.id + ".1"};
      auto vp1 = opt::Color::Points(b1, i);
      auto vp2 = opt::Color::Points(b2, i);
      auto c1 = opt::Color::MinCost(vp1, cq);
      auto c2 = opt::Color::MinCost(vp2, cq);
      double sim_cost = opt::Color::Cost(vp1, c1) + opt::Color::Cost(vp2, c2);
      if (best_cost > sim_cost + extra_cost) {
        best_cost = sim_cost + extra_cost;
        best_split_x = 0;
        best_split_y = y;
        best_b1 = b1;
        best_b2 = b2;
        best_c1 = c1;
        best_c2 = c2;
      }
    }
    // std::cout << "id = " << bq.id << "\tcost = " << cost
    //           << "\tbest = " << best_cost << std::endl;
    vid.push_back(bq.id);
    mblocks[bq.id] = bq;
    if (best_cost < cost) {
      q.push({best_b1, best_c1});
      q.push({best_b2, best_c2});
      msplit[bq.id] = {best_split_x, best_split_y};
    } else {
      mcolors[bq.id] = cq;
    }
  }
  std::reverse(vid.begin(), vid.end());
  for (auto& id : vid) {
    auto it = mcolors.find(id);
    if (it == mcolors.end()) {
      auto it1 = mcolors.find(id + ".0");
      auto it2 = mcolors.find(id + ".1");
      if ((it1 == mcolors.end()) || (it2 == mcolors.end())) {
        std::cout << "Something is wrong" << std::endl;
        assert(false);
      } else {
        auto& b1 = mblocks[id + ".0"];
        auto& b2 = mblocks[id + ".1"];
        if (b1.Size() < b2.Size()) {
          mcolors[id] = mcolors[b1.id];
          mcolors.erase(b1.id);
        } else {
          mcolors[id] = mcolors[b2.id];
          mcolors.erase(b2.id);
        }
      }
    }
  }
  std::reverse(vid.begin(), vid.end());
  for (auto& id : vid) {
    auto itc = mcolors.find(id);
    if (itc != mcolors.end()) {
      s.push_back(Move(Move::COLOR, id, itc->second));
    }
    auto its = msplit.find(id);
    if (its != msplit.end()) {
      s.push_back(
          Move(Move::LINE_CUT, id, its->second.first, its->second.second));
    }
  }
  std::cout << "Done with " << p.Id() << std::endl;
  // auto c = opt::Color::MinCost(opt::Color::Points(b, i));
  return Solution(p.Id(), s);
}
