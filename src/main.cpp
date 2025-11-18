#include <iostream>
#include <fstream>
#include <sstream>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/exceptions.h"
#include "../include/arduino_generator.h"
#include <nlohmann/json.hpp>
#include <unistd.h>

using json = nlohmann::json;

/**
 * @brief Write Arduino code to a .cpp file
 */
void writeArduinoToFile(const std::string& arduinoCode, const std::string& filename = "robot_code.cpp") {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not create file " << filename << "\n";
        return;
    }

    outFile << arduinoCode;
    outFile.close();

    // Get absolute path for display
    char buffer[1024];
    char* cwd = getcwd(buffer, sizeof(buffer));
    std::string fullPath = std::string(cwd) + "/" + filename;

    std::cout << "✓ Arduino code written to: " << fullPath << "\n";
}

/**
 * @brief Parse and display results
 */
void parseAndDisplay(const std::string& code) {
    try {
        std::cout << "\n" << std::string(80, '=') << "\n";
        std::cout << "Parsing RoboScript Code\n";
        std::cout << std::string(80, '=') << "\n\n";

        // Lexical analysis
        std::cout << "[1] Lexical Analysis...\n";
        Lexer lexer(code);
        auto tokens = lexer.tokenize();
        std::cout << "✓ Tokenization successful!\n";
        std::cout << "  Token count: " << tokens.size() << "\n\n";

        // Display tokens
        std::cout << "Tokens:\n";
        for (size_t i = 0; i < tokens.size() - 1; ++i) { // Skip EOF
            const auto& token = tokens[i];
            std::cout << "  [" << i << "] " << token.typeToString()
                      << " = \"" << token.value << "\" "
                      << "(line " << token.line << ", col " << token.column << ")\n";
        }
        std::cout << "\n";

        // Syntax analysis
        std::cout << "[2] Syntax Analysis...\n";
        Parser parser(tokens);
        auto ast = parser.parse();
        std::cout << "✓ Parsing successful!\n\n";

        // Display AST as JSON
        std::cout << "[3] Abstract Syntax Tree (JSON):\n";
        json astJson = ast->toJson();
        std::cout << astJson.dump(2) << "\n\n";

        // Generate Arduino Code
        std::cout << std::string(80, '=') << "\n";
        std::cout << "[4] Arduino Code Generation...\n";
        std::cout << std::string(80, '=') << "\n\n";

        ArduinoGenerator generator;
        std::string arduinoCode = generator.generate(ast);

        // Write Arduino code to file
        writeArduinoToFile(arduinoCode, "robot_code.cpp");
        std::cout << "\n";

        // Summary
        std::cout << std::string(80, '=') << "\n";
        std::cout << "Summary:\n";
        std::cout << "  Total statements: " << ast->statements.size() << "\n";
        std::cout << "  AST size: " << astJson.dump().length() << " characters\n";
        std::cout << "  Arduino code size: " << arduinoCode.length() << " characters\n";
        std::cout << std::string(80, '=') << "\n";

    } catch (const LexerException& e) {
        std::cerr << "\n✗ Lexer Error:\n" << e.toString() << "\n";
    } catch (const ParserException& e) {
        std::cerr << "\n✗ Parser Error:\n" << e.toString() << "\n";
    } catch (const SemanticException& e) {
        std::cerr << "\n✗ Semantic Error:\n" << e.toString() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Error: " << e.what() << "\n";
    }
}

/**
 * @brief Read entire file into string
 */
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    std::cout << "RoboScript Parser v1.0 with Arduino Code Generation\n";
    std::cout << "Reading from: program.robo\n";

    std::string code;

    try {
        code = readFile("program.robo");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    // Display input
    std::cout << "Input Code:\n";
    std::cout << std::string(80, '-') << "\n";
    std::cout << code << "\n";
    std::cout << std::string(80, '-') << "\n";

    // Parse and display results
    parseAndDisplay(code);

    return 0;
}
