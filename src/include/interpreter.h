/**
 * @file interpreter.h
 * @brief Script interpreter
 * @author Yutaro Shoji (ICRR, the University of Tokyo)
 * @date Created on: 2018/02/12, 22:54
 */

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "evaluator.h"
#include "parser.h"
#include <iostream>

class Interpreter {
protected:
    std::istream& _is;
    std::ostream& _os;
    ASTReader::Evaluator _eval;
    std::vector<AST::Expression> _begRoutine, _mainRoutine, _endRoutine, _finRoutine;
    char _section;
    std::string _recordDelim, _datasetDelim, _outputDelim;
    std::vector<std::string> _recordVarNames, _datasetVarNames;
    std::unordered_map<std::string, std::string> _strings;
    std::unordered_map<std::string, std::vector<std::string>> _lists;
    bool _break, _continue;
    std::vector<std::string> _printStr;

    void _executeAST(std::vector<AST::Expression>& arg_asts);

    void _beginFunc(const std::vector<std::string>& arg_varNames, const std::vector<double>& arg_secVals);

    void _mainFunc(const std::vector<std::string>& arg_varNames, const std::vector<double>& arg_recordVals);

    void _endFunc() {
        _executeAST(_endRoutine);
    }

    void _finFunc() {
        for (auto& ast : _finRoutine) {
            _eval(ast);
        }
    }

    template<class DataType>
    void _getData(std::unordered_map<std::string, DataType>& arg_map, const std::string& arg_name, DataType& arg_result);

    bool _readGenSec(const std::string& arg_buf);

    bool _readInitSec(const std::string& arg_buf);

    bool _readDataSec(const std::string& arg_buf);

    bool _readOtherSec(const std::string& arg_buf);

    bool _readDatasetVar(const std::string& arg_secVar);

public:

    class InterpreterError : public std::runtime_error {
        size_t _pos;
        std::string _expected;
        std::string _original;
        bool _isSet;
    public:

        InterpreterError(const std::string& str) : std::runtime_error(str), _isSet(false) {
        }

        InterpreterError(const boost::spirit::x3::expectation_failure<std::string::iterator>& arg_e, std::string& arg_in)
        : std::runtime_error("Parser: Expectation Error."), _pos(std::distance(arg_in.begin(), arg_e.where())), _expected(arg_e.which()), _original(arg_in), _isSet(true) {
        }

        void errorMsg(std::ostream& arg_out) const;
    };

    Interpreter(std::istream& arg_is, std::ostream& arg_os);

    void evaluateAST(AST::Expression& arg_ast) {
        _eval(arg_ast);
    }

    void setConst(const std::string& arg_name, const double& arg_val) {
        _eval.setConst(arg_name, arg_val);
    }

    void setFunc(const std::string& arg_name, const int& arg_argNum, const std::function<double(const std::vector<double>& arg_x)>& arg_func) {
        _eval.setFunc(arg_name, arg_argNum, arg_func);
    }

    void eraseConst(const std::string& arg_name) {
        _eval.eraseConst(arg_name);
    }

    void eraseFunc(const std::string& arg_name) {
        _eval.eraseFunc(arg_name);
    }

    const double& getConst(const std::string& arg_name) const {
        return _eval.getConst(arg_name);
    }

    void setOutputDelim(const std::string& arg_delim) {
        _outputDelim = arg_delim;
    }

    void analyze();

    void interactive();
};

#endif /* INTERPRETER_H */

