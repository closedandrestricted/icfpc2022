#include "canvas.h"

#include "cost.h"
#include "move.h"

#include "common/assert_exception.h"
#include "common/files/json.h"

#include <algorithm>
#include <fstream>
#include <iostream>

void Canvas::Init(unsigned dx, unsigned dy) {
  image.Init(dx, dy);
  blocks.clear();
  blocks["0"] = {0, dx, 0, dy, "0"};
  index = 0;
  isl_cost = 0;
}

std::vector<Block> Canvas::GetBlocks() const {
  std::vector<Block> vb;
  vb.reserve(blocks.size());
  for (const auto &it : blocks) vb.emplace_back(it.second);
  return vb;
}

void Canvas::Apply(const Move &move) {
  isl_cost += Cost(*this, move);
  switch (move.type) {
    case Move::SKIP: {
      break;
    }
    case Move::LINE_CUT: {
      Assert((move.x == 0) != (move.y == 0));
      auto id = move.block_id1;
      auto b = blocks[id];
      if (move.x) {
        Assert((b.x0 < move.x) && (move.x < b.x1));
        blocks[id + ".0"] = Block({b.x0, move.x, b.y0, b.y1, id + ".0"});
        blocks[id + ".1"] = Block({move.x, b.x1, b.y0, b.y1, id + ".1"});
      } else {
        Assert((b.y0 < move.y) && (move.y < b.y1));
        blocks[id + ".0"] = Block({b.x0, b.x1, b.y0, move.y, id + ".0"});
        blocks[id + ".1"] = Block({b.x0, b.x1, move.y, b.y1, id + ".1"});
      }
      blocks.erase(id);
      break;
    }
    case Move::POINT_CUT: {
      auto id = move.block_id1;
      auto b = blocks[id];
      Assert((b.x0 < move.x) && (move.x < b.x1));
      Assert((b.y0 < move.y) && (move.y < b.y1));
      blocks[id + ".0"] = Block({b.x0, move.x, b.y0, move.y, id + ".0"});
      blocks[id + ".1"] = Block({move.x, b.x1, b.y0, move.y, id + ".1"});
      blocks[id + ".3"] = Block({b.x0, move.x, move.y, b.y1, id + ".3"});
      blocks[id + ".2"] = Block({move.x, b.x1, move.y, b.y1, id + ".2"});
      blocks.erase(id);
      break;
    }
    case Move::COLOR: {
      const auto &b = Get(move.block_id1);
      image.Color(b, move.color);
      break;
    }
    case Move::SWAP: {
      auto &b1 = Get(move.block_id1);
      auto &b2 = Get(move.block_id2);
      Assert((b1.x1 - b1.x0 == b2.x1 - b2.x0) &&
             (b1.y1 - b1.y0 == b2.y1 - b2.y0));
      for (unsigned x = 0; x < b1.x1 - b1.x0; ++x) {
        for (unsigned y = 0; y < b1.y1 - b1.y0; ++y) {
          image(x + b1.x0, y + b1.y0)
              .rgba.swap(image(x + b2.x0, y + b2.y0).rgba);
        }
      }
      std::swap(b1.x0, b2.x0);
      std::swap(b1.x1, b2.x1);
      std::swap(b1.y0, b2.y0);
      std::swap(b1.y1, b2.y1);
      break;
    }
    case Move::MERGE: {
      auto id1 = move.block_id1, id2 = move.block_id2;
      auto b1 = blocks[id1], b2 = blocks[id2];
      if ((b2.x0 < b1.x0) || (b2.y0 < b1.y0)) std::swap(b1, b2);
      if (b1.x1 == b2.x0) {
        Assert((b1.y0 == b2.y0) && (b1.y1 == b2.y1));
      } else if (b1.y1 == b2.y0) {
        Assert((b1.x0 == b2.x0) && (b1.x1 == b2.x1));
      } else {
        Assert(false);
      }
      auto new_id = std::to_string(++index);
      blocks[new_id] = Block({b1.x0, b2.x1, b1.y0, b2.y1, new_id});
      blocks.erase(id1);
      blocks.erase(id2);
      break;
    }
    default:
      assert(false);
      break;
  }
}

namespace {
bool TestFile(const std::string &filename) {
  std::ifstream f(filename);
  return f.good();
}
}  // namespace

bool Canvas::Load(const std::string &filename) {
  // std::cout << "Load canvas from file " << filename << std::endl;
  if (!TestFile(filename)) return true;
  files::JSON js;
  if (!js.Load(filename)) return false;
  // std::cout << "Json loaded" << std::endl;
  isl_cost = 0;
  image.Init(js.GetInteger("width"), js.GetInteger("height"));
  auto &jsbs = js.GetValue("blocks");
  index = jsbs.Size() - 1;
  blocks.clear();
  for (unsigned i = 0; i < jsbs.Size(); ++i) {
    auto &jsbi = jsbs.GetValue(i);
    auto &c0 = jsbi.GetValue("bottomLeft");
    auto &c1 = jsbi.GetValue("topRight");
    Block b{unsigned(c0.GetInteger(0)), unsigned(c1.GetInteger(0)),
            unsigned(c0.GetInteger(1)), unsigned(c1.GetInteger(1)),
            jsbi.GetString("blockId")};
    blocks[b.id] = b;
    auto &jc = jsbi.GetValue("color");
    Pixel color;
    for (unsigned j = 0; j < 4; ++j) color.rgba[j] = jc.GetInteger(j);
    image.Color(b, color);
  }
  return true;
}
