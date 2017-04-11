#ifndef RANDOM_H
#define RANDOM_H

#include <QtGlobal>
#include <random>

class Random
{
private:
    std::mt19937 mt;

    static Random instance;
    Random() {
        std::random_device rd;
        mt.seed(rd());
        qsrand(rd());
    }

public:
    static int randint(int lo, int hi);

    template<typename T>
    static void shuffle(T begin, T end) {
        std::shuffle(begin, end, instance.mt);
    }
};

#endif // RANDOM_H
