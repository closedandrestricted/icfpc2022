#pragma once

#include "block.h"
#include "image.h"
#include "pixel.h"

#include <vector>

namespace opt {
class Color {
 public:
  static double Cost(const std::vector<Pixel>& points, const Pixel& color);
  static std::vector<Pixel> Points(const Block& block, const Image& image);

  static Pixel MSE(const std::vector<Pixel>& points);
  static Pixel Median(const std::vector<Pixel>& points);
  static Pixel MinCost(const std::vector<Pixel>& points, const Pixel& initial);
  static Pixel MinCost(const std::vector<Pixel>& points);

  static double ApproxCost(const Block& block, const Image& image);
};
}  // namespace opt
