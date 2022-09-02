#include "move.h"

#include "common/base.h"

#include <ostream>
#include <sstream>

std::string Move::Encode() const {
  std::ostringstream ss;
  switch (type) {
    case SKIP:
      break;
    case LINE_CUT:
      assert((x == 0) != (y == 0));
      ss << "cut [" << block_id1 << "] [" << ((x == 0) ? "y" : "x") << "] ["
         << ((x == 0) ? y : x) << "]";
      break;
    case POINT_CUT:
      ss << "cut [" << block_id1 << "] [" << x << ", " << y << "]";
      break;
    case COLOR:
      ss << "color [" << block_id1 << "] [" << unsigned(color.rgba[0]) << ", "
         << unsigned(color.rgba[1]) << ", " << unsigned(color.rgba[2]) << ", "
         << unsigned(color.rgba[3]) << "]";
      break;
    case SWAP:
      ss << "swap [" << block_id1 << "] [" << block_id2 << "]";
      break;
    case MERGE:
      ss << "merge [" << block_id1 << "] [" << block_id2 << "]";
      break;
    default:
      assert(false);
      return "";
  }
  return ss.str();
}

bool Move::Decode(const std::string&) { return false; }
