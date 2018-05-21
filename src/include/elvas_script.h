/**
 * @file elvas_script.h
 * @brief Interpreter of ELVAS
 * @author Yutaro Shoji (ICRR, the University of Tokyo)
 * @date Created on: 2018/02/12, 13:03
 */

#ifndef ELVAS_SCRIPT_H
#define ELVAS_SCRIPT_H

#include "interpreter.h"
#include "elvas.h"

class ElvasScript : public Interpreter {
    std::vector<std::pair<double, double>> _lndgamma, _lnPhiC;
public:

    class EScriptError : public std::runtime_error {
    public:

        EScriptError(const std::string& str) : std::runtime_error(str) {
        }
    };

    ElvasScript(std::istream& arg_is, std::ostream& arg_os);

};

#endif /* ELVAS_SCRIPT_H */

