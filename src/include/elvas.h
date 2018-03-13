/**
 * @file elvas.h
 * @brief ELVAS core
 * @author Yutaro Shoji (ICRR, the University of Tokyo)
 * @date Created on: 2018/02/01, 13:07
 */

#ifndef ELVAS_H
#define ELVAS_H
#include <string>
#include <cmath>
#include <vector>
#include <stdexcept>

class Elvas {
public:

    static const int nInteg = 111;

    class ElvasError : public std::runtime_error {
    public:

        ElvasError(const std::string& arg_str) : std::runtime_error(arg_str) {
        }
    };

    static double lnPhiC2LnRinv(const double& arg_lnPhiC, std::vector<std::pair<double, double>>&arg_lnPhiC2lnRinv);

    static double getLnGamma(std::vector<std::pair<double, double>>&arg_lndgam, const double& arg_lnRinvBeg, const double& arg_lnRinvEnd);

    static double instantonB(const double& arg_lambdaAbs) {
        return 26.3189450695716 / arg_lambdaAbs;
    }

    static double higgsQC(const double& arg_lambdaAbs, const double& arg_lnQR) {
        return -0.99192944327027 + 2.5 * log(arg_lambdaAbs) - 3. * arg_lnQR;
    }

    static double scalarQC(const double& arg_kappa, const double& arg_lambdaAbs, const double& arg_lnQR);

    static double fermionQC(const double& arg_y, const double& arg_lambdaAbs, const double& arg_lnQR);

    static double gaugeQC(const double& arg_gSquared, const double& arg_lambdaAbs, const double& arg_lnQR);

    class PrintBox {
        std::ostream& _out;
        int _ws;
    public:
        PrintBox(std::ostream& arg_out);
        ~PrintBox();
        void printStr(const std::string& arg_str);
        void emptyLine(int arg_num);

    };

    static void printHeader(std::ostream& arg_out);
};

#endif /* ELVAS_H */

