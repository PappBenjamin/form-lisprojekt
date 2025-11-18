#ifndef ROBOSCRIPT_PARSER_H
#define ROBOSCRIPT_PARSER_H

#include <vector>
#include <memory>
#include <unordered_set>
#include "lexer.h"
#include "ast.h"
#include "exceptions.h"

/**
 * @brief Parser for RoboScript language
 * Converts tokens into an Abstract Syntax Tree
 */
class Parser {
private:
    std::vector<Token> tokens;
    size_t pos;
    std::unordered_set<std::string> declaredFunctions;
    std::unordered_set<std::string> calledFunctions;

    /**
     * @brief Get current token without consuming it
     */
    const Token& peek() const;

    /**
     * @brief Get token at offset from current position
     */
    const Token& peek(size_t offset) const;

    /**
     * @brief Consume and return current token
     */
    Token consume();

    /**
     * @brief Consume expected token
     * @throws ParserException if token doesn't match expected
     */
    Token consume(const std::string& expected);

    /**
     * @brief Check if current token matches type
     */
    bool check(TokenType type) const;

    /**
     * @brief Check if current token matches value
     */
    bool check(const std::string& value) const;

    /**
     * @brief Parse a complete program
     */
    std::shared_ptr<Program> parseProgram();

    /**
     * @brief Parse a single statement
     */
    std::shared_ptr<Statement> parseStatement();

    /**
     * @brief Parse ROBOT declaration
     */
    std::shared_ptr<RobotDeclaration> parseRobotDeclaration();

    /**
     * @brief Parse MOVE statement
     */
    std::shared_ptr<MoveStatement> parseMoveStatement();

    /**
     * @brief Parse TURN statement
     */
    std::shared_ptr<TurnStatement> parseTurnStatement();

    /**
     * @brief Parse STOP statement
     */
    std::shared_ptr<StopStatement> parseStopStatement();

    /**
     * @brief Parse IF statement
     */
    std::shared_ptr<IfStatement> parseIfStatement();

    /**
     * @brief Parse WHILE loop
     */
    std::shared_ptr<WhileStatement> parseWhileStatement();

    /**
     * @brief Parse REPEAT loop
     */
    std::shared_ptr<RepeatStatement> parseRepeatStatement();

    /**
     * @brief Parse LED control
     */
    std::shared_ptr<LEDStatement> parseLEDStatement();

    /**
     * @brief Parse SERVO control
     */
    std::shared_ptr<ServoStatement> parseServoStatement();

    /**
     * @brief Parse MOTOR control
     */
    std::shared_ptr<MotorStatement> parseMotorStatement();

    /**
     * @brief Parse WAIT command
     */
    std::shared_ptr<WaitStatement> parseWaitStatement();

    /**
     * @brief Parse FUNCTION definition
     */
    std::shared_ptr<FunctionDef> parseFunctionDef();

    /**
     * @brief Parse CALL statement
     */
    std::shared_ptr<CallStatement> parseCallStatement();

    /**
     * @brief Parse SEND statement
     */
    std::shared_ptr<SendStatement> parseSendStatement();

    /**
     * @brief Parse a condition expression
     */
    std::shared_ptr<Condition> parseCondition();

    /**
     * @brief Parse a block of statements until END or other terminator
     */
    std::vector<std::shared_ptr<Statement>> parseBlock(const std::string& terminator = "END");

    /**
     * @brief Perform semantic analysis on AST
     */
    void semanticAnalysis(const std::shared_ptr<Program>& program);

public:
    Parser(const std::vector<Token>& toks);

    /**
     * @brief Parse tokens into AST
     * @return Abstract Syntax Tree (Program node)
     * @throws ParserException for syntax errors
     * @throws SemanticException for semantic errors
     */
    std::shared_ptr<Program> parse();
};

#endif // ROBOSCRIPT_PARSER_H

