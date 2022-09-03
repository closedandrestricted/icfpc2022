#include "color.h"

#include "common/base.h"

#include <algorithm>
#include <vector>

using namespace opt;

double Color::Cost(const std::vector<Pixel>& points, const Pixel& color) {
  double s = 0;
  for (auto& p : points) s += Distance(p, color);
  return s / 200;
}

std::vector<Pixel> Color::Points(const Block& block, const Image& image) {
  std::vector<Pixel> v;
  for (unsigned x = block.x0; x < block.x1; ++x) {
    for (unsigned y = block.y0; y < block.y1; ++y) v.push_back(image(x, y));
  }
  return v;
}

Pixel Color::MSE(const std::vector<Pixel>& points) {
  assert(!points.empty());
  std::array<unsigned, 4> as{0, 0, 0, 0};
  for (auto& p : points) {
    for (unsigned i = 0; i < 4; ++i) as[i] += p.rgba[i];
  }
  Pixel r;
  for (unsigned i = 0; i < 4; ++i)
    r.rgba[i] = (as[i] + points.size() / 2) / points.size();

  return r;
}

Pixel Color::Median(const std::vector<Pixel>& points) {
  assert(!points.empty());
  std::vector<std::vector<uint8_t>> vv(4);
  for (auto& p : points) {
    for (unsigned i = 0; i < 4; ++i) vv[i].push_back(p.rgba[i]);
  }
  unsigned m = points.size() / 2;
  Pixel r;
  for (unsigned i = 0; i < 4; ++i) {
    std::nth_element(vv[i].begin(), vv[i].begin() + m, vv[i].end());
    r.rgba[i] = vv[i][m];
  }
  return r;
}

// TODO:
//   Make it faster
Pixel Color::MinCost(const std::vector<Pixel>& points,
                     const Pixel& initial_point) {
  auto cc = initial_point;
  double sbest = Cost(points, cc);
  unsigned lit = 0;
  for (unsigned it = 0; it < lit + 4; ++it) {
    unsigned d = it % 4;
    for (; cc.rgba[d] < 255;) {
      auto cc1 = cc;
      cc1.rgba[d] += 1;
      auto s = Cost(points, cc1);
      if (s < sbest) {
        sbest = s;
        cc = cc1;
        lit = it;
      } else {
        break;
      }
    }
    for (; cc.rgba[d] > 0;) {
      auto cc1 = cc;
      cc1.rgba[d] -= 1;
      auto s = Cost(points, cc1);
      if (s < sbest) {
        sbest = s;
        cc = cc1;
        lit = it;
      } else {
        break;
      }
    }
  }
  return cc;
}

Pixel Color::MinCost(const std::vector<Pixel>& points) {
  auto c1 = MSE(points), c2 = Median(points);
  double s1 = Cost(points, c1), s2 = Cost(points, c2);
  return MinCost(points, ((s1 < s2) ? c1 : c2));
}
