/* This file contains auxiliary functions that display information about the
 * compiler and the system */

/// @brief Displays the LDPL version information message.
void displayVersionInfo()
{
    cout << endl
         << " This is \033[32;1mLDPL version " << VERSION << "\033[0m '\033[32;1m"
         << VERSIONNAME << "\033[0m'. Rawrrr!!"
         << endl
         << endl
         << " Copyright 2018-2025, \033[35;1mLartu\033[0m (www.lartu.net)."
         << endl
         << " Built with amazing contributions from \033[35;1mChris West\033[0m, "
            "\033[35;1mDamián Garro\033[0m,"
         << endl
         << " and other wonderful contributors!"
         << endl
         << endl
         << " The LDPL Home Page can be found at "
            "\033[36;1mwww.ldpl-lang.org\033[0m."
         << endl
         << " The documentation for LDPL should have come with your LDPL download. "
         << endl
         << " If you have access to the internet, the documentation can also be "
         << endl
         << " found online at \033[36;1mdocs.ldpl-lang.org\033[0m."
         << endl
         << endl
         << " LDPL may be copied only under the terms of the Apache License"
         << endl
         << " Version 2.0, which may be found in the LDPL repository." << endl
         << endl
         << " This binary was compiled on \033[31;1m" << COMPILEDATE
         << "\033[0m at \033[31;1m" << COMPILEHOUR << "\033[0m." << endl
         << endl;
}

/// @brief Displays the LDPL help information message.
void displayHelpInfo()
{
    cout << endl;
    cout << " \033[33;1mUsage:\033[0m" << endl;
    cout << "    ldpl [-i='<included file>']... <source file>|-c" << endl;
    cout << "         [-o='<output name>'|-r] [-f='<c++ flag>']... [-n]" << endl;
    cout << "    ldpl [-v|-h]" << endl;
    cout << endl;
    cout << " \033[33;1mOptions:\033[0m" << endl;
    cout << "   -v --version             Display LDPL version information"
         << endl;
    cout << "   -h --help                Display this information" << endl;
    cout << "   -r                       Display generated C++ code" << endl;
    cout << "   -o=<name>                Set output file for compiled binary"
         << endl;
    cout << "   -i=<file>                Include file in current compilation"
         << endl;
    cout << "   -f=<flag>                Pass a flag to the C++ compiler" << endl;
#ifdef STATIC_BUILDS
    cout << "   -n --non-static          Disables static binary building" << endl;
#endif
    cout << "   -c                       Compile from standard input" << endl;
    cout << endl;
    cout << " Complete documentation for LDPL should be found on this system"
         << endl;
    cout << " using '\033[33;1mman ldpl\033[0m'. If you have access to the "
            "internet, the"
         << endl;
    cout << " documentation can also be found online at "
            "\033[36;1mdocs.ldpl-lang.org\033[0m."
         << endl;
    cout << endl;
}

/// @brief Prints a non-fatal warning message.
void warning(const string &msg)
{
    cerr << "\033[1;35mLDPL Warning: ";
    cerr << msg;
    cerr << "\033[0m" << endl;
}

/// @brief Prints a message preceded by a *
void bullet_msg(const string &msg)
{
    cerr << "\033[1;33m*\033[0m ";
    cerr << msg << endl;
}

/// @brief Shows where the error was in a code error.
void badcode(const string &msg, const code_location where)
{
    error(msg + " (\033[0m" + where.current_file + ":" + to_string(where.current_line) + "\033[1;31m)");
}

/// @brief Shows a fatal error message and exits.
void error(const string &msg)
{
    cerr << "\033[1;31mLDPL Error: ";
    cerr << msg;
    cerr << "\033[0m" << endl;
    exit(1);
}

/// @brief Returns the current operating system
string current_os()
{
    // https://sourceforge.net/p/predef/wiki/OperatingSystems/
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