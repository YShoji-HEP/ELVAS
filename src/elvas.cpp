/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "include/version.h"
#include "include/elvas.h"
#include "include/ntools.h"
#include <iostream>

#ifdef _WIN32
#define NOMINMAX 1
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

double Elvas::lnPhiC2LnRinv(const double& arg_lnPhiC, std::vector<std::pair<double, double>>&arg_lnPhiC2lnRinv) {
    if (arg_lnPhiC2lnRinv.size() < 3) {
        throw ElvasError("lnPhiC2LnRinv: Data size is too small.");
    }

    std::sort(arg_lnPhiC2lnRinv.begin(), arg_lnPhiC2lnRinv.end(), [](std::pair<double, double>& a, std::pair<double, double>& b) {
        return a.second < b.second;
    });

    auto it_lnPhiC2lnRinv = arg_lnPhiC2lnRinv.begin();
    while (it_lnPhiC2lnRinv != arg_lnPhiC2lnRinv.end() - 1) {
        if (it_lnPhiC2lnRinv->first < (it_lnPhiC2lnRinv + 1)->first) {
            break;
        }
        ++it_lnPhiC2lnRinv;
    }
    if (it_lnPhiC2lnRinv == arg_lnPhiC2lnRinv.end() - 1) {
        throw ElvasError("lnPhiC2LnRinv: Corresponding lnRinv is not found. (decreasing lnRinv)");
    }

    if (arg_lnPhiC < it_lnPhiC2lnRinv->first || arg_lnPhiC2lnRinv.back().first < arg_lnPhiC) {
        throw ElvasError("lnPhiC2LnRinv: Corresponding lnRinv is not found. (out of range)");
    }
    return NTools::interpolateL2(it_lnPhiC2lnRinv, arg_lnPhiC2lnRinv.end(), arg_lnPhiC);
}

double Elvas::getLnGamma(std::vector<std::pair<double, double>>&arg_lndgam, const double& arg_lnRinvBeg, const double& arg_lnRinvEnd) {
    if (arg_lndgam.size() < 3) {
        throw ElvasError("getLnGamma: Data size is too small.");
    }
    if (arg_lnRinvBeg >= arg_lnRinvEnd) {
        throw ElvasError("getLnGamma: Invalid region of integration.");
    }

    std::sort(arg_lndgam.begin(), arg_lndgam.end(), [](std::pair<double, double>& a, std::pair<double, double>& b) {
        return a.first < b.first;
    });

    auto it_max = std::max_element(arg_lndgam.begin(), arg_lndgam.end(), [](const std::pair<double, double>& a, const std::pair<double, double>& b) {
        return a.second < b.second;
    });
    double& lndgamMax = it_max->second;

    double dlnRinv = (arg_lnRinvEnd - arg_lnRinvBeg) / (nInteg - 1.);

    std::vector<double> dgamma(nInteg);
    int i;
    for (i = 0; i < nInteg; i++) {
        dgamma.at(i) = exp(NTools::interpolateL2(arg_lndgam.begin(), arg_lndgam.end(), arg_lnRinvBeg + dlnRinv * i, true) - lndgamMax);
    }

    return lndgamMax + log(NTools::integrateSIMP(dgamma.begin(), dgamma.end(), dlnRinv, NTools::SIMPSON_LAST));
}

double Elvas::scalarQC(const double& arg_kappa, const double& arg_lambdaAbs, const double& arg_lnQR) {
    double temp;
    double x = arg_kappa / arg_lambdaAbs;
    double x2 = x * x;
    double x3 = x * x2;
    double x4 = x * x3;
    if (x < 0.7) {
        double x5 = x * x4;
        double x6 = x * x5;
        double x7 = x * x6;
        double x8 = x * x7;
        double x9 = x * x8;
        double x10 = x * x9;
        temp = -0.239133939224974 * x2 + 0.222222222222222 * x3
                - 0.134704602106396 * x4 + 0.102278606592866 * x5
                - 0.0839329261179402 * x6 + 0.0715956882048009 * x7
                - 0.0625481711576628 * x8 + 0.0555697470602515 * x9
                - 0.0500042455037409 * x10 - 0.333333333333333 * x2 * arg_lnQR;
    } else {
        temp = -0.0261559272783723 + 0.0000886704923163256 / x4
                + 0.0000962000962000962 / x3 + 0.000198412698412698 / x2
                + 0.00105820105820106 / x + 0.111111111111111 * x
                - 0.181204187497805 * x2 + (-0.0055555555555556
                + 0.166666666666667 * x2) * log(x) - 0.333333333333333 * x2 * arg_lnQR;
    }
    return temp;
}

