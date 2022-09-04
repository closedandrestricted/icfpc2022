#include "image.h"

#include "spng/spng.h"

#include "common/base.h"
#include "common/template.h"

#include <iostream>

void Image::Init(unsigned _dx, unsigned _dy) {
  dx = _dx;
  dy = _dy;
  Pixel color{255, 255, 255, 255};
  pixels.clear();
  pixels.resize(dx * dy, color);
}

void Image::Color(const Block& b, const Pixel& color) {
  for (unsigned x = b.x0; x < b.x1; ++x) {
    for (unsigned y = b.y0; y < b.y1; ++y) {
      (*this)(x, y) = color;
    }
  }
}

bool Image::LoadPNG(const std::string& filename) {
  FILE* png;
  png = fopen(filename.c_str(), "rb");
  if (png == NULL) {
    std::cout << "Error opening file: [" << filename << "]" << std::endl;
    return false;
  }
  spng_ctx* ctx = spng_ctx_new(0);
  assert(ctx);
  /* Ignore and don't calculate chunk CRC's */
  spng_set_crc_action(ctx, SPNG_CRC_USE, SPNG_CRC_USE);

  /* Set memory usage limits for storing standard and unknown chunks,
     this is important when reading untrusted files! */
  size_t limit = 1024 * 1024 * 64;
  spng_set_chunk_limits(ctx, limit, limit);

  /* Set source PNG */
  spng_set_png_file(ctx, png); /* or _buffer(), _stream() */

  struct spng_ihdr ihdr;
  int ret = spng_get_ihdr(ctx, &ihdr);
  assert(ret == 0);

  //   printf(
  //       "width: %u\n"
  //       "height: %u\n"
  //       "bit depth: %u\n"
  //       "color type: %u \n",
  //       ihdr.width, ihdr.height, ihdr.bit_depth, ihdr.color_type);

  //   printf(
  //       "compression method: %u\n"
  //       "filter method: %u\n"
  //       "interlace method: %u\n",
  //       ihdr.compression_method, ihdr.filter_method, ihdr.interlace_method);

  size_t image_size;

  /* Output format, does not depend on source PNG format except for
     SPNG_FMT_PNG, which is the PNG's format in host-endian or
     big-endian for SPNG_FMT_RAW.
     Note that for these two formats <8-bit images are left byte-packed */
  int fmt = SPNG_FMT_PNG;

  /* With SPNG_FMT_PNG indexed color images are output as palette indices,
     pick another format to expand them. */
  if (ihdr.color_type == SPNG_COLOR_TYPE_INDEXED) fmt = SPNG_FMT_RGB8;

  ret = spng_decoded_image_size(ctx, fmt, &image_size);
  assert(ret == 0);

  unsigned char* image = (unsigned char*)malloc(image_size);
  assert(image);

  /* Decode the image in one go */
  ret = spng_decode_image(ctx, image, image_size, fmt, 0);
  assert(ret == 0);

  //   for (unsigned i = 0; i < 1600; ++i)
  //     std::cout << unsigned(image[i + 1600 * 5]) << " ";
  //   std::cout << std::endl;

  dx = ihdr.width;
  dy = ihdr.height;
  assert((dx == 400) && (dy == 400));
  pixels.resize(dx * dy);
  auto p = image;
  for (unsigned y = 0; y < dy; ++y) {
    for (unsigned x = 0; x < dx; ++x) {
      for (unsigned i = 0; i < 4; ++i) {
        pixels[x * dy + (dy - y - 1)].rgba[i] = *p++;
      }
    }
  }

  /* Free context memory */
  spng_ctx_free(ctx);
  free(image);

  FakeUse(ret);
  return true;
}

bool Image::LoadSJSON(const std::string& /*filename*/) {
  // TODO: real code
  return false;
}
