#include "block.h"
#include "cost.h"
#include "optimization/color.h"
#include "solvers/greedy_swap.h"

#include <algorithm>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace src_solvers;

GreedySwap::GreedySwap(unsigned, Base::PSolver subsolver)
    : subsolver_(subsolver) {}

std::vector<Move> GreedySwap::SolveI(const Image& target, const Image& current,
                                     const std::vector<Block>& current_blocks) {
  struct QueueNode {
    Block b;
    double sim_cost0;
    Pixel best_color;
    double isl_cost;
    double sim_cost;
  };

  std::vector<Move> s;
  std::queue<QueueNode> q;
  std::vector<std::string> vid;
  std::unordered_map<std::string, QueueNode> mqnodes;
  std::unordered_map<std::string, Pixel> mcolors;
  std::unordered_map<std::string, std::pair<unsigned, unsigned>> msplit;

  for (auto& b : current_blocks) {
    auto vp = opt::Color::Points(b, target);
    auto bc = opt::Color::MinCost(vp);
    q.push({b, Similarity(target, current, b), bc,
            BaseCost(Move::COLOR) * target.Size() / b.Size(),
            opt::Color::Cost(vp, bc)});
  }
  for (; !q.empty(); q.pop()) {
    auto qn = q.front();
    auto& bq = qn.b;
    auto cost0 = std::min(qn.sim_cost0, qn.isl_cost + qn.sim_cost);
    auto best_cost = cost0;
    unsigned best_split_x = 0, best_split_y = 0;
    QueueNode qn1, qn2;
    double split_cost = (BaseCost(Move::LINE_CUT) * target.Size()) / bq.Size();
    for (unsigned x = bq.x0 + 1; x < bq.x1; ++x) {
      unsigned dx = std::max(x - bq.x0, bq.x1 - x);
      double color_cost =
          (BaseCost(Move::COLOR) * target.Size()) / (dx * (bq.y1 - bq.y0));
      double extra_cost = split_cost + color_cost;
      if (extra_cost >= best_cost) continue;
      Block b1{bq.x0, x, bq.y0, bq.y1, bq.id + ".0"};
      Block b2{x, bq.x1, bq.y0, bq.y1, bq.id + ".1"};
      double sim_cost0_1 = Similarity(target, current, b1);
      double sim_cost0_2 = Similarity(target, current, b2);
      auto vp1 = opt::Color::Points(b1, target);
      auto vp2 = opt::Color::Points(b2, target);
      auto c1 = opt::Color::MinCost(vp1, qn.best_color);
      auto c2 = opt::Color::MinCost(vp2, qn.best_color);
      double sim_cost_1 = opt::Color::Cost(vp1, c1);
      double sim_cost_2 = opt::Color::Cost(vp2, c2);
      double sim_cost =
          std::min(sim_cost0_1, sim_cost_1) + std::min(sim_cost0_2, sim_cost_2);
      if (best_cost > sim_cost + extra_cost) {
        // Warning. If best_cost == qn.sim_cost0 but we need to color both parts
        // extra_cost is qb.isl_cost off.
        best_cost = sim_cost + extra_cost;
        best_split_x = x;
        best_split_y = 0;
        qn1 = QueueNode{b1, sim_cost0_1, c1,
                        BaseCost(Move::COLOR) * target.Size() / b1.Size(),
                        sim_cost_1};
        qn2 = QueueNode{b2, sim_cost0_2, c2,
                        BaseCost(Move::COLOR) * target.Size() / b2.Size(),
                        sim_cost_2};
      }
    }
    for (unsigned y = bq.y0 + 1; y < bq.y1; ++y) {
      unsigned dy = std::max(y - bq.y0, bq.y1 - y);
      double color_cost =
          (BaseCost(Move::COLOR) * target.Size()) / (dy * (bq.x1 - bq.x0));
      double extra_cost = split_cost + color_cost;
      if (extra_cost >= best_cost) continue;
      Block b1{bq.x0, bq.x1, bq.y0, y, bq.id + ".0"};
      Block b2{bq.x0, bq.x1, y, bq.y1, bq.id + ".1"};
      double sim_cost0_1 = Similarity(target, current, b1);
      double sim_cost0_2 = Similarity(target, current, b2);
      auto vp1 = opt::Color::Points(b1, target);
      auto vp2 = opt::Color::Points(b2, target);
      auto c1 = opt::Color::MinCost(vp1, qn.best_color);
      auto c2 = opt::Color::MinCost(vp2, qn.best_color);
      double sim_cost_1 = opt::Color::Cost(vp1, c1);
      double sim_cost_2 = opt::Color::Cost(vp2, c2);
      double sim_cost =
          std::min(sim_cost0_1, sim_cost_1) + std::min(sim_cost0_2, sim_cost_2);
      if (best_cost > sim_cost + extra_cost) {
        // Warning. If best_cost == qn.sim_cost0 but we need to color both parts
        // extra_cost is qb.isl_cost off.
        best_cost = sim_cost + extra_cost;
        best_split_x = 0;
        best_split_y = y;
        qn1 = QueueNode{b1, sim_cost0_1, c1,
                        BaseCost(Move::COLOR) * target.Size() / b1.Size(),
                        sim_cost_1};
        qn2 = QueueNode{b2, sim_cost0_2, c2,
                        BaseCost(Move::COLOR) * target.Size() / b2.Size(),
                        sim_cost_2};
      }
    }
    vid.push_back(bq.id);
    mqnodes[bq.id] = qn;
    if (best_cost < cost0) {
      q.push(qn1);
      q.push(qn2);
      msplit[bq.id] = {best_split_x, best_split_y};
    } else if (qn.sim_cost < qn.sim_cost0) {
      mcolors[bq.id] = qn.best_color;
    }
  }
  std::reverse(vid.begin(), vid.end());
  for (auto& id : vid) {
    auto it = msplit.find(id);
    if (it != msplit.end()) {
      auto it1 = mcolors.find(id + ".0");
      auto it2 = mcolors.find(id + ".1");
      if ((it1 != mcolors.end()) && (it2 != mcolors.end())) {
        auto& b1 = mqnodes[id + ".0"].b;
        auto& b2 = mqnodes[id + ".1"].b;
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
  return s;
}

std::vector<Move> GreedySwap::SolveI(const Image& target,
                                     const Canvas& canvas) {
  return SolveI(target, canvas.GetImage(), canvas.GetBlocks());
}

Solution GreedySwap::Solve(const Problem& p) {
  std::vector<Move> s = SolveI(p.Target(), p.InitialCanvas());
  std::cout << "Done with " << p.Id() << std::endl;
  return Solution(p.Id(), s);
}
