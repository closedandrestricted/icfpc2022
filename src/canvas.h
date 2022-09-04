#pragma once

#include "block.h"
#include "image.h"

#include "common/assert_exception.h"
#include "common/base.h"

#include <string.h>
#include <unordered_map>
#include <vector>

class Move;

class Canvas {
 public:
  Image image;
  std::unordered_map<std::string, Block> blocks;
  unsigned index;
  double isl_cost;

 protected:
  void InitBlocks();

 public:
  Canvas() { Init(1, 1); }
  Canvas(unsigned dx, unsigned dy) { Init(dx, dy); }

  void Init(unsigned dx, unsigned dy);

  Image &GetImage() { return image; }
  const Image &GetImage() const { return image; }

  unsigned BSize() const { return blocks.size(); }
  std::vector<Block> GetBlocks() const;

  double Size() const { return image.Size(); }

  Block &Get(const std::string &id) { return blocks[id]; }

  const Block &Get(const std::string &id) const {
    auto it = blocks.find(id);
    Assert(it != blocks.end());
    return it->second;
  }

  void Apply(const Move &move);

  bool LoadJSON(const std::string &filename);
  bool LoadPNG(const std::string &filename);
  bool LoadSJSON(const std::string &filename);
};
