#include "peglib_parser.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <vector>

PeglibParser::PeglibParser(const std::string& sourceCode)
    : source(sourceCode) {}

// Helper function: trim whitespace
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

// Helper function: split string by delimiter
static std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        token = trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

std::shared_ptr<Program> PeglibParser::parse() {
    auto program = std::make_shared<Program>();

    // Split source into lines
    std::vector<std::string> lines;
    std::stringstream ss(source);
    std::string line;

    while (std::getline(ss, line)) {
        line = trim(line);
        // Skip empty lines and comments
        if (!line.empty() && line[0] != '#') {
            lines.push_back(line);
        }
    }

    // Parse lines
    size_t i = 0;
    parseStatements(lines, i, program->statements);

    semanticAnalysis(program);
    return program;
}

void PeglibParser::parseStatements(const std::vector<std::string>& lines, size_t& index,
                                   std::vector<std::shared_ptr<Statement>>& statements) {
    while (index < lines.size()) {
        const std::string& line = lines[index];

        if (line == "END") {
            break;
        } else if (line.find("REPEAT") == 0) {
            auto repeatStmt = parseRepeat(lines, index);
            if (repeatStmt) {
                statements.push_back(repeatStmt);
            }
        } else if (line.find("WAIT") == 0) {
            auto waitStmt = parseWait(line);
            if (waitStmt) {
                statements.push_back(waitStmt);
            }
            index++;
        } else if (line.find("LED") == 0) {
            auto ledStmt = parseLED(line);
            if (ledStmt) {
                statements.push_back(ledStmt);
            }
            index++;
        } else if (line.find("MOTOR") == 0) {
            auto motorStmt = parseMotor(line);
            if (motorStmt) {
                statements.push_back(motorStmt);
            }
            index++;
        } else if (line == "STOP") {
            statements.push_back(std::make_shared<StopStatement>());
            index++;
        } else {
            index++;
        }
    }
}

std::shared_ptr<RepeatStatement> PeglibParser::parseRepeat(const std::vector<std::string>& lines, size_t& index) {
    const std::string& line = lines[index];
    index++;

    // Parse "REPEAT N TIMES"
    std::vector<std::string> parts = split(line, ' ');
    if (parts.size() < 3) {
        return nullptr;
    }

    int count = 0;
    try {
        count = std::stoi(parts[1]);
    } catch (...) {
        return nullptr;
    }

    auto repeatStmt = std::make_shared<RepeatStatement>(count);

    // Parse body until END
    parseStatements(lines, index, repeatStmt->body);

    // Skip the END line
    if (index < lines.size() && lines[index] == "END") {
        index++;
    }

    return repeatStmt;
}

std::shared_ptr<WaitStatement> PeglibParser::parseWait(const std::string& line) {
    // Parse "WAIT 500"
    std::vector<std::string> parts = split(line, ' ');
    if (parts.size() < 2) {
        return nullptr;
    }

    try {
        int duration = std::stoi(parts[1]);
        return std::make_shared<WaitStatement>(duration);
    } catch (...) {
        return nullptr;
    }
}

std::shared_ptr<LEDStatement> PeglibParser::parseLED(const std::string& line) {
    // Parse "LED on RED" or "LED off"
    std::vector<std::string> parts = split(line, ' ');
    if (parts.size() < 2) {
        return nullptr;
    }

    std::string state = parts[1];  // "on" or "off"
    std::string color = parts.size() > 2 ? parts[2] : "";

    return std::make_shared<LEDStatement>(state, color);
}

std::shared_ptr<MotorStatement> PeglibParser::parseMotor(const std::string& line) {
    // Parse "MOTOR SPEED 100" or "MOTOR left SPEED 75" or similar variations
    std::vector<std::string> parts = split(line, ' ');

    // Handle different motor command formats
    // Format 1: "MOTOR SPEED 100" (default motor)
    // Format 2: "MOTOR left SPEED 75" (specific motor)

    std::string motorName = "default";
    int speed = 0;

    if (parts.size() < 3) {
        return nullptr;
    }

    try {
        // Check if second part is a number (MOTOR SPEED 100 format)
        if (parts.size() >= 3 && parts[1] == "SPEED") {
            speed = std::stoi(parts[2]);
            motorName = "default";
        }
        // Check if it's MOTOR <name> SPEED <value> format
        else if (parts.size() >= 4 && parts[2] == "SPEED") {
            motorName = parts[1];
            speed = std::stoi(parts[3]);
        }
        // Check for MOTOR <value> format (assume it's speed)
        else if (parts.size() >= 2) {
            speed = std::stoi(parts[1]);
            motorName = "default";
        }
        else {
            return nullptr;
        }

        // Clamp speed to 0-100 range
        if (speed < 0) speed = 0;
        if (speed > 100) speed = 100;

        return std::make_shared<MotorStatement>(motorName, speed);
    } catch (...) {
        return nullptr;
    }
}

void PeglibParser::semanticAnalysis(const std::shared_ptr<Program>& program) {
    for (const auto& funcName : calledFunctions) {
        if (declaredFunctions.find(funcName) == declaredFunctions.end()) {
            std::string available = declaredFunctions.empty() ? "none" : "";
            for (const auto& f : declaredFunctions) {
                if (!available.empty()) available += ", ";
                available += f;
            }
            throw SemanticException(
                "Function '" + funcName + "' called but not defined",
                "Available: " + available
            );
        }
    }
}
