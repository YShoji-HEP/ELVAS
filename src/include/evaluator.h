/**
 * @file evaluator.h
 * @brief AST evaluator, printer
 * @author Yutaro Shoji (ICRR, the University of Tokyo)
 * @date Created on: 2018/02/13, 19:17
 */

#ifndef EVALUATOR_H
#define EVALUATOR_H

#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif

#include "ast.h"
#include "ntools.h"
#include <unordered_map>

namespace ASTReader {
    namespace x3 = boost::spirit::x3;

    class ASTReadError : public std::runtime_error {
    public:

        ASTReadError(const std::string& str) : std::runtime_error(str) {
        }
    };

    class DoNothing {
    public:

        virtual void operator()(const double& arg_ast) {
        }

        virtual void operator()(AST::_Constant& arg_ast) {
        }

        virtual void operator()(AST::_If& arg_ast) {
            for (auto& elem : arg_ast.arguments) {
                boost::apply_visitor(*this, elem);
            }
        }

        virtual void operator()(AST::_FuncCall& arg_ast) {
            for (auto& elem : arg_ast.arguments) {
                boost::apply_visitor(*this, elem);
            }
        }

        virtual void operator()(AST::Primary& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

        virtual void operator()(AST::_Signed& arg_ast) {
            boost::apply_visitor(*this, arg_ast.primary);
        }

        virtual void operator()(AST::Signed& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

        virtual void operator()(AST::_PowerUnary& arg_ast) {
            boost::apply_visitor(*this, arg_ast.base);
            boost::apply_visitor(*this, arg_ast.uexp);
        }

        virtual void operator()(AST::_PowerInt& arg_ast) {
            boost::apply_visitor(*this, arg_ast.base);
        }

        virtual void operator()(AST::Power& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

        virtual void operator()(AST::_Times& arg_ast) {
            boost::apply_visitor(*this, arg_ast.first);
            for (auto& elem : arg_ast.rest) {
                boost::apply_visitor(*this, elem.operand);
            }
        }

        virtual void operator()(AST::Times& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

        virtual void operator()(AST::_Plus& arg_ast) {
            for (auto& elem : arg_ast) {
                boost::apply_visitor(*this, elem);
            }
        }

        virtual void operator()(AST::Plus& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

        virtual void operator()(AST::_Relational& arg_ast) {
            boost::apply_visitor(*this, arg_ast.first);
            boost::apply_visitor(*this, arg_ast.rest.operand);
        }

        virtual void operator()(AST::Relational& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

        virtual void operator()(AST::_AndRel& arg_ast) {
            for (auto& elem : arg_ast) {
                boost::apply_visitor(*this, elem);
            }
        }

        virtual void operator()(AST::AndRel& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

        virtual void operator()(AST::_OrRel& arg_ast) {
            for (auto& elem : arg_ast) {
                boost::apply_visitor(*this, elem);
            }
        }

        virtual void operator()(AST::OrRel& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

        virtual void operator()(AST::_Substitute& arg_ast) {
            boost::apply_visitor(*this, arg_ast.val);
        }

        virtual void operator()(AST::Substitute& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

        virtual void operator()(AST::_FuncDef& arg_ast) {
            boost::apply_visitor(*this, arg_ast.expr);
        }

        virtual void operator()(AST::Expression& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

    };

    class ChangeConstName : public DoNothing {
    protected:
        const std::unordered_map<std::string, std::string>& _rule;
    public:

        ChangeConstName(const std::unordered_map<std::string, std::string>& arg_rule) : DoNothing(), _rule(arg_rule) {
        }
        using DoNothing::operator();

        void operator()(AST::_Constant& arg_ast) override {
            auto it_pos = _rule.find(arg_ast);
            if (it_pos != _rule.end()) {
                arg_ast = it_pos->second;
            }
        }
    };

    class PrintAST {
    protected:
        std::ostream& _out;
        int _position;

        void _startBr() {
            _out << std::endl;
            _position++;
            _out << std::string(_position * 2, ' ');
        }

        void _endBr() {
            _out << std::endl;
            _position--;
            _out << std::string(_position * 2, ' ');
        }
    public:

        PrintAST(std::ostream& arg_out) : _out(arg_out), _position(0) {
        }

        void operator()(const double& arg_ast) {
            _out << arg_ast;
        }

        void operator()(const AST::_Constant& arg_ast) {
            _out << '"' << arg_ast << '"';
        }


        void operator()(const AST::_If& arg_ast);

        void operator()(const AST::_FuncCall& arg_ast);

        void operator()(const AST::Primary& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

        void operator()(const AST::_Signed& arg_ast);

        void operator()(const AST::Signed& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

        void operator()(const AST::_PowerUnary& arg_ast);

        void operator()(const AST::_PowerInt& arg_ast);

        void operator()(const AST::Power& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

        void operator()(const AST::_Times& arg_ast);

        void operator()(const AST::Times& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

        void operator()(const AST::_Plus& arg_ast);

        void operator()(const AST::Plus& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

        void operator()(const AST::_Relational& arg_ast);

        void operator()(const AST::Relational& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

        void operator()(const AST::_AndRel& arg_ast);

        void operator()(const AST::AndRel& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

        void operator()(const AST::_OrRel& arg_ast);

        void operator()(const AST::OrRel& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

        void operator()(const AST::_Substitute& arg_ast) {
            for (const auto& elem : arg_ast.cName) {
                _out << "\"" << elem << "\" =" << std::endl;
            }
            boost::apply_visitor(*this, arg_ast.val);
        }

        void operator()(const AST::Substitute& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

        void operator()(const AST::_FuncDef& arg_ast);

        void operator()(const AST::Expression& arg_ast) {
            boost::apply_visitor(*this, arg_ast);
        }

    };

    class Evaluator {
    protected:
        std::unordered_map<std::string, double> _constants;
        std::unordered_map<std::string, std::pair<size_t, std::function<double(const std::vector<double>& arg_x)> >> _functions;

        static double _sqrt(const std::vector<double>& arg_x) {
            return sqrt(arg_x.front());
        };

        static double _max(const std::vector<double>& arg_x) {
            return *std::max_element(arg_x.begin(), arg_x.end());
        };

        static double _min(const std::vector<double>& arg_x) {
            return *std::min_element(arg_x.begin(), arg_x.end());
        };

        static double _pow(const std::vector<double>& arg_x) {
            return pow(arg_x.at(0), arg_x.at(1));
        };

        static double _exp(const std::vector<double>& arg_x) {
            return exp(arg_x.front());
        };

        static double _log(const std::vector<double>& arg_x) {
            return log(arg_x.front());
        };

        static double _log10(const std::vector<double>& arg_x) {
            return log10(arg_x.front());
        };

        static double _sin(const std::vector<double>& arg_x) {
            return sin(arg_x.front());
        };

        static double _cos(const std::vector<double>& arg_x) {
            return cos(arg_x.front());
        };

        static double _tan(const std::vector<double>& arg_x) {
            return tan(arg_x.front());
        };

        static double _abs(const std::vector<double>& arg_x) {
            return fabs(arg_x.front());
        };

        static double _asin(const std::vector<double>& arg_x) {
            return asin(arg_x.front());
        };

        static double _acos(const std::vector<double>& arg_x) {
            return acos(arg_x.front());
        };

        static double _atan(const std::vector<double>& arg_x) {
            return atan(arg_x.front());
        };

        static double _eval(const std::vector<double>& arg_x) {
            return arg_x.back();
        };

        static double _exit(const std::vector<double>& arg_x) {
            exit(0);
            return 0.;
        }
    public:

        Evaluator();

        void setConst(const std::string& arg_name, const double& arg_val) {
            _constants[arg_name] = arg_val;
        }

        void setFunc(const std::string& arg_name, const int& arg_argNum, const std::function<double(const std::vector<double>& arg_x)>& arg_func) {
            _functions[arg_name] = std::pair<int, std::function<double(const std::vector<double>)>>(arg_argNum, arg_func);
        }

        void eraseConst(const std::string& arg_name) {
            _constants.erase(arg_name);
        }

        void eraseFunc(const std::string& arg_name) {
            _functions.erase(arg_name);
        }

        const double& getConst(const std::string& arg_name) const {
            return _constants.at(arg_name);
        }

        double operator()(const double& arg_ast) {
            return arg_ast;
        }

        double operator()(const AST::_Constant& arg_ast);

        double operator()(AST::_If& arg_ast);

        double operator()(AST::_FuncCall& arg_ast);

        double operator()(AST::Primary& arg_ast) {
            return boost::apply_visitor(*this, arg_ast);
        }

        double operator()(AST::_Signed & arg_ast) {
            double temp = boost::apply_visitor(*this, arg_ast.primary);
            return arg_ast.sign * temp;
        }

        double operator()(AST::Signed & arg_ast) {
            return boost::apply_visitor(*this, arg_ast);
        }

        double operator()(AST::_PowerUnary & arg_ast) {
            return pow(boost::apply_visitor(*this, arg_ast.base), boost::apply_visitor(*this, arg_ast.uexp));
        }

        double operator()(AST::_PowerInt & arg_ast) {
            return NTools::powInt(boost::apply_visitor(*this, arg_ast.base), arg_ast.iexp);
        }

        double operator()(AST::Power & arg_ast) {
            return boost::apply_visitor(*this, arg_ast);
        }

        double operator()(AST::_Times & arg_ast);

        double operator()(AST::Times & arg_ast) {
            return boost::apply_visitor(*this, arg_ast);
        }

        double operator()(AST::_Plus & arg_ast);

        double operator()(AST::Plus & arg_ast) {
            return boost::apply_visitor(*this, arg_ast);
        }

        double operator()(AST::_Relational & arg_ast);

        double operator()(AST::Relational & arg_ast) {
            return boost::apply_visitor(*this, arg_ast);
        }

        double operator()(AST::_AndRel & arg_ast);

        double operator()(AST::AndRel & arg_ast) {
            return boost::apply_visitor(*this, arg_ast);
        }

        double operator()(AST::_OrRel & arg_ast);

        double operator()(AST::OrRel & arg_ast) {
            return boost::apply_visitor(*this, arg_ast);
        }

        double operator()(AST::_Substitute& arg_ast);

        double operator()(AST::Substitute & arg_ast) {
            return boost::apply_visitor(*this, arg_ast);
        }

        double operator()(AST::_FuncDef& arg_ast);

        double operator()(AST::Expression& arg_ast) {
            return boost::apply_visitor(*this, arg_ast);
        }

    };

}

#endif /* EVALUATOR_H */

