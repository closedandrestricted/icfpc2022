#pragma once

#include "block.h"
#include "image.h"

#include <vector>

namespace opt {
// Compress coordinates
class Compression {
 protected:
  Image image, image2;

 protected:
  static double Score(const std::vector<double>& v1,
                      const std::vector<double>& v2);

  static std::vector<unsigned> CompressXI(const Image& image, const Block& b,
                                          unsigned vxsize, bool merge_by_y);

 public:
  void Init(const Image& image);

  std::vector<unsigned> CompressX(const Block& b, unsigned vxsize,
                                  bool merge_by_y);
  std::vector<unsigned> CompressY(const Block& b, unsigned vysize,
                                  bool merge_by_x);
};
}  // namespace opt
