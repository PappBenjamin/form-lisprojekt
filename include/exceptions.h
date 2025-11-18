#ifndef ROBOSCRIPT_EXCEPTIONS_H
#define ROBOSCRIPT_EXCEPTIONS_H

#include <string>
#include <sstream>
#include <stdexcept>

/**
 * @brief Base exception class for RoboScript parser
 */
class RoboScriptException : public std::exception {
protected:
    std::string message;

public:
    RoboScriptException(const std::string& msg) : message(msg) {}
    virtual const char* what() const noexcept override {
        return message.c_str();
    }
};

/**
 * @brief Exception thrown during lexical analysis
 */
class LexerException : public RoboScriptException {
private:
    int line;
    int column;
    char invalidChar;

public:
    LexerException(const std::string& msg, int line, int col, char ch)
        : RoboScriptException(msg), line(line), column(col), invalidChar(ch) {}

    std::string toString() const {
        std::ostringstream oss;
        oss << "LexerException at line " << line << ", column " << column << ":\n"
            << "  " << message << "\n"
            << "  Invalid character: '" << invalidChar << "'";
        return oss.str();
    }

    int getLine() const { return line; }
    int getColumn() const { return column; }
    char getInvalidChar() const { return invalidChar; }
};

/**
 * @brief Exception thrown during parsing (syntax errors)
 */
class ParserException : public RoboScriptException {
private:
    int line;
    int column;
    std::string expected;
    std::string found;

public:
    ParserException(const std::string& msg, int line, int col,
                    const std::string& exp, const std::string& fnd)
        : RoboScriptException(msg), line(line), column(col), expected(exp), found(fnd) {}

    std::string toString() const {
        std::ostringstream oss;
        oss << "ParserException at line " << line << ", column " << column << ":\n"
            << "  " << message << "\n"
            << "  Expected: " << expected << "\n"
            << "  Found: " << found;
        return oss.str();
    }

    int getLine() const { return line; }
    int getColumn() const { return column; }
    std::string getExpected() const { return expected; }
    std::string getFound() const { return found; }
};

/**
 * @brief Exception thrown during semantic analysis (logic errors)
 */
class SemanticException : public RoboScriptException {
private:
    std::string context;

public:
    SemanticException(const std::string& msg, const std::string& ctx)
        : RoboScriptException(msg), context(ctx) {}

    std::string toString() const {
        std::ostringstream oss;
        oss << "SemanticException:\n"
            << "  " << message << "\n"
            << "  Context: " << context;
        return oss.str();
    }

    std::string getContext() const { return context; }
};

#endif // ROBOSCRIPT_EXCEPTIONS_H

