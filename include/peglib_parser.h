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

