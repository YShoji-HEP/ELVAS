/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "include/interpreter.h"

void Interpreter::_executeAST(std::vector<AST::Expression>& arg_asts) {
    for (auto& ast : arg_asts) {
        _eval(ast);
        if (_continue) {
            _continue = false;
            break;
        }
    }
    if (_break) {
        _break = false;
        _section = 'N';
    }
}

void Interpreter::_beginFunc(const std::vector<std::string>& arg_varNames, const std::vector<double>& arg_secVals) {
    int i = 0;
    for (const auto& elem : arg_secVals) {
        setConst(arg_varNames.at(i), elem);
        i++;
    }
    _executeAST(_begRoutine);
}

void Interpreter::_mainFunc(const std::vector<std::string>& arg_varNames, const std::vector<double>& arg_recordVals) {
    int i = 0;
    for (const auto& elem : arg_recordVals) {
        setConst(arg_varNames.at(i), elem);
        i++;
    }
    _executeAST(_mainRoutine);
}

template<class DataType>
void Interpreter::_getData(std::unordered_map<std::string, DataType>& arg_map, const std::string& arg_name, DataType& arg_result) {
    if (arg_result.size() == 0) {
        auto it_names = arg_map.find(arg_name);
        if (it_names != arg_map.end()) {
            arg_result = std::move(it_names->second);
        } else {
            throw InterpreterError(arg_name + " is not set.");
        }
    }
}

template void Interpreter::_getData(std::unordered_map<std::string, std::string>& arg_map, const std::string& arg_name, std::string& arg_result);
template void Interpreter::_getData(std::unordered_map<std::string, std::vector<std::string>>&arg_map, const std::string& arg_name, std::vector<std::string>& arg_result);

bool Interpreter::_readGenSec(const std::string& arg_buf) {
    namespace x3 = boost::spirit::x3;
    auto name = x3::raw[x3::lexeme[(x3::alpha | x3::char_('_')) >> *(x3::alnum | x3::char_('_'))]];
    auto listF = name >> '=' >> '{' >> -(name % ",") > '}' >> !x3::char_;
    auto strF = name >> '=' >> x3::lexeme['"' >> *(~x3::char_("\"#")) > '"'] >> !x3::char_;

    std::vector<std::string> list, str;
    if (x3::phrase_parse(arg_buf.begin(), arg_buf.end(), strF, x3::ascii::space, str)) {
        _strings.emplace(str.at(0), str.at(1));
        return true;
    } else if (x3::phrase_parse(arg_buf.begin(), arg_buf.end(), listF, x3::ascii::space, list)) {
        std::string key = list.at(0);
        list.erase(list.begin());
        _lists.emplace(key, list);
        return true;
    }
    return false;
}

bool Interpreter::_readInitSec(const std::string& arg_buf) {
    namespace x3 = boost::spirit::x3;
    auto eqF = x3::raw[*(x3::alnum | x3::char_("!:_&()=^|+*,.<>/-"))] >> !x3::char_;
    auto printStrF = x3::lit("print") >> '(' >> x3::lexeme['"' >> *(~x3::char_('"')) > '"'] > ')' >> !x3::char_;

    std::string eq, printStr;
    if (x3::phrase_parse(arg_buf.begin(), arg_buf.end(), eqF, x3::ascii::space, eq)) {
        AST::Expression ast;
        try {
            if (x3::phrase_parse(eq.begin(), eq.end(), Parser::Expression, x3::ascii::space, ast)) {
                _eval(ast);
                if (_continue || _break) {
                    _continue = false;
                    _break = false;
                    _section = 'N';
                }
                return true;
            }
        } catch (const x3::expectation_failure<std::string::iterator>& arg_e) {
            throw InterpreterError(arg_e, eq);
        }
    } else if (x3::phrase_parse(arg_buf.begin(), arg_buf.end(), printStrF, x3::ascii::space, printStr)) {
        _os << printStr << std::endl;
        return true;
    }
    return false;
}

bool Interpreter::_readDataSec(const std::string& arg_buf) {
    namespace x3 = boost::spirit::x3;
    auto sp = x3::omit[*x3::ascii::space];

    _getData(_strings, "RECORD_DELIM", _recordDelim);
    auto recordF = (x3::double_ % _recordDelim) >> sp >> !x3::char_;
    std::vector<double> recordVals;
    _getData(_lists, "RECORD_VARS", _recordVarNames);
    if (x3::parse(arg_buf.begin(), arg_buf.end(), recordF, recordVals)) {
        if (recordVals.size() == _recordVarNames.size()) {
            _mainFunc(_recordVarNames, recordVals);
        } else {
            throw InterpreterError("Data format error.");
        }
        return true;
    }
    return false;
}

bool Interpreter::_readOtherSec(const std::string& arg_buf) {
    namespace x3 = boost::spirit::x3;
    auto eqF = x3::raw[*(x3::alnum | x3::char_("!:_&()=^|+*,.<>/-"))] >> !x3::char_;
    auto printStrF = x3::lit("print") >> '(' >> x3::lexeme['"' >> *(~x3::char_('"')) > '"'] > ')' >> !x3::char_;

    std::string eq, printStr;
    if (x3::phrase_parse(arg_buf.begin(), arg_buf.end(), eqF, x3::ascii::space, eq)) {
        AST::Expression ast;
        try {
            if (x3::phrase_parse(eq.begin(), eq.end(), Parser::Expression, x3::ascii::space, ast)) {
                switch (_section) {
                    case 'B':
                        _begRoutine.emplace_back(ast);
                        return true;
                    case 'M':
                        _mainRoutine.emplace_back(ast);
                        return true;
                    case 'E':
                        _endRoutine.emplace_back(ast);
                        return true;
                    case 'F':
                        _finRoutine.emplace_back(ast);
                        return true;
                }
            }
        } catch (const x3::expectation_failure<std::string::iterator>& arg_e) {
            throw InterpreterError(arg_e, eq);
        }
    } else if (x3::phrase_parse(arg_buf.begin(), arg_buf.end(), printStrF, x3::ascii::space, printStr)) {
        _printStr.emplace_back(printStr);
        std::string temp = "print_str(" + std::to_string(_printStr.size() - .9) + ")";
        AST::Expression ast;
        x3::phrase_parse(temp.begin(), temp.end(), Parser::Expression, x3::ascii::space, ast);
        switch (_section) {
            case 'B':
                _begRoutine.emplace_back(ast);
                return true;
            case 'M':
                _mainRoutine.emplace_back(ast);
                return true;
            case 'E':
                _endRoutine.emplace_back(ast);
                return true;
            case 'F':
                _finRoutine.emplace_back(ast);
                return true;
        }
    }
    return false;
}

bool Interpreter::_readDatasetVar(const std::string & arg_secVar) {
    namespace x3 = boost::spirit::x3;

    if (arg_secVar.length() != 0) {
        _getData(_strings, "DATASET_DELIM", _datasetDelim);
        auto secVarF = x3::double_ % _datasetDelim;
        std::vector<double> secVars;
        _getData(_lists, "DATASET_VARS", _datasetVarNames);
        if (x3::parse(arg_secVar.begin(), arg_secVar.end(), secVarF, secVars)) {
            if (secVars.size() == _datasetVarNames.size()) {
                _beginFunc(_datasetVarNames, secVars);
                return true;
            } else {
                throw InterpreterError("Dataset values format error.");
            }
        }
    } else {
        _beginFunc(_datasetVarNames, std::vector<double>{});
        return true;
    }
    return false;
}

Interpreter::Interpreter(std::istream& arg_is, std::ostream & arg_os)
: _is(arg_is), _os(arg_os), _section('N'), _recordDelim(""), _datasetDelim(""), _outputDelim(""), _break(false), _continue(false) {

    auto printFunc = [ this ](const std::vector<double>& arg_x) {
        _getData(_strings, "OUTPUT_DELIM", _outputDelim);
        bool isFirst = true;
        for (const auto& elem : arg_x) {
            _os << (isFirst ? (isFirst = false, "") : _outputDelim) << elem;
        }
        _os << std::endl;
        return arg_x.back();
    };
    auto printStrFunc = [ this ](const std::vector<double>& arg_x) {
        _os << _printStr.at((int) (arg_x.front() + 0.5)) << std::endl;
        return 0.;
    };
    auto continueFunc = [ this ](const std::vector<double>& arg_x) {
        _continue = true;
        return 0.;
    };
    auto breakFunc = [ this ](const std::vector<double>& arg_x) {
        _continue = true;
        _break = true;
        return 0.;
    };
    setFunc("print", -1, printFunc);
    setFunc("print_str", 1, printStrFunc);
    setFunc("continue", 0, continueFunc);
    setFunc("break", 0, breakFunc);
}

void Interpreter::InterpreterError::errorMsg(std::ostream & arg_out) const {
    if (_isSet) {
        arg_out << "Wrong syntax: ";
        if (_expected.find("spirit") != _expected.npos) {
            arg_out << "Incomprete expression." << std::endl;
        } else {
            arg_out << _expected << " is expected here." << std::endl;
        }
        arg_out << _original << std::endl;
        arg_out << std::string(_pos, '_') << "^_" << std::endl;
    } else {
        arg_out << what() << std::endl;
    }
}

void Interpreter::analyze() {
    namespace x3 = boost::spirit::x3;

    auto sp = x3::omit[*x3::ascii::space];

    const struct SecNames : x3::symbols<char> {

        SecNames() {
            add("DATASET", 'D')("GENERAL", 'G')
                    ("INITIALIZE", 'I')("BEGIN_ROUTINE", 'B')
                    ("END_ROUTINE", 'E')("MAIN_ROUTINE", 'M')
                    ("FINALIZE", 'F');
        }
    } secNames;

    auto entryF = sp >> x3::raw[*(~x3::char_("#\\"))] >> '\\';
    auto secF = '[' >> sp >> secNames >> sp > ']' >> sp >> -('(' >> *(~x3::char_(')')) > ')') >> sp >> !x3::char_;

    std::string strBuf;
    int lineNum = 0;
    while (std::getline(_is, strBuf)) {
        lineNum++;
        std::string buf;
        try {
            while (x3::parse(strBuf.begin(), strBuf.end(), entryF, buf) && std::getline(_is, strBuf)) {
                lineNum++;
            }
            if (buf.size() == 0) {
                continue;
            }

            std::pair<char, std::string> secName;

            if (x3::parse(buf.begin(), buf.end(), secF, secName)) {
                if (_section == 'D') {
                    _endFunc();
                }
                if (secName.first == 'D') {
                    if (!_readDatasetVar(secName.second)) {
                        throw InterpreterError(strBuf);
                    }
                }
                _section = secName.first;
            } else if (_section == 'D' && _readDataSec(buf)) {
            } else if (_section == 'I' && _readInitSec(buf)) {
            } else if (_section == 'G' && _readGenSec(buf)) {
            } else if (_section != 'N' && _readOtherSec(buf)) {
            } else {
                throw InterpreterError(strBuf);
            }
        } catch (const InterpreterError& arg_e) {
            _os << "Wrong syntax in line " << lineNum << ":" << std::endl;
            arg_e.errorMsg(_os);
            throw arg_e;
        }
    }
    _endFunc();
    _finFunc();

};

void Interpreter::interactive() {
    namespace x3 = boost::spirit::x3;

    auto eqF = x3::raw[*(x3::alnum | x3::char_("!:_&()=^|+*,.<>/-"))] >> !x3::char_;
    auto printStrF = x3::lit("print") >> '(' >> x3::lexeme['"' >> *(~x3::char_('"')) > '"'] > ')' >> !x3::char_;

    ASTReader::PrintAST printast(_os);

    _os << "[in]: ";
    std::string strBuf;
    while (getline(_is, strBuf)) {
        std::string eq, printStr;
        try {
            if (strBuf.length() != 0) {
                if (x3::phrase_parse(strBuf.begin(), strBuf.end(), eqF, x3::ascii::space, eq)) {
                    AST::Expression ast;
                    try {
                        if (x3::phrase_parse(eq.begin(), eq.end(), Parser::Expression, x3::ascii::space, ast)) {
                            _os << "===AST===" << std::endl;
                            printast(ast);
                            _os << std::endl << "=========" << std::endl;
                            double result = _eval(ast);
                            _os << "[out]: " << result << std::endl << std::endl;
                        } else {
                            _os << "[Incomprete expression]: " << strBuf << std::endl;
                        }
                    } catch (const x3::expectation_failure<std::string::iterator>& arg_e) {
                        InterpreterError err(arg_e, eq);
                        err.errorMsg(_os);
                    }
                } else if (x3::phrase_parse(strBuf.begin(), strBuf.end(), printStrF, x3::ascii::space, printStr)) {
                    _os << printStr << std::endl;
                    _os << "[out]: " << 0. << std::endl << std::endl;
                } else {
                    _os << "[Incomprete expression]: " << strBuf << std::endl;
                }
                strBuf.clear();
            }
            _os << "[in]: ";
        } catch (const ASTReader::ASTReadError& arg_e) {
            _os << "[Error]: " << arg_e.what() << std::endl;
            _os << "[in]: ";
        }
    }
}