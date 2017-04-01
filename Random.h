#ifndef RANDOM_H
#define RANDOM_H

#include <random>

class Random
{
private:
    std::mt19937 mt;

    static Random instance;
    Random() {
        std::random_device rd;
        mt.seed(rd());
    }

public:
    static int randint(int lo, int hi);
};

#endif // RANDOM_H
