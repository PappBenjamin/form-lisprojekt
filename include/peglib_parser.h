#ifndef ROBOSCRIPT_PEGLIB_PARSER_H
#define ROBOSCRIPT_PEGLIB_PARSER_H

#include <memory>
#include <string>
#include <unordered_set>
#include "ast.h"
#include "exceptions.h"

/**
 * @brief Parser for RoboScript language using Peglib
 * Converts source code into an Abstract Syntax Tree
 */
class PeglibParser {
private:
    std::string source;
    std::unordered_set<std::string> declaredFunctions;
    std::unordered_set<std::string> calledFunctions;

    /**
     * @brief Parse statements from lines
     */
    void parseStatements(const std::vector<std::string>& lines, size_t& index,
                        std::vector<std::shared_ptr<Statement>>& statements);

    /**
     * @brief Parse REPEAT block
     */
    std::shared_ptr<RepeatStatement> parseRepeat(const std::vector<std::string>& lines, size_t& index);

    /**
     * @brief Parse WAIT statement
     */
    std::shared_ptr<WaitStatement> parseWait(const std::string& line);

    /**
     * @brief Parse LED statement
     */
    std::shared_ptr<LEDStatement> parseLED(const std::string& line);

    /**
     * @brief Parse MOTOR statement
     */
    std::shared_ptr<MotorStatement> parseMotor(const std::string& line);

    /**
     * @brief Perform semantic analysis on AST
     */
    void semanticAnalysis(const std::shared_ptr<Program>& program);

public:
    /**
     * @brief Constructor
     * @param sourceCode The RoboScript source code to parse
     */
    PeglibParser(const std::string& sourceCode);

    /**
     * @brief Parse the source code into an AST
     * @return Abstract Syntax Tree (Program node)
     * @throws ParserException for syntax errors
     * @throws SemanticException for semantic errors
     */
    std::shared_ptr<Program> parse();
};

#endif // ROBOSCRIPT_PEGLIB_PARSER_H
