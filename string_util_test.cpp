#include <iostream>
#include "string_util.cpp"

void test_startsWith() {
    std::string str = "abcdef?=";
    std::string prefix = "abc";
    bool expected = true;
    bool actual = startsWith(str, prefix);

    if (expected != actual) {
        throw std::invalid_argument("expected and actual are different");
    }
}

void test_strToLower() {
    std::string str = "ABcdef?=";
    std::string expected = "abcdef?=";
    std::string actual = strToLower(str);

    if (expected != actual) {
        throw std::invalid_argument("expected '" + expected + "' but actual was: '" + actual + "'");
    }
}

void test_trimPrefix() {
    std::string str = "abcdef?=";
    std::string prefix = "abc";
    std::string expected = "def?=";
    std::string actual = trimPrefix(str, prefix);

    if (expected != actual) {
        throw std::invalid_argument("expected '" + expected + "' but actual was: '" + actual + "'");
    }
}

void test_trimSuffix() {
    std::string str = "abcdef?=";
    std::string suffix = "?=";
    std::string expected = "abcdef";
    std::string actual = trimSuffix(str, suffix);

    if (expected != actual) {
        throw std::invalid_argument("expected '" + expected + "' but actual was: '" + actual + "'");
    }
}

void test_decodeQuotedPrinted() {
    std::string expectedDecoded = "J'interdis aux marchands de vanter trop leurs marchandises. Car ils se font vite pédagogues et t'enseignent comme but ce qui n'est par essence qu'un moyen, et te trompant ainsi sur la route à suivre les voilà bientôt qui te dégradent, car si leur musique est vulgaire ils te fabriquent pour te la vendre une âme vulgaire.";

    std::string encoded = 
    "J'interdis aux marchands de vanter trop leurs marchandises. Car ils se font =\n"
    "vite p=C3=A9dagogues et t'enseignent comme but ce qui n'est par essence qu'=\n"
    "un moyen, et te trompant ainsi sur la route =C3=A0 suivre les voil=C3=A0 bi=\n"
    "ent=C3=B4t qui te d=C3=A9gradent, car si leur musique est vulgaire ils te f=\n"
    "abriquent pour te la vendre une =C3=A2me vulgaire.";

    auto decoded (decodeQuotedPrinted(encoded));

    if (decoded != expectedDecoded) {
        throw std::invalid_argument("expected '" + expectedDecoded + "' but actual was: '" + decoded + "'");
    }
}

int main() {
    test_decodeQuotedPrinted();
    test_trimSuffix();
    test_trimPrefix();
    test_strToLower();
    test_startsWith();
}
