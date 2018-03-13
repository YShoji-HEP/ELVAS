/**
 * @file ntools.h
 * @brief Numerical tools
 * @author Yutaro Shoji (ICRR, the University of Tokyo)
 * @date Created on: 2018/02/08, 19:33
 */

#ifndef NTOOLS_H
#define NTOOLS_H

#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <string>
#include <cmath>
#include <algorithm>
#include <utility>

class NTools {
public:
    static const int SIMPSON_FIRST = -1;
    static const int SIMPSON_LAST = 1;

    class NtoolsError : public std::runtime_error {
    public:

        NtoolsError(const std::string& str) : std::runtime_error(str) {
        }
    };

    template<class Iter>
    static double integrateSIMP(Iter arg_yfirst, Iter arg_ylast, const double& arg_dx, int arg_even = 0);

    template<class Iter>
    static double interpolateL2(Iter arg_it_first, Iter arg_it_last, const double& arg_x, const bool& arg_replaceFirst = false);

    template<class Number>
    static Number powInt(const Number& arg_base, const int32_t& arg_exp);
};


////////////////////////////////////////////////////////
//
// Implementation
//
////////////////////////////////////////////////////////

template<class Iter>
double NTools::integrateSIMP(Iter arg_yfirst, Iter arg_ylast, const double& arg_dx, int arg_even) {
    size_t ysize = std::distance(arg_yfirst, arg_ylast);
    Iter iter_y;
    bool isOdd = ysize & 1;

    if (ysize == 0 || (!isOdd && abs(arg_even) != 1)) {
        throw NtoolsError("Simpson integrator: wrong inputs.");
    }

    double sum = 0.;
    for (iter_y = arg_yfirst + (!isOdd && arg_even == SIMPSON_LAST); iter_y < arg_ylast - 2; iter_y += 2) {
        sum += *iter_y + *(iter_y + 1) * 2.;
    }
    sum *= 2.;

    if (isOdd) {
        sum += *(arg_ylast - 1) - *(arg_yfirst);
    } else {
        if (arg_even == SIMPSON_LAST) {
            sum += 0.5 * *(arg_yfirst + 1) + 1.5 * *(arg_yfirst) + *(arg_ylast - 1);
        }
        if (arg_even == SIMPSON_FIRST) {
            sum += 1.5 * *(arg_ylast - 1) + 2.5 * *(arg_ylast - 2) - *(arg_yfirst);
        }
    }
    sum *= arg_dx / 3.;
    return sum;
}

template<class Iter>
double NTools::interpolateL2(Iter arg_it_first, Iter arg_it_last, const double& arg_x, const bool& arg_replaceFirst) {
    auto it_match = std::lower_bound(arg_it_first, arg_it_last, arg_x, [](const auto& a, const double& b) {
        return a.first < b;
    });

    if (it_match == arg_it_last) {
        if (fabs(arg_x - (it_match - 1)->first) > 0.1 * fabs((it_match - 1)->first - (it_match - 2)->first)) {
            throw NtoolsError("Interpolation: out of range (High).");
        }
        --it_match;
    } else if (it_match == arg_it_first) {
        if (fabs(arg_x - it_match->first) > 0.1 * fabs(it_match->first - (it_match + 1)->first)) {
            throw NtoolsError("Interpolation: out of range (Low).");
        }
        ++it_match;
    } else if (it_match == arg_it_last - 1) {
        --it_match;
    } else if (it_match != arg_it_first + 1 && 2. * arg_x < (it_match->first + (it_match - 1)->first)) {
        --it_match;
    }
    if (arg_replaceFirst) {
        arg_it_first = it_match - 2;
    }

    const double &x0 = (it_match - 1)->first, &x1 = (it_match)->first, &x2 = (it_match + 1)->first;
    const double &y0 = (it_match - 1)->second, &y1 = (it_match)->second, &y2 = (it_match + 1)->second;
    return (arg_x - x1) * (arg_x - x2) / ((x0 - x1) * (x0 - x2)) * y0
            + (arg_x - x0) * (arg_x - x2) / ((x1 - x0) * (x1 - x2)) * y1
            + (arg_x - x0) * (arg_x - x1) / ((x2 - x0) * (x2 - x1)) * y2;
};

template<class Number>
Number NTools::powInt(const Number& arg_base, const int32_t& arg_exp) {
    Number result = 1.;
    uint32_t tester = 0xFFFFFFFF;
    uint32_t extractor = 1;
    uint32_t absexp = arg_exp > 0 ? arg_exp : -arg_exp;
    Number powBase = arg_base;
    while (absexp & tester) {
        result = (absexp & extractor) ? result * powBase : result;
        powBase *= powBase;
        extractor <<= 1;
        tester <<= 1;
    }
    return arg_exp > 0 ? result : 1. / result;
}

#endif /* NTOOLS_H */

