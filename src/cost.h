#pragma once

#include "block.h"
#include "canvas.h"
#include "image.h"
#include "move.h"

double BaseCost(unsigned pid, Move::Type type);
double Cost(unsigned pid, Move::Type type, double tsize, double bsize);
double Cost(unsigned pid, Move::Type type, double tsize, const Block &b);
double Cost(const Canvas &canvas, const Move &move);
double Similarity(const Image &i1, const Image &i2);
double Similarity(const Image &i1, const Image &i2, const Block &b);
