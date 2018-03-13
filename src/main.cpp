/**
 * @file main.cpp
 * @brief ELVAS main
 * @author Yutaro Shoji (ICRR, the University of Tokyo)
 * @date Created on: 2018/02/01, 13:07
 */

#include "include/version.h"
#include "include/elvas_script.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <boost/program_options.hpp>

using namespace std;

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    string usage =
            "=======I/O=======: Usage\n"
            "   (stdin/stdout): ./elvas\n"
            "  (filein/stdout): ./elvas [INPUT1] [INPUT2] ...\n"
            "  (stdin/fileout): ./elvas -o [OUTPUT]\n"
            " (filein/fileout): ./elvas -o [OUTPUT] [INPUT1] [INPUT2] ...\n"
            "For details, see the attached manual.\n\n"
            "Allowed options";
    po::options_description desc(usage);
    desc.add_options()
            ("output,o", po::value<string>(), "output file")
            ("help,h", "display this help message")
            ("version,v", "output version information")
            ("no_header,n", "disable header printing");

    po::options_description hidden;
    hidden.add_options()
            ("input", po::value<vector < string >> (), "input file")
            ("debug", "debug mode");

    po::positional_options_description pdesc;
    pdesc.add("input", -1);

    po::options_description descAll;
    descAll.add(desc).add(hidden);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(descAll).positional(pdesc).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return 0;
    }
    if (vm.count("version")) {
        cout << "ELVAS v" << ELVAS_VERSION_MAJOR << "." << ELVAS_VERSION_MINOR << endl;
        return 0;
    }

    if (!vm.count("no_header")) {
        Elvas::printHeader(std::cout);
        if ((!vm.count("input"))&&(!vm.count("output"))) {
            cout << "======USING STANDARD IN/OUT======" << endl;
            cout << "use \"-n\" to suppress this message" << endl << endl;
        }
    }

    if (vm.count("debug")) {
        cout << "!!!!!DEBUG MODE!!!!!" << endl;
        ElvasScript elvas(std::cin, std::cout);
        elvas.setOutputDelim(", ");
        elvas.interactive();
        return 0;
    }

    stringstream ss;
    ofstream ofs;
    if (vm.count("input")) {
        for (const auto& in : vm["input"].as<vector < string >> ()) {
            ifstream ifs(in);
            if (ifs) {
                ss << ifs.rdbuf();
            } else {
                throw runtime_error("File open error. (" + in + ")");
            }
        }
    }
    if (vm.count("output")) {
        ofs.open(vm["output"].as<string>());
        if (!ofs) {
            throw runtime_error("File open error. (" + vm["output"].as<string>() + ")");
        }
    }

    if (vm.count("input") && vm.count("output")) {
        ElvasScript elvas(ss, ofs);
        elvas.analyze();
    } else if ((!vm.count("input")) && vm.count("output")) {
        ElvasScript elvas(std::cin, ofs);
        elvas.analyze();
    } else if (vm.count("input")&&(!vm.count("output"))) {
        ElvasScript elvas(ss, std::cout);
        elvas.analyze();
    } else {
        ElvasScript elvas(std::cin, std::cout);
        elvas.analyze();
    }

    return 0;
}


