// https://en.wikipedia.org/wiki/Quoted-printable
#include <iostream>
#include <sstream>

std::string decodeQuotedPrinted(std::string const& encoded) {
    std::string out;
    for (std::size_t i = 0; i < encoded.length(); i++) {
        char ch = encoded.at(i);

        if (ch != '=') {
            out += ch;
            continue;
        }

        auto nextChar = encoded.at(i + 1);
        if (nextChar == '\n') {
            i += 1;
            continue;
        }

        std::string next2Chars = encoded.substr(i+1, 2);
        unsigned int x;
        std::stringstream ss;
        ss << std::hex << next2Chars;
        ss >> x;
        out += x;

        i += 2;
    }

    return out;
}


std::string strToLower(std::string const& str) {
    std::string strCopy {};
    for (std::size_t i = 0; i < str.length(); i++) {
        strCopy += std::tolower(str.at(i));
    }
    return strCopy;
}

bool startsWith(std::string const& line, std::string const& prefix) {
    return line.substr(0, prefix.length()) == prefix;
}

std::string trimPrefix(std::string const& line, std::string const& prefix) {
    return line.substr(prefix.length());
}

std::string trimSuffix(std::string const& line, std::string const& suffix) {
    std::size_t beforeSuffixLen = line.length() - suffix.length();

    std::string lastXChars = line.substr(beforeSuffixLen);

    if (suffix != lastXChars) {
        // no match, return original
        return line;
    }

    return line.substr(0, beforeSuffixLen);
}
