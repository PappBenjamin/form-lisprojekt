#ifndef ROBOSCRIPT_LEXER_H
#define ROBOSCRIPT_LEXER_H

#include <string>
#include <vector>
#include <unordered_set>
#include "exceptions.h"

/**
 * @brief Token types in RoboScript language
 */
enum class TokenType {
    KEYWORD,      // Keywords like ROBOT, MOVE, TURN, etc.
    IDENTIFIER,   // Variable/function names
    NUMBER,       // Numeric literals
    STRING,       // String literals
    OPERATOR,     // Operators like +, -, <, >, =, etc.
    LPAREN,       // (
    RPAREN,       // )
    COMMA,        // ,
    DOT,          // .
    END_OF_FILE,  // EOF
};

/**
 * @brief Represents a single token
 */
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType t, const std::string& v, int l, int c)
        : type(t), value(v), line(l), column(c) {}

    std::string typeToString() const;
};

/**
 * @brief Lexical analyzer for RoboScript
 */
class Lexer {
private:
    std::string source;
    size_t pos;
    int line;
    int column;

    // Keywords in the RoboScript language
    static const std::unordered_set<std::string> KEYWORDS;

    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    void skipWhitespace();
    void skipComment();
    Token readNumber();
    Token readIdentifier();
    Token readString();

public:
    Lexer(const std::string& code);

    /**
     * @brief Tokenize the entire source code
     * @return Vector of tokens
     * @throws LexerException if invalid characters are found
     */
    std::vector<Token> tokenize();

    /**
     * @brief Get next token
     * @return Next token in the sequence
     * @throws LexerException if invalid character is encountered
     */
    Token getNextToken();
};

#endif // ROBOSCRIPT_LEXER_H

