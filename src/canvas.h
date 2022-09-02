#pragma once

#include "block.h"
#include "image.h"

#include "common/base.h"

#include <string.h>
#include <unordered_map>

class Move;

class Canvas {
 public:
  Image image;
  std::unordered_map<std::string, Block> blocks;
  unsigned index;
  double isl_cost;

 public:
  Canvas(unsigned dx, unsigned dy) { Init(dx, dy); }

  void Init(unsigned dx, unsigned dy);

  double Size() const { return image.Size(); }

  Block& Get(const std::string& id) { return blocks[id]; }

  const Block& Get(const std::string& id) const {
    auto it = blocks.find(id);
    assert(it != blocks.end());
    return it->second;
  }

  void Apply(const Move& move);
};
