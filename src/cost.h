#pragma once

#include "canvas.h"
#include "move.h"

double BaseCost(Move::Type type);
double Cost(const Canvas& canvas, const Move& move);
