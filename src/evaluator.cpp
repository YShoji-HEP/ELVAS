/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "include/evaluator.h"
#include "include/ast.h"

void ASTReader::PrintAST::operator()(const AST::_If& arg_ast) {
    if (arg_ast.arguments.size() != 1 && arg_ast.arguments.size() != 2) {
        throw ASTReadError("Wrong number of arguments. (if)");
    }
    _out << "if" << "[";
    _startBr();
    boost::apply_visitor(*this, arg_ast.test);
    for (const auto& elem : arg_ast.arguments) {
        _out << ",\n" + std::string(_position * 2, ' ');
        boost::apply_visitor(*this, elem);
    }
    _endBr();
    _out << "]";
}

void ASTReader::PrintAST::operator()(const AST::_FuncCall& arg_ast) {
    _out << arg_ast.funcName << "[";
    _startBr();
    bool isFirst = true;
    for (const auto& elem : arg_ast.arguments) {
        _out << (isFirst ? (isFirst = false, "") : ",\n" + std::string(_position * 2, ' '));
        boost::apply_visitor(*this, elem);
    }
    if (arg_ast.arguments.size() == 0) {
        _out << "(no_args)";
    }
    _endBr();
    _out << "]";
}

void ASTReader::PrintAST::operator()(const AST::_Signed& arg_ast) {
    _out << "Signed[";
    _startBr();
    _out << "sign = " << (arg_ast.sign == 1 ? "\"+\"," : "\"-\",") << "\n" + std::string(_position * 2, ' ');
    boost::apply_visitor(*this, arg_ast.primary);
    _endBr();
    _out << "]";
}

void ASTReader::PrintAST::operator()(const AST::_PowerUnary& arg_ast) {
    _out << "Power[";
    _startBr();
    boost::apply_visitor(*this, arg_ast.base);
    _out << ",\n" + std::string(_position * 2, ' ');
    boost::apply_visitor(*this, arg_ast.uexp);
    _endBr();
    _out << "]";
}

void ASTReader::PrintAST::operator()(const AST::_PowerInt& arg_ast) {
    _out << "PowerInt[";
    _startBr();
    boost::apply_visitor(*this, arg_ast.base);
    _out << ",\n" + std::string(_position * 2, ' ') << (arg_ast.iexp);
    _endBr();
    _out << "]";
}

void ASTReader::PrintAST::operator()(const AST::_Times& arg_ast) {
    _out << "Times[";
    _startBr();
    boost::apply_visitor(*this, arg_ast.first);
    for (const auto& elem : arg_ast.rest) {
        _out << ",\n" + std::string(_position * 2, ' ');
        _out << "op = " << (elem.invert ? "\"/\"" : "\"*\"") << ",\n" + std::string(_position * 2, ' ');
        boost::apply_visitor(*this, elem.operand);
    }
    _endBr();
    _out << "]";
}

void ASTReader::PrintAST::operator()(const AST::_Plus& arg_ast) {
    _out << "Plus[";
    _startBr();
    bool isFirst = true;
    for (const auto& elem : arg_ast) {
        _out << (isFirst ? (isFirst = false, "") : ",\n" + std::string(_position * 2, ' '));
        boost::apply_visitor(*this, elem);
    }
    _endBr();
    _out << "]";
}

void ASTReader::PrintAST::operator()(const AST::_Relational& arg_ast) {
    _out << "Rel[";
    _startBr();
    boost::apply_visitor(*this, arg_ast.first);
    _out << " ,\n" + std::string(_position * 2, ' ') << "op = " << "\"" << arg_ast.rest.operation << "\",\n" + std::string(_position * 2, ' ');
    boost::apply_visitor(*this, arg_ast.rest.operand);
    _endBr();
    _out << "]";
}

void ASTReader::PrintAST::operator()(const AST::_AndRel& arg_ast) {
    _out << "And[";
    _startBr();
    bool isFirst = true;
    for (const auto& elem : arg_ast) {
        _out << (isFirst ? (isFirst = false, "") : ",\n" + std::string(_position * 2, ' '));
        boost::apply_visitor(*this, elem);
    }
    _endBr();
    _out << "]";
}

void ASTReader::PrintAST::operator()(const AST::_OrRel& arg_ast) {
    _out << "Or[";
    _startBr();
    bool isFirst = true;
    for (const auto& elem : arg_ast) {
        _out << (isFirst ? (isFirst = false, "") : ",\n" + std::string(_position * 2, ' '));
        boost::apply_visitor(*this, elem);
    }
    _endBr();
    _out << "]";
}

void ASTReader::PrintAST::operator()(const AST::_FuncDef& arg_ast) {
    _out << arg_ast.fName.fName << "(";
    bool isFirst = true;
    for (const auto& arg : arg_ast.fName.fArgs) {
        _out << (isFirst ? (isFirst = false, "") : ", ");
        _out << arg;
    }
    _out << ")";
    _out << " :=" << std::endl;
    boost::apply_visitor(*this, arg_ast.expr);
}

