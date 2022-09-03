#include "canvas.h"

#include "cost.h"
#include "move.h"

void Canvas::Init(unsigned dx, unsigned dy) {
  image.Init(dx, dy);
  blocks.clear();
  blocks["0"] = {0, dx, 0, dy, "0"};
  index = 0;
  isl_cost = 0;
}

void Canvas::Apply(const Move &move) {
  isl_cost += Cost(*this, move);
  switch (move.type) {
    case Move::SKIP: {
      break;
    }
    case Move::LINE_CUT: {
      assert((move.x == 0) != (move.y == 0));
      auto id = move.block_id1;
      auto b = blocks[id];
      if (move.x) {
        blocks[id + ".0"] = Block({b.x0, move.x, b.y0, b.y1, id + ".0"});
        blocks[id + ".1"] = Block({move.x, b.x1, b.y0, b.y1, id + ".1"});
      } else {
        blocks[id + ".0"] = Block({b.x0, b.x1, b.y0, move.y, id + ".0"});
        blocks[id + ".1"] = Block({b.x0, b.x1, move.y, b.y1, id + ".1"});
      }
      blocks.erase(id);
      break;
    }
    case Move::POINT_CUT: {
      auto id = move.block_id1;
      auto b = blocks[id];
      blocks[id + ".0"] = Block({b.x0, move.x, b.y0, move.y, id + ".0"});
      blocks[id + ".1"] = Block({move.x, b.x1, b.y0, move.y, id + ".1"});
      blocks[id + ".3"] = Block({b.x0, move.x, move.y, b.y1, id + ".3"});
      blocks[id + ".2"] = Block({move.x, b.x1, move.y, b.y1, id + ".2"});
      blocks.erase(id);
      break;
    }
    case Move::COLOR: {
      const auto &b = Get(move.block_id1);
      for (unsigned x = b.x0; x < b.x1; ++x) {
        for (unsigned y = b.y0; y < b.y1; ++y) {
          image(x, y) = move.color;
        }
      }
      break;
    }
    case Move::SWAP: {
      const auto &b1 = Get(move.block_id1);
      const auto &b2 = Get(move.block_id2);
      assert((b1.x1 - b1.x0 == b2.x1 - b2.x0) &&
             (b1.y1 - b1.y0 == b2.y1 - b2.y0));
      for (unsigned x = 0; x < b1.x1 - b1.x0; ++x) {
        for (unsigned y = 0; y < b1.y1 - b1.y0; ++y) {
          image(x + b1.x0, y + b1.y0)
              .rgba.swap(image(x + b2.x0, y + b2.y0).rgba);
        }
      }
      break;
    }
    case Move::MERGE: {
      auto id1 = move.block_id1, id2 = move.block_id2;
      auto b1 = blocks[id1], b2 = blocks[id2];
      if ((b2.x0 < b1.x0) || (b2.y0 < b1.y0)) std::swap(b1, b2);
      if (b1.x1 == b2.x0) {
        assert((b1.y0 == b2.y0) && (b1.y1 == b2.y1));
      } else if (b1.y1 == b2.y0) {
        assert((b1.x0 == b2.x0) && (b1.x1 == b2.x1));
      } else {
        assert(false);
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
