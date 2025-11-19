#include "../include/lexer.h"
#include <cctype>
#include <iostream>

const std::unordered_set<std::string> Lexer::KEYWORDS = {
    "ROBOT", "MOVE", "TURN", "STOP", "IF", "THEN", "ELSE", "END",
    "WHILE", "DO", "REPEAT", "TIMES", "FUNCTION", "CALL", "LED",
    "SERVO", "MOTOR", "SPEED", "WAIT", "SEND", "TO", "forward",
    "backward", "left", "right", "on", "off", "sensor", "message"
};

std::string Token::typeToString() const {
    switch (type) {
        case TokenType::KEYWORD: return "KEYWORD";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::STRING: return "STRING";
        case TokenType::OPERATOR: return "OPERATOR";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::END_OF_FILE: return "EOF";
        default: return "UNKNOWN";
    }
}

Lexer::Lexer(const std::string& code)
    : source(code), pos(0), line(1), column(1) {}

char Lexer::currentChar() const {
    if (pos >= source.length()) {
        return '\0';
    }
    return source[pos];
}

char Lexer::peekChar(size_t offset) const {
    if (pos + offset >= source.length()) {
        return '\0';
    }
    return source[pos + offset];
}

// Basically moves to the next character in the source code for lexing
void Lexer::advance() {
    if (pos < source.length()) {
        if (source[pos] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        pos++;
    }
}

void Lexer::skipWhitespace() {
    while (std::isspace(currentChar())) {
        advance();
    }
}

void Lexer::skipComment() {
    if (currentChar() == '#') {
        while (currentChar() != '\n' && currentChar() != '\0') {
            advance();
        }
        if (currentChar() == '\n') {
            advance();
        }
    }
}

Token Lexer::readNumber() {
    int startLine = line;
    int startCol = column;
    std::string number;

    while (std::isdigit(currentChar())) {
        number += currentChar();
        advance();
    }

    return Token(TokenType::NUMBER, number, startLine, startCol);
}

Token Lexer::readIdentifier() {
    int startLine = line;
    int startCol = column;
    std::string ident;

    while (std::isalnum(currentChar()) || currentChar() == '_' || currentChar() == '.') {
        ident += currentChar();
        advance();
    }

    // Check if it's a keyword
    if (KEYWORDS.find(ident) != KEYWORDS.end()) {
        return Token(TokenType::KEYWORD, ident, startLine, startCol);
    }

    return Token(TokenType::IDENTIFIER, ident, startLine, startCol);
}

Token Lexer::readString() {
    int startLine = line;
    int startCol = column;
    advance(); // Skip opening quote
    std::string str;

    while (currentChar() != '"' && currentChar() != '\0') {
        if (currentChar() == '\\' && peekChar() != '\0') {
            advance();
            switch (currentChar()) {
                case 'n': str += '\n'; break;
                case 't': str += '\t'; break;
                case '\\': str += '\\'; break;
                case '"': str += '"'; break;
                default: str += currentChar();
            }
            advance();
        } else {
            str += currentChar();
            advance();
        }
    }

    if (currentChar() != '"') {
        throw LexerException("Unterminated string literal", line, column, currentChar());
    }

    advance(); // Skip closing quote
    return Token(TokenType::STRING, str, startLine, startCol);
}

Token Lexer::getNextToken() {
    skipWhitespace();

    while (currentChar() == '#') {
        skipComment();
        skipWhitespace();
    }

    int tokenLine = line;
    int tokenCol = column;

    char ch = currentChar();

    if (ch == '\0') {
        return Token(TokenType::END_OF_FILE, "EOF", tokenLine, tokenCol);
    }

    if (std::isdigit(ch)) {
        return readNumber();
    }

    if (std::isalpha(ch) || ch == '_') {
        return readIdentifier();
    }

    if (ch == '"') {
        return readString();
    }

    if (ch == '(') {
        advance();
        return Token(TokenType::LPAREN, "(", tokenLine, tokenCol);
    }

    if (ch == ')') {
        advance();
        return Token(TokenType::RPAREN, ")", tokenLine, tokenCol);
    }

    if (ch == ',') {
        advance();
        return Token(TokenType::COMMA, ",", tokenLine, tokenCol);
    }

    if (ch == '.') {
        advance();
        return Token(TokenType::DOT, ".", tokenLine, tokenCol);
    }

    if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '<' || ch == '>' || ch == '=') {
        std::string op;
        op += ch;
        advance();
        return Token(TokenType::OPERATOR, op, tokenLine, tokenCol);
    }

    throw LexerException("Invalid character", line, column, ch);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (true) {
        Token token = getNextToken();
        tokens.push_back(token);
        if (token.type == TokenType::END_OF_FILE) {
            break;
        }
    }

    return tokens;
}