ASTReader::Evaluator::Evaluator() {
    _constants["pi"] = M_PI;
    setFunc("sqrt", 1, _sqrt);
    setFunc("max", -2, _max);
    setFunc("min", -2, _min);
    setFunc("pow", 2, _pow);
    setFunc("exp", 1, _exp);
    setFunc("log", 1, _log);
    setFunc("log10", 1, _log10);
    setFunc("sin", 1, _sin);
    setFunc("cos", 1, _cos);
    setFunc("tan", 1, _tan);
    setFunc("abs", 1, _abs);
    setFunc("asin", 1, _asin);
    setFunc("acos", 1, _acos);
    setFunc("atan", 1, _atan);
    setFunc("eval", -1, _eval);
    setFunc("exit", 0, _exit);
}

double ASTReader::Evaluator::operator()(const AST::_Constant& arg_ast) {
    auto it_cons = _constants.find(arg_ast);
    if (it_cons != _constants.end()) {
        return _constants.at(arg_ast);
    } else {
        auto it_func = _functions.find(arg_ast);
        if (it_func != _functions.end() && it_func->second.first == 0) {
            return (it_func->second.second)(std::vector<double>{});
        } else {
            throw ASTReadError("Constant not found. (" + arg_ast + ")");
        }
    }
    return std::nan("");
}

double ASTReader::Evaluator::operator()(AST::_If& arg_ast) {
    if (arg_ast.arguments.size() == 1) {
        if (boost::apply_visitor(*this, arg_ast.test) >= 0.5) {
            return boost::apply_visitor(*this, arg_ast.arguments.at(0));
        } else {
            return 0.;
        }
    } else if (arg_ast.arguments.size() == 2) {
        if (boost::apply_visitor(*this, arg_ast.test) >= 0.5) {
            return boost::apply_visitor(*this, arg_ast.arguments.at(0));
        } else {
            return boost::apply_visitor(*this, arg_ast.arguments.at(1));
        }
    } else {
        throw ASTReadError("The if(...) directive should have two or three arguments.");
    }
    return std::nan("");
}

double ASTReader::Evaluator::operator()(AST::_FuncCall& arg_ast) {
    std::vector<double> arguments;
    for (auto& elem : arg_ast.arguments) {
        arguments.emplace_back(boost::apply_visitor(*this, elem));
    }
    if (!arg_ast.function) {
        auto it_func = _functions.find(arg_ast.funcName);
        if (it_func != _functions.end() && (it_func->second.first == arguments.size() || -(it_func->second.first) <= arguments.size())) {
            arg_ast.function = it_func->second.second;
        } else {
            throw ASTReadError("Function not found or wrong number of arguments. (" + arg_ast.funcName + ")");
        }
    }
    return arg_ast.function(arguments);
}

double ASTReader::Evaluator::operator()(AST::_Times & arg_ast) {
    double temp = boost::apply_visitor(*this, arg_ast.first);
    for (auto& elem : arg_ast.rest) {
        if (elem.invert) {
            temp /= boost::apply_visitor(*this, elem.operand);
        } else {
            temp *= boost::apply_visitor(*this, elem.operand);
        }
    }
    return temp;
}

double ASTReader::Evaluator::operator()(AST::_Plus & arg_ast) {
    double temp = 0.;
    for (auto& elem : arg_ast) {
        temp += boost::apply_visitor(*this, elem);
    }
    return temp;
}

double ASTReader::Evaluator::operator()(AST::_Relational & arg_ast) {
    std::vector<double> lhsrhs(2);
    lhsrhs.at(0) = boost::apply_visitor(*this, arg_ast.first);
    lhsrhs.at(1) = boost::apply_visitor(*this, arg_ast.rest.operand);
    return lhsrhs.at(arg_ast.rest.operation.front() == '>') < lhsrhs.at(arg_ast.rest.operation.front() == '<')
            || (arg_ast.rest.operation.length() == 2 && lhsrhs.at(0) == lhsrhs.at(1)) == (arg_ast.rest.operation.front() != '!');
}

double ASTReader::Evaluator::operator()(AST::_AndRel & arg_ast) {
    bool temp = 1.;
    for (auto& elem : arg_ast) {
        temp = temp && boost::apply_visitor(*this, elem) >= 0.5;
    }
    return temp;
}

double ASTReader::Evaluator::operator()(AST::_OrRel & arg_ast) {
    bool temp = false;
    for (auto& elem : arg_ast) {
        temp = temp || boost::apply_visitor(*this, elem) >= 0.5;
    }
    return temp;
}

double ASTReader::Evaluator::operator()(AST::_Substitute& arg_ast) {
    double temp = boost::apply_visitor(*this, arg_ast.val);
    for (auto& elem : arg_ast.cName) {
        setConst(elem, temp);
    }
    return temp;
}

double ASTReader::Evaluator::operator()(AST::_FuncDef& arg_ast) {
    std::unordered_map<std::string, std::string> rule;
    int i = 0;
    for (const auto& name : arg_ast.fName.fArgs) {
        rule.emplace(name, "_INTERNAL_VARS_" + std::to_string(i));
        i++;
    }
    ChangeConstName chConst(rule);
    AST::Substitute& expr = arg_ast.expr;
    chConst(expr);
    setFunc(arg_ast.fName.fName, arg_ast.fName.fArgs.size(), [ this, expr ](const std::vector<double>& arg_x) mutable {
        size_t i;
        for (i = 0; i < arg_x.size(); i++) {
            setConst("_INTERNAL_VARS_" + std::to_string(i), arg_x.at(i));
        }
        return (*this)(expr);
    });
    return 0.;
}
