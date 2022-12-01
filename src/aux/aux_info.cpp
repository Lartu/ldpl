/* This file contains auxiliary functions that display information about the compiler and the system */

void displayVersionInfo()
{
    // -- Displays LDPL Version Information -- 
    cout << endl;
    cout << " This is \033[32;1mLDPL version " << VERSION << "\033[0m '\033[32;1m" << VERSIONNAME << "\033[0m'." << endl;
    cout << endl;
    cout << " Copyright 2018-2022, \033[35;1mLartu\033[0m (www.lartu.net)." << endl;
    cout << " Built with amazing contributions from \033[35;1mChris West\033[0m, \033[35;1mDamián Garro\033[0m," << endl;
    cout << " \033[35;1mIgnacio Losiggio\033[0m and other wonderful contributors." << endl;
    cout << endl;
    cout << " The LDPL Home Page can be found at \033[36;1mwww.ldpl-lang.org\033[0m." << endl;
    cout << " The LDPL source code is available at \033[36;1mwww.github.com/lartu/ldpl\033[0m." << endl;
    cout << endl;
    cout << " Complete documentation for LDPL should be found on this system" << endl;
    cout << " using '\033[33;1mman ldpl\033[0m'. If you have access to the internet, the" << endl;
    cout << " documentation can also be found online at \033[36;1mdocs.ldpl-lang.org\033[0m." << endl;
    cout << endl;
    cout << " LDPL may be copied only under the terms of the Apache License" << endl;
    cout << " Version 2.0, which may be found in the LDPL repository." << endl;
    cout << endl;
    cout << " This binary was compiled on \033[31;1m" << COMPILEDATE << "\033[0m at \033[31;1m" << COMPILEHOUR << "\033[0m." << endl;
    cout << endl;
}

void displayHelpInfo()
{
    // -- Displays LDPL Help Information -- 
    cout << endl;
    cout << " \033[33;1mUsage:\033[0m" << endl;
    cout << "    ldpl [-i='<included file>']... <source file>|-c" << endl;
    cout << "         [-o='<output name>'|-r] [-f='<c++ flag>']... [-n]" << endl;
    cout << "    ldpl [-v|-h]" << endl;
    cout << endl;
    cout << " \033[33;1mOptions:\033[0m" << endl;
    cout << "   -v --version             Display LDPL version information" << endl;
    cout << "   -h --help                Display this information" << endl;
    cout << "   -r                       Display generated C++ code" << endl;
    cout << "   -o=<name>                Set output file for compiled binary" << endl;
    cout << "   -i=<file>                Include file in current compilation" << endl;
    cout << "   -f=<flag>                Pass a flag to the C++ compiler" << endl;
#ifdef STATIC_BUILDS
    cout << "   -n --non-static          Disables static binary building" << endl;
#endif
    cout << "   -c                       Compile from standard input" << endl;
    cout << endl;
    cout << " Complete documentation for LDPL should be found on this system" << endl;
    cout << " using '\033[33;1mman ldpl\033[0m'. If you have access to the internet, the" << endl;
    cout << " documentation can also be found online at \033[36;1mdocs.ldpl-lang.org\033[0m." << endl;
    cout << endl;
}

void warning(const string &msg)
{
    // -- Shows a warning message --
    cerr << "\033[1;35mLDPL Warning: ";
    cerr << msg;
    cerr << "\033[0m" << endl;
}

void bullet_msg(const string &msg)
{
    // -- Shows message preceded by a * --
    cerr << "\033[1;33m*\033[0m ";
    cerr << msg << endl;
}

void badcode(const string &msg, const code_location where)
{
    // -- Shows where the error was in a code error --
    error(msg + " (\033[0m" + where.current_file + ":" + to_string(where.current_line) + "\033[1;31m)");
}

void error(const string &msg)
{
    // -- Shows a fatal error message --
    cerr << "\033[1;31mLDPL Error: ";
    cerr << msg;
    cerr << "\033[0m" << endl;
    exit(1);
}

string current_os()
{
    //-- Returns the current operating system --
    // https:// sourceforge.net/p/predef/wiki/OperatingSystems/
#if defined(__linux__)
    return "LINUX";
#elif defined(__APPLE__)
    return "MACOS";
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
    return "BSD";
#elif defined(__ANDROID__)
    return "ANDROID";
#elif defined(__EMSCRIPTEN__)
    return "EMSCRIPTEN";
#else
    return "UNKNOWN";
#endif
}