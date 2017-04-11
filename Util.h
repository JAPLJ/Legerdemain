#ifndef UTIL_H
#define UTIL_H

#include <QJsonObject>

#include <bitset>
#include <limits>
#include <algorithm>

class Util
{
private:
    Util() { }

public:
    template<typename T>
    static int popcount(T x) {
        return std::bitset<std::numeric_limits<T>::digits>(x).count();
    }

    template<typename T>
    static T clip(T lo, T x, T hi) {
        return std::min(hi, std::max(lo, x));
    }

    static int to2p(int note_idx);
};

#endif // UTIL_H
