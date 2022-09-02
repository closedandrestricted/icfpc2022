#include "move.h"

#include "common/base.h"
#include "common/string/utils/split.h"

#include <iostream>
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

bool Move::Decode(const std::string& s) {
  auto vs = Split(s, " ,[]");
  if (vs.empty()) {
    type = SKIP;
    return true;
  }
  assert(vs.size() >= 3);
  if (vs[0] == "cut") {
    // ...
    return false;
  } else if (vs[0] == "color") {
    assert(vs.size() == 6);
    type = COLOR;
    block_id1 = vs[1];
    for (unsigned i = 0; i < 4; ++i) {
      color.rgba[i] = stoi(vs[i + 2]);
    }
    return true;
  } else {
    // ...
    return false;
  }
  return true;
}
