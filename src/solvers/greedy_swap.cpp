#include "solvers/greedy_swap.h"

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

GreedySwap::PSolver GreedySwap::Clone() const {
  return std::make_shared<GreedySwap>(*this);
}

std::vector<Move> GreedySwap::SolveI(unsigned pid, const Image& target,
                                     const Image& current,
                                     const std::vector<Block>& current_blocks) {
    /*
    std::unordered_map<Block::TDim, std::vector<size_t>> dim2indices;
    for (size_t i = 0; i < current_blocks.size(); ++i) {
        dim2indices[current_blocks[i].Dim()].emplace_back(i);
    }
    */
    (void)pid;
    (void)target;
    (void)current;
    (void)current_blocks;
    return {};
}

std::vector<Move> GreedySwap::SolveI(const Image& target,
                                     const Canvas& canvas) {
  return SolveI(canvas.pid, target, canvas.GetImage(), canvas.GetBlocks());
}

Solution GreedySwap::Solve(const Problem& p) {
  std::vector<Move> s = SolveI(p.Target(), p.InitialCanvas());
  std::cout << "Done with " << p.Id() << std::endl;
  return Solution(p.Id(), s);
}
