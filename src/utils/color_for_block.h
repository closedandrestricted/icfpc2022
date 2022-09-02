#pragma once

#include "block.h"
#include "image.h"
#include "pixel.h"

double Cost(const Block& block, const Image& image, const Pixel& color);

Pixel ColorMSE(const Block& block, const Image& image);
