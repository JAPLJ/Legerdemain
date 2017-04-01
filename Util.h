#ifndef UTIL_H
#define UTIL_H

#include <bitset>
#include <limits>

class Util
{
private:
    Util() { }

public:
    template<typename T>
    static int popcount(T x) {
        return std::bitset<std::numeric_limits<T>::digits>(x).count();
    }

    static int to2p(int note_idx);
};

#endif // UTIL_H
