/**
 * @file ast.h
 * @brief Abstract Syntax Tree
 * @author Yutaro Shoji (ICRR, the University of Tokyo)
 * @date Created on: 2018/02/13, 19:14
 */

#ifndef AST_H
#define AST_H

#include <vector>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/fusion/include/vector.hpp>

namespace AST {
    namespace x3 = boost::spirit::x3;
    using _Constant = std::string;
    struct _If;
    struct _FuncCall;
    struct Primary;
    struct _Signed;
    struct Signed;
    struct _PowerUnary;
    struct _PowerInt;
    struct Power;
    struct _TimesOp;
    struct _Times;
    struct Times;
    using _Plus = std::vector<Times>;
    struct Plus;
    struct _RelOp;
    struct _Relational;
    struct Relational;
    using _AndRel = std::vector<Relational>;
    struct AndRel;
    using _OrRel = std::vector<AndRel>;
    struct OrRel;
    struct _Substitute;
    struct Substitute;
    struct _FuncName;
    struct _FuncDef;
    struct Expression;

    using _Recursion = Substitute;

    struct _FuncCall : x3::position_tagged {
        std::function<double(const std::vector<double>&) > function;
        std::string funcName;
        std::vector<_Recursion> arguments;
    };

    struct Primary : x3::variant<double, x3::forward_ast<_Constant>, x3::forward_ast<_FuncCall>, x3::forward_ast<_Recursion>, x3::forward_ast<_If>>
    {
        using base_type::base_type;
        using base_type::operator=;
    };

    struct _Signed : x3::position_tagged {
        int sign;
        Primary primary;
    };

    struct Signed : x3::variant<x3::forward_ast<_Signed>, x3::forward_ast<Primary>>
    {
        using base_type::base_type;
        using base_type::operator=;
    };

    struct _PowerUnary : x3::position_tagged {
        Signed base, uexp;
    };

    struct _PowerInt : x3::position_tagged {
        Signed base;
        int iexp;
    };

    struct Power : x3::variant<x3::forward_ast<_PowerUnary>, x3::forward_ast<_PowerInt>, x3::forward_ast<Signed>>
    {
        using base_type::base_type;
        using base_type::operator=;
    };

    struct _TimesOp : x3::position_tagged {
        bool invert;
        Power operand;
    };

    struct _Times : x3::position_tagged {
        Power first;
        std::vector<_TimesOp> rest;
    };

    struct Times : x3::variant<x3::forward_ast<_Times>, x3::forward_ast<Power>>
    {
        using base_type::base_type;
        using base_type::operator=;
    };

    struct Plus : x3::variant<x3::forward_ast<_Plus>, x3::forward_ast<Times>>
    {
        using base_type::base_type;
        using base_type::operator=;
    };

    struct _RelOp : x3::position_tagged {
        std::string operation;
        Plus operand;

    };

    struct _Relational : x3::position_tagged {
        Plus first;
        _RelOp rest;
    };

    struct Relational : x3::variant<x3::forward_ast<_Relational>, x3::forward_ast<Plus>>
    {
        using base_type::base_type;
        using base_type::operator=;
    };

    struct AndRel : x3::variant<x3::forward_ast<_AndRel>, x3::forward_ast<Relational>>
    {
        using base_type::base_type;
        using base_type::operator=;
    };

    struct OrRel : x3::variant<x3::forward_ast<_OrRel>, x3::forward_ast<AndRel>>
    {
        using base_type::base_type;
        using base_type::operator=;
    };

    struct _Substitute : x3::position_tagged {
        std::vector<std::string> cName;
        OrRel val;
    };

    struct Substitute : x3::variant<x3::forward_ast<_Substitute>, x3::forward_ast<OrRel>>
    {
        using base_type::base_type;
        using base_type::operator=;
    };

    struct _FuncName : x3::position_tagged {
        std::string fName;
        std::vector<std::string> fArgs;
    };

    struct _FuncDef : x3::position_tagged {
        _FuncName fName;
        Substitute expr;
    };

    struct Expression : x3::variant<x3::forward_ast<_FuncDef>, x3::forward_ast<Substitute>>
    {
        using base_type::base_type;
        using base_type::operator=;
    };

    struct _If : x3::position_tagged {
        _Recursion test;
        std::vector<_Recursion> arguments;
    };
}

BOOST_FUSION_ADAPT_STRUCT(AST::_If, (AST::_Recursion, test)(std::vector<AST::_Recursion>, arguments))
BOOST_FUSION_ADAPT_STRUCT(AST::_FuncCall, (std::string, funcName)(std::vector<AST::_Recursion>, arguments))
BOOST_FUSION_ADAPT_STRUCT(AST::_Signed, (int, sign)(AST::Primary, primary))
BOOST_FUSION_ADAPT_STRUCT(AST::_PowerUnary, (AST::Signed, base)(AST::Signed, uexp))
BOOST_FUSION_ADAPT_STRUCT(AST::_PowerInt, (AST::Signed, base)(int, iexp))
BOOST_FUSION_ADAPT_STRUCT(AST::_TimesOp, (bool, invert)(AST::Power, operand))
BOOST_FUSION_ADAPT_STRUCT(AST::_Times, (AST::Power, first)(std::vector<AST::_TimesOp>, rest))
BOOST_FUSION_ADAPT_STRUCT(AST::_RelOp, (std::string, operation)(AST::Plus, operand))
BOOST_FUSION_ADAPT_STRUCT(AST::_Relational, (AST::Plus, first)(AST::_RelOp, rest))
BOOST_FUSION_ADAPT_STRUCT(AST::_Substitute, (std::vector<std::string>, cName)(AST::OrRel, val))
BOOST_FUSION_ADAPT_STRUCT(AST::_FuncName, (std::string, fName)(std::vector<std::string>, fArgs))
BOOST_FUSION_ADAPT_STRUCT(AST::_FuncDef, (AST::_FuncName, fName)(AST::Substitute, expr))

#endif /* AST_H */

