#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include "thirdparty/sqlite3/sqlite3.h"
#include "thirdparty/cpp-base64/base64.h"
#include "string_util.cpp"

int callback(void *a_param, int argc, char **argv, char **column){
    (void)a_param;
    (void)column;
    for (int i=0; i< argc; i++)
        printf("%s,\t", argv[i]);
    printf("\n");
    return 0;
}

class Email {
    public:
        std::string from;
        std::string to;
        std::string date;
        std::string subject;
};

const std::string utf8EncodingPrefix = "=?utf-8?b?";
const std::string utf8quotedPrintableEncodingPrefix = "=?utf-8?q?";
const std::string iso8859QuotedPrintableEncodingPrefix = "=?iso-8859-1?q?";
const std::string encodingSuffix = "?=";

// https://stackoverflow.com/questions/4389676/email-from-php-has-broken-subject-header-encoding
std::string stripEncoding(std::string const& line) {
    // determine encoding
    if (startsWith(strToLower(line), utf8EncodingPrefix)) {
        std::string const& strToDecode = trimPrefix(trimSuffix(line, encodingSuffix), utf8EncodingPrefix);
        return base64_decode(strToDecode);
    } else if (startsWith(strToLower(line), utf8quotedPrintableEncodingPrefix)) {
        std::string const& strToDecode = trimPrefix(trimSuffix(line, encodingSuffix), utf8quotedPrintableEncodingPrefix);
        return decodeQuotedPrinted(strToDecode);
    } else if (startsWith(strToLower(line), iso8859QuotedPrintableEncodingPrefix)) {
        std::string const& strToDecode = trimPrefix(trimSuffix(line, encodingSuffix), iso8859QuotedPrintableEncodingPrefix);
        return decodeQuotedPrinted(strToDecode);
    }

    return line;
}

void checkRetval(sqlite3* db , int retVal, int expectedRetval, std::string suppliedMessage) {
    if (retVal != expectedRetval) {
        auto errMsg = sqlite3_errmsg(db);
        sqlite3_free(db);
        std::string message = suppliedMessage + ". SQLITE3 error: " + errMsg;
        throw std::invalid_argument(message);
    }
}

int main(int argc, char* argv[]) {
    const char* mboxFilePath(argv[1]);
    const char* dbFilePath(argv[2]);
    (void)argc;

    if (mboxFilePath == NULL) {
        std::cerr << "no mbox filePath specified" << std::endl;
        exit(1);
    }

    if (dbFilePath == NULL) {
        std::cerr << "no sqlite DB (output) filePath specified" << std::endl;
        exit(1);
    }

    std::cout << "fetching mails from mbox file at:" << mboxFilePath << std::endl;

    std::ifstream infile(mboxFilePath);
    
    std::string const& newMailPrefix = "From ";
    std::string const& fromPrefix = "From: ";
    std::string const& toPrefix = "To: ";
    std::string const& datePrefix = "Date: ";
    std::string const& subjectPrefix = "Subject: ";

    std::vector<Email> emails {};

    Email email {};
    std::string line;

    std::cerr << "starting reading emails" << std::endl;

    while (std::getline(infile, line)) {
        if (startsWith(line, newMailPrefix)) {
            if (email.from != "") {
                emails.push_back(email);
                email = {};
            }
        } else if (startsWith(line, fromPrefix)) {
            email.from = stripEncoding(trimPrefix(line, fromPrefix));
        } else if (startsWith(line, toPrefix)) {
            email.to = stripEncoding(trimPrefix(line, toPrefix));
        } else if (startsWith(line, subjectPrefix)) {
            email.subject = stripEncoding(trimPrefix(line, subjectPrefix));
        } else if (startsWith(line, datePrefix)) {
            email.date = stripEncoding(trimPrefix(line, datePrefix));
        }
    }

    std::cerr << "finished reading emails. Found " << emails.size() << " emails" << std::endl;

    sqlite3* db;
    int retVal = sqlite3_open(dbFilePath, &db);
    checkRetval(db, retVal, SQLITE_OK, "failed to open Sqlite DB at: " + std::string(dbFilePath));

    std::cerr << "creating table" << std::endl;

    constexpr const char* sql = "CREATE TABLE emails("
        "from_address TEXT NOT NULL,"
        "to_address TEXT NOT NULL,"
        "subject TEXT NOT NULL,"
        "date TEXT NOT NULL"
        ");";

    char* errMsg;

    retVal = sqlite3_exec(db, sql, callback, 0, &errMsg);
    checkRetval(db, retVal,SQLITE_OK, errMsg == NULL ? "no supplied message" : std::string(errMsg));

    std::cerr << "created table" << std::endl;

    auto insertSql = "INSERT INTO emails(from_address, to_address, subject, date)"
    "VALUES(?, ?, ?, ?)";

    sqlite3_stmt* insertStmt;
    retVal = sqlite3_prepare_v2(db, insertSql, -1, &insertStmt, 0);
    checkRetval(db, retVal, SQLITE_OK,"failed to prepare sql statement");

    std::cerr << "starting inserting emails" << std::endl;

    for (size_t i = 0; i < emails.size(); i++) {
        if (i % 200 == 0) {
            std::cerr << "inserted " << i << " so far..." << std::endl;
        }
        Email email = emails[i];

        // using with prepared statements: https://github.com/mapbox/node-sqlite3
        // https://stackoverflow.com/questions/26969752/get-the-value-from-the-sqlite3-stmt-when-calling-the-sqlite3-step
        retVal = sqlite3_bind_text(insertStmt, 1, email.from.c_str(), -1, SQLITE_TRANSIENT);
        checkRetval(db, retVal,SQLITE_OK, "failed to bind text");
        retVal = sqlite3_bind_text(insertStmt, 2, email.to.c_str(), -1, SQLITE_TRANSIENT);
        checkRetval(db, retVal,SQLITE_OK, "failed to bind text");
        retVal = sqlite3_bind_text(insertStmt, 3, email.subject.c_str(), -1, SQLITE_TRANSIENT);
        checkRetval(db, retVal,SQLITE_OK, "failed to bind text");
        retVal = sqlite3_bind_text(insertStmt, 4, email.date.c_str(), -1, SQLITE_TRANSIENT);
        checkRetval(db, retVal,SQLITE_OK, "failed to bind text");

        retVal = sqlite3_step(insertStmt);
        checkRetval(db, retVal, SQLITE_DONE, "failed to step through sql statement");

        retVal = sqlite3_reset(insertStmt);
        checkRetval(db, retVal, SQLITE_OK, "couldn't reset statement");
    }

    retVal = sqlite3_finalize(insertStmt);
    checkRetval(db, retVal, SQLITE_OK, "couldn't finalize statement");

    std::cerr << "finished inserting " << emails.size() << " emails" << std::endl;

    sqlite3_close(db);

    return 0;
}
