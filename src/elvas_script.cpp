/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "include/elvas_script.h"
#include "include/parser.h"
#include <iomanip>

ElvasScript::ElvasScript(std::istream& arg_is, std::ostream& arg_os) : Interpreter(arg_is, arg_os) {
    arg_os << std::scientific;
    auto InstantonB = [ this ](const std::vector<double>& arg_x) {
        return Elvas::instantonB(-_eval("HIGGS_QUARTIC_COUPLING"));
    };

    auto HiggsQC = [ this ](const std::vector<double>& arg_x) {
        return Elvas::higgsQC(-_eval("HIGGS_QUARTIC_COUPLING"), _eval("LN_QR"));
    };

    auto ScalarQC = [ this ](const std::vector<double>& arg_x) {
        return Elvas::scalarQC(arg_x.at(0), -_eval("HIGGS_QUARTIC_COUPLING"), _eval("LN_QR"));
    };

    auto FermionQC = [ this ](const std::vector<double>& arg_x) {
        return Elvas::fermionQC(arg_x.at(0), -_eval("HIGGS_QUARTIC_COUPLING"), _eval("LN_QR"));
    };

    auto GaugeQC = [ this ](const std::vector<double>& arg_x) {
        return Elvas::gaugeQC(arg_x.at(0), -_eval("HIGGS_QUARTIC_COUPLING"), _eval("LN_QR"));
    };

    auto savedLnGamma = [ this ](const std::vector<double>& arg_x) {
        _dlngamma.emplace_back(_eval("LN_RINV"), arg_x.at(0));
        return 0.;
    };

    auto saveLnPhiC = [ this ](const std::vector<double>& arg_x) {
        const double&lambda = _eval("HIGGS_QUARTIC_COUPLING"), &lnRinv = _eval("LN_RINV");
        _lnPhiC.emplace_back(lnRinv + .5 * log(8.) - .5 * log(-lambda), lnRinv);
        return 0.;
    };

    auto initialize = [ this ](const std::vector<double>& arg_x) {
        _lnPhiC.clear();
        _dlngamma.clear();
        return 0.;
    };

    auto checkSize = [ this ](const std::vector<double>& arg_x) {
        return _dlngamma.size() >= 3;
    };

    auto getMaxLnRinv = [ this ](const std::vector<double>& arg_x) {
        if (_dlngamma.size() < 3) {
            throw EScriptError("get_max_lnRinv: Too small data size.");
        }
        auto it_max = std::max_element(_dlngamma.begin(), _dlngamma.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
        });
        double temp = it_max->first;
        try {
            temp = std::min(temp, Elvas::lnPhiC2LnRinv(arg_x.front(), _lnPhiC));
        } catch (const Elvas::ElvasError& arg_e) {
        }
        return std::min(temp, arg_x.front());
    };

    auto getMinLnRinv = [ this ](const std::vector<double>& arg_x) {
        if (_dlngamma.size() < 3) {
            throw EScriptError("get_min_lnRinv: Too small data size.");
        }
        auto it_min = std::min_element(_dlngamma.begin(), _dlngamma.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
        });
        double temp = it_min->first;
        try {
            temp = std::max(temp, Elvas::lnPhiC2LnRinv(arg_x.front(), _lnPhiC));
        } catch (const Elvas::ElvasError& arg_e) {
        }
        return std::max(temp, arg_x.front());
    };

    auto getLnGamma = [ this ](const std::vector<double>& arg_x) {
        return Elvas::getLnGamma(_dlngamma, arg_x.at(0), arg_x.at(1));
    };

    auto outputPrecision = [ &arg_os ](const std::vector<double>& arg_x) {
        arg_os << std::setprecision((int) (arg_x.front() + 0.5));
        return 0.;
    };

    setFunc("InstantonB", 0, InstantonB);
    setFunc("HiggsQC", 0, HiggsQC);
    setFunc("ScalarQC", 1, ScalarQC);
    setFunc("FermionQC", 1, FermionQC);
    setFunc("GaugeQC", 1, GaugeQC);
    setFunc("output_precision", 1, outputPrecision);
    setFunc("initialize", 0, initialize);
    setFunc("save_phiC", 0, saveLnPhiC);
    setFunc("save_dlngamma_dRinv", 1, savedLnGamma);
    setFunc("is_data_enough", 0, checkSize);
    setFunc("get_max_lnRinv", 1, getMaxLnRinv);
    setFunc("get_min_lnRinv", 1, getMinLnRinv);
    setFunc("get_lngamma", 2, getLnGamma);
}