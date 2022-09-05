#include "optimization/compression.h"

#include "utils/flip.h"

#include "common/base.h"
#include "common/heap/ukvm/dheap.h"
#include "common/vector/enumerate.h"
#include "common/vector/make.h"

using namespace opt;

double Compression::Score(const std::vector<double>& v1,
                          const std::vector<double>& v2) {
  assert(v1.size() == v2.size());
  assert((v1.size() % 8) == 0);
  double total = 0.0;
  for (unsigned j = 0; j < v1.size(); j += 8) {
    for (unsigned i = 0; i < 4; ++i) {
      auto d = v1[j + i + 4] * v2[j + i] - v1[j + i] * v2[j + i + 4];
      total += d * d / (v1[j + i] * v2[j + i] * (v1[j + i] + v2[j + i]));
    }
  }
  return total;
}

std::vector<unsigned> Compression::CompressXI(const Image& image,
                                              const Block& b, unsigned vxsize,
                                              bool merge_by_y) {
  assert(b.Size() > 0);
  if (vxsize <= 2) return {b.x0, b.x1};
  if (b.x1 - b.x0 + 1 <= vxsize)
    return nvector::Enumerate<unsigned>(b.x0, b.x1 + 1);
  auto vv = nvector::Make<double>(b.x1 - b.x0,
                                  8 * (merge_by_y ? 1u : b.y1 - b.y0), 0.);
  for (unsigned x = b.x0; x < b.x1; ++x) {
    for (unsigned y = b.y0; y < b.y1; ++y) {
      auto ya = 8 * (merge_by_y ? 0 : y - b.y0);
      auto& p = image(x, y);
      for (auto i = 0; i < 4; ++i) {
        vv[x - b.x0][ya + i] += 1.;
        vv[x - b.x0][ya + i + 4] += p.rgba[i];
      }
    }
  }

  heap::ukvm::DHeap<4, double> dheap(vv.size() + 1);
  auto vl = nvector::Enumerate<unsigned>(0, vv.size());
  auto vr = nvector::Enumerate<unsigned>(1, vv.size() + 1);
  auto sl = nvector::Enumerate<unsigned>(0, vv.size());
  auto sr = nvector::Enumerate<unsigned>(0, vv.size() + 1);
  sl.insert(sl.begin(), vv.size());
  for (unsigned i = 1; i < vv.size(); ++i)
    dheap.AddNewKey(i, Score(vv[sl[i]], vv[sr[i]]));

  for (; dheap.Size() > vxsize - 2;) {
    auto s = dheap.ExtractKey();
    auto l = sl[s], r = sr[s];
    for (unsigned j = 0; j < vv[l].size(); ++j) vv[l][j] += vv[r][j];
    auto s0 = vl[l], s1 = vr[r];
    sl[s1] = l;
    vr[l] = s1;
    for (auto si : {s0, s1}) {
      if (dheap.InHeap(si)) dheap.Set(si, Score(vv[sl[si]], vv[sr[si]]));
    }
  }

  std::vector<unsigned> output(1, b.x0);
  for (unsigned i = 0; i < dheap.UKeySize(); ++i) {
    if (dheap.InHeap(i)) output.push_back(i + b.x0);
  }
  output.push_back(b.x1);
  return output;
}

void Compression::Init(const Image& _image) {
  image = _image;
  image2 = FlipXY(image);
}

std::vector<unsigned> Compression::CompressX(const Block& b, unsigned vxsize,
                                             bool merge_by_y) {
  return CompressXI(image, b, vxsize, merge_by_y);
}

std::vector<unsigned> Compression::CompressY(const Block& b, unsigned vysize,
                                             bool merge_by_x) {
  return CompressXI(image2, FlipXY(b), vysize, merge_by_x);
}
