/**
 * @file parser.h
 * @brief X3 parser
 * @author Yutaro Shoji (ICRR, the University of Tokyo)
 * @date Created on: 2018/02/06, 19:14
 */

#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include <boost/spirit/home/x3.hpp>

namespace Parser {

    namespace x3 = boost::spirit::x3;

    const struct _PluMin : x3::symbols<int> {

        _PluMin() {
            add("+", 1)("-", -1);
        }
    } pm;

    const struct _MulDiv : x3::symbols<bool> {

        _MulDiv() {
            add("*", false)("/", true);
        }
    } md;

    struct _IdentName_class;
    struct _ConstExpr_class;
    struct _IfExpr_class;
    struct _FuncCallExpr_class;
    struct PrimaryExpr_class;
    struct UnaryExpr_class;
    struct _PowerUExpr_class;
    struct _PowerIExpr_class;
    struct PowerExpr_class;
    struct TimesExpr_class;
    struct PlusExpr_class;
    struct _RelOpExpr_class;
    struct RelExpr_class;
    struct AndExpr_class;
    struct OrExpr_class;
    struct _RecurExpr_class;
    struct _SubstExpr_class;
    struct SubstExpr_class;
    struct _FuncNameExpr_class;
    struct _FuncDefExpr_class;
    struct Expression_class;

    typedef x3::rule<_IdentName_class, std::string> _IdentName_type;
    typedef x3::rule<_ConstExpr_class, AST::_Constant> _ConstExpr_type;
    typedef x3::rule<_IfExpr_class, AST::_If> _IfExpr_type;
    typedef x3::rule<_FuncCallExpr_class, AST::_FuncCall> _FuncCallExpr_type;
    typedef x3::rule<PrimaryExpr_class, AST::Primary> PrimaryExpr_type;
    typedef x3::rule<UnaryExpr_class, AST::Signed> UnaryExpr_type;
    typedef x3::rule<PowerExpr_class, AST::Power> PowerExpr_type;
    typedef x3::rule<TimesExpr_class, AST::Times> TimesExpr_type;
    typedef x3::rule<PlusExpr_class, AST::Plus> PlusExpr_type;
    typedef x3::rule<_RelOpExpr_class, AST::_RelOp> _RelOpExpr_type;
    typedef x3::rule<RelExpr_class, AST::Relational> RelExpr_type;
    typedef x3::rule<AndExpr_class, AST::AndRel> AndExpr_type;
    typedef x3::rule<OrExpr_class, AST::OrRel> OrExpr_type;
    typedef x3::rule<_SubstExpr_class, AST::_Substitute> _SubstExpr_type;
    typedef x3::rule<SubstExpr_class, AST::Substitute> SubstExpr_type;
    typedef x3::rule<_RecurExpr_class, AST::_Recursion> _RecurExpr_type;
    typedef x3::rule<_FuncNameExpr_class, AST::_FuncName> _FuncNameExpr_type;
    typedef x3::rule<_FuncDefExpr_class, AST::_FuncDef> _FuncDefExpr_type;
    typedef x3::rule<Expression_class, AST::Expression> Expression_type;

    const _IdentName_type _IdentName = "Name";
    const _ConstExpr_type _ConstExpr = "Constant";
    const _IfExpr_type _IfExpr = "If Directive";
    const _FuncCallExpr_type _FuncCallExpr = "Function Call";
    const PrimaryExpr_type PrimaryExpr = "Expression";
    const UnaryExpr_type UnaryExpr = "Expression";
    const PowerExpr_type PowerExpr = "Expression";
    const TimesExpr_type TimesExpr = "Expression";
    const PlusExpr_type PlusExpr = "Expression";
    const _RelOpExpr_type _RelOpExpr = "Relational Operator";
    const RelExpr_type RelExpr = "Expression";
    const AndExpr_type AndExpr = "Expression";
    const OrExpr_type OrExpr = "Expression";
    const _SubstExpr_type _SubstExpr = "Expression";
    const SubstExpr_type SubstExpr = "Expression";
    const _RecurExpr_type _RecurExpr = "Expression";
    const _FuncNameExpr_type _FuncNameExpr = "Function Name";
    const _FuncDefExpr_type _FuncDefExpr = "Function Definition";
    const Expression_type Expression = "Expression";


    const auto _IdentName_def = x3::lexeme[x3::raw[(x3::alpha | x3::char_('_')) >> *(x3::alnum | x3::char_('_'))]];
    const auto _ConstExpr_def = (_IdentName >> !x3::char_("(")) - (x3::lit("inf") >> !(x3::alnum | x3::char_('_')));
    const auto _IfExpr_def = x3::lit("if") >> '(' > _RecurExpr > +(',' > _RecurExpr) > ')';
    const auto _FuncCallExpr_def = (_IdentName - x3::lit("if")) >> '(' >> -(_RecurExpr % ',') > ')';
    const auto PrimaryExpr_def = (_ConstExpr | _IfExpr | _FuncCallExpr | x3::double_ | '(' > _RecurExpr > ')') > !(x3::alnum | x3::char_("(_."));
    const auto UnaryExpr_def = PrimaryExpr | (pm > PrimaryExpr);
    const auto PowerExpr_def = (UnaryExpr >> !x3::char_('^')) | (UnaryExpr >> '^' >> x3::int_ >> !x3::lit('.')) | (UnaryExpr >> '^' > UnaryExpr);
    const auto TimesExpr_def = (PowerExpr >> !x3::char_("*/")) | (PowerExpr > +(md > PowerExpr));
    const auto PlusExpr_def = (TimesExpr >> !TimesExpr) | x3::repeat(2, x3::inf)[TimesExpr];
    const auto _RelOpExpr_def = (x3::string("<=") | x3::string(">=") | x3::string("==") | x3::string("!=") | x3::string("<") | x3::string(">")) > PlusExpr;
    const auto RelExpr_def = (PlusExpr >> !(x3::char_("<!>") | x3::lit("=="))) | (PlusExpr > _RelOpExpr);
    const auto AndExpr_def = (RelExpr >> !x3::char_('&')) | (RelExpr % '&');
    const auto OrExpr_def = (AndExpr >> !x3::char_('|')) | (AndExpr % '|');
    const auto _SubstExpr_def = +(_IdentName >> '=') > OrExpr;
    const auto SubstExpr_def = _SubstExpr | (OrExpr > !x3::char_('='));
    const auto _RecurExpr_def = SubstExpr;
    const auto _FuncNameExpr_def = _IdentName >> -('(' >> -(_IdentName % ',') >> ')');
    const auto _FuncDefExpr_def = _FuncNameExpr >> x3::lit(":=") > SubstExpr;
    const auto Expression_def = (_FuncDefExpr | (SubstExpr > !x3::char_(':'))) >> !x3::char_;

    BOOST_SPIRIT_DEFINE(_IdentName, _ConstExpr, _IfExpr, _FuncCallExpr, PrimaryExpr, UnaryExpr, PowerExpr,
            TimesExpr, PlusExpr, _RelOpExpr, RelExpr, AndExpr, OrExpr, _SubstExpr, SubstExpr,
            _RecurExpr, _FuncNameExpr, _FuncDefExpr, Expression)

}

#endif /* PARSER_H */

