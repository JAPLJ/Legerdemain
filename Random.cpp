#include "Random.h"

#include <random>

Random Random::instance;

int Random::randint(int lo, int hi) {
    std::uniform_int_distribution<int> dist(lo, hi);
    return dist(instance.mt);
}
