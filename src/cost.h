#pragma once

#include "block.h"
#include "canvas.h"
#include "image.h"
#include "move.h"

double BaseCost(Move::Type type);
double Cost(const Canvas &canvas, const Move &move);
double Similarity(const Image &i1, const Image &i2);
double Similarity(const Image &i1, const Image &i2, const Block &b);