double Elvas::fermionQC(const double& arg_y, const double& arg_lambdaAbs, const double& arg_lnQR) {
    double temp;
    double x = arg_y * arg_y / arg_lambdaAbs;
    double x2 = x * x;
    double x3 = x * x2;
    if (x < 1.3) {
        double x4 = x * x3;
        double x5 = x * x4;
        double x6 = x * x5;
        double x7 = x * x6;
        double x8 = x * x7;
        temp = 0.64493454511661 * x + 0.005114971505109 * x2
                - 0.0366953662258276 * x3 + 0.00476307962690785 * x4
                - 0.000845451274112082 * x5 + 0.000168244913551417 * x6
                - 0.0000353785958610453 * x7 + 7.67709260595572e-6 * x8
                + (0.66666666666667 * x + 0.333333333333333 * x2) * arg_lnQR;
    } else {
        temp = -0.227732960077634 + 0.00260942760942761 / x3
                + 0.00271164021164021 / x2 + 0.00820105820105820 / x
                + 0.53790187962670 * x + 0.296728717591129 * x2
                + (-0.06111111111111111 - 0.3333333333333333 * x
                - 0.1666666666666666 * x2) * log(x)
                + (0.66666666666667 * x + 0.333333333333333 * x2) * arg_lnQR;
    }
    return temp;
}

double Elvas::gaugeQC(const double& arg_gSquared, const double& arg_lambdaAbs, const double& arg_lnQR) {
    double temp;
    double x = arg_gSquared / arg_lambdaAbs;
    double x2 = x * x;
    double x3 = x * x2;
    double x4 = x * x3;
    if (x < 1.1) {
        double x5 = x * x4;
        double x6 = x * x5;
        double x7 = x * x6;
        double x8 = x * x7;
        temp = -0.966861032843734 - 1.76813696868318 * x + 0.61593151565841 * x2
                + 0.127848258241082 * x3 - 0.0205690315959429 * x4
                + 0.00467728575401191 * x5 - 0.00121386963701736 * x6
                + 0.000336192073844430 * x7 - 0.0000966171446396430 * x8
                + 0.5 * log(arg_lambdaAbs)
                + (-0.333333333333333 - 2. * x -  x2) * arg_lnQR;
    } else {
        double sqrt_x = sqrt(x);
        double x3_2 = x * sqrt_x;
        double x5_2 = x * x3_2;
        double x7_2 = x * x5_2;
        temp = -0.580011057371274 + 0.000482461693399193 / x4 - 0.0000211853167446059 / x7_2
                + 0.000685425685425685 / x3 - 0.000271172054330955 / x5_2 + 0.00218253968253968 / x2
                - 0.00433875286929528 / x3_2 + 0.0198412698412698 / x - 0.138840091817449 / sqrt_x
                + 2.22144146907918 * sqrt_x - 1.58722512498683 * x - 0.210279229160082 * x2
                + (-0.183333333333333 + x + 0.5 * x2) * log(x)
                + 0.5 * log(x / cosh(8.47412669784234e-6 / x7_2 * 
                    (5. + 64. * x + 1024. * x2 + 32768. * x3 - 524288. * x4)))
                + 0.5 * log(arg_lambdaAbs)
                - (0.333333333333333 + 2. * x + x2) * arg_lnQR;
    }
    return temp;
}

Elvas::PrintBox::PrintBox(std::ostream& arg_out) : _out(arg_out) {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info) != 0 && info.srWindow.Right - info.srWindow.Left + 1 > 0) {
        _ws = info.srWindow.Right - info.srWindow.Left + 1;
    } else {
        _ws = 60;
    }
#else
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != -1 && ws.ws_col > 0) {
        _ws = ws.ws_col;
    } else {
        _ws = 60;
    }
#endif
    _out << std::string(_ws, '#') << std::endl;
}

Elvas::PrintBox::~PrintBox() {
    _out << std::string(_ws, '#') << std::endl;
}

void Elvas::PrintBox::printStr(const std::string& arg_str) {
    int leftsp = std::max((int) (_ws - 2 - arg_str.length()) / 2, 0);
    int rightsp = std::max((int) (_ws - leftsp - 2 - arg_str.length()), 0);
    _out << "#" << std::string(leftsp, ' ') << arg_str << std::string(rightsp, ' ') << "#" << std::endl;
}

void Elvas::PrintBox::emptyLine(int arg_num) {
    for (int i = 0; i < arg_num; i++) {
        _out << "#" << std::string(_ws - 2, ' ') << "#" << std::endl;
    }
}

void Elvas::printHeader(std::ostream& arg_out) {
    PrintBox box(arg_out);
    box.emptyLine(2);
    box.printStr("ELVAS");
    box.printStr("----------------------------");
    box.printStr("C++ Package for");
    box.printStr("ELectroweak VAcuum Stability");
    box.printStr("(version " + std::to_string(ELVAS_VERSION_MAJOR) + "." + std::to_string(ELVAS_VERSION_MINOR) + ")");
    box.emptyLine(1);
    box.printStr("- Authors -");
    box.printStr("S. Chigusa, T. Moroi, Y. Shoji");
    box.emptyLine(1);
    box.printStr("For more details, see");
    box.printStr("https://github.com/YShoji-HEP/ELVAS");
    box.emptyLine(1);
    box.printStr("Phys.Rev.Lett. 119 (2017) no.21, 211801");
    box.printStr("Phys. Rev. D 97, 116012 (2018)");
    box.printStr("arXiv: 2406.05180 [hep-ph]");
    box.emptyLine(2);
}