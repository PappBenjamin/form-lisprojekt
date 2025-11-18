#include "../include/parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& toks)
    : tokens(toks), pos(0) {}

const Token& Parser::peek() const {
    if (pos >= tokens.size()) {
        static Token eof(TokenType::END_OF_FILE, "EOF", 0, 0);
        return eof;
    }
    return tokens[pos];
}

const Token& Parser::peek(size_t offset) const {
    if (pos + offset >= tokens.size()) {
        static Token eof(TokenType::END_OF_FILE, "EOF", 0, 0);
        return eof;
    }
    return tokens[pos + offset];
}

Token Parser::consume() {
    if (pos >= tokens.size()) {
        throw ParserException("Unexpected end of input", 0, 0, "token", "EOF");
    }
    return tokens[pos++];
}

Token Parser::consume(const std::string& expected) {
    const Token& token = peek();
    if (token.value != expected) {
        throw ParserException(
            "Unexpected token",
            token.line,
            token.column,
            expected,
            token.value
        );
    }
    pos++;
    return token;
}

bool Parser::check(TokenType type) const {
    return peek().type == type;
}

bool Parser::check(const std::string& value) const {
    return peek().value == value;
}

std::vector<std::shared_ptr<Statement>> Parser::parseBlock(const std::string& terminator) {
    std::vector<std::shared_ptr<Statement>> statements;

    while (!check(terminator) && !check("EOF") && peek().type != TokenType::END_OF_FILE) {
        // Safety check: if we're looking at END or another terminator, break
        if (check("END") || check("ELSE") || check("DO")) {
            break;
        }

        auto stmt = parseStatement();
        if (stmt) {
            statements.push_back(stmt);
        } else {
            // If parseStatement returns nullptr, we must advance to avoid infinite loop
            // This happens when we encounter a terminator or error condition
            if (!check(terminator) && !check("EOF") && peek().type != TokenType::END_OF_FILE) {
                break;  // Break to prevent infinite loop
            }
        }
    }

    return statements;
}

std::shared_ptr<RobotDeclaration> Parser::parseRobotDeclaration() {
    consume("ROBOT");
    Token nameToken = consume();
    return std::make_shared<RobotDeclaration>(nameToken.value);
}

std::shared_ptr<MoveStatement> Parser::parseMoveStatement() {
    consume("MOVE");
    Token dirToken = consume();
    std::string direction = dirToken.value;

    if (direction != "forward" && direction != "backward") {
        throw SemanticException(
            "Invalid movement direction: " + direction,
            "Expected 'forward' or 'backward'"
        );
    }

    Token distToken = consume();
    if (distToken.type != TokenType::NUMBER) {
        throw ParserException(
            "Movement distance must be a number",
            distToken.line,
            distToken.column,
            "NUMBER",
            distToken.typeToString()
        );
    }

    int distance = std::stoi(distToken.value);
    if (distance < 0) {
        throw SemanticException(
            "Movement distance must be positive",
            "Found: " + std::to_string(distance)
        );
    }

    return std::make_shared<MoveStatement>(direction, distance);
}

std::shared_ptr<TurnStatement> Parser::parseTurnStatement() {
    consume("TURN");
    Token dirToken = consume();
    std::string direction = dirToken.value;

    if (direction != "left" && direction != "right") {
        throw SemanticException(
            "Invalid turn direction: " + direction,
            "Expected 'left' or 'right'"
        );
    }

    Token angleToken = consume();
    if (angleToken.type != TokenType::NUMBER) {
        throw ParserException(
            "Turn angle must be a number",
            angleToken.line,
            angleToken.column,
            "NUMBER",
            angleToken.typeToString()
        );
    }

    int angle = std::stoi(angleToken.value);
    return std::make_shared<TurnStatement>(direction, angle);
}

std::shared_ptr<StopStatement> Parser::parseStopStatement() {
    consume("STOP");
    return std::make_shared<StopStatement>();
}

std::shared_ptr<Condition> Parser::parseCondition() {
    Token left = consume();
    Token op = consume();
    Token right = consume();

    return std::make_shared<Condition>(left.value, op.value, right.value);
}

std::shared_ptr<IfStatement> Parser::parseIfStatement() {
    consume("IF");
    auto condition = parseCondition();
    consume("THEN");

    auto thenBody = parseBlock("ELSE");

    std::vector<std::shared_ptr<Statement>> elseBody;
    if (check("ELSE")) {
        consume("ELSE");
        elseBody = parseBlock("END");
    }

    consume("END");

    auto ifStmt = std::make_shared<IfStatement>();
    ifStmt->condition = condition;
    ifStmt->thenBody = thenBody;
    ifStmt->elseBody = elseBody;
    return ifStmt;
}

std::shared_ptr<WhileStatement> Parser::parseWhileStatement() {
    consume("WHILE");
    auto condition = parseCondition();
    consume("DO");

    auto body = parseBlock("END");
    consume("END");

    auto whileStmt = std::make_shared<WhileStatement>();
    whileStmt->condition = condition;
    whileStmt->body = body;
    return whileStmt;
}

std::shared_ptr<RepeatStatement> Parser::parseRepeatStatement() {
    consume("REPEAT");
    Token timesToken = consume();
    if (timesToken.type != TokenType::NUMBER) {
        throw ParserException(
            "Repeat count must be a number",
            timesToken.line,
            timesToken.column,
            "NUMBER",
            timesToken.typeToString()
        );
    }

    int times = std::stoi(timesToken.value);
    consume("TIMES");

    auto body = parseBlock("END");
    consume("END");

    auto repeatStmt = std::make_shared<RepeatStatement>(times);
    repeatStmt->body = body;  // Store the body!
    return repeatStmt;
}

std::shared_ptr<LEDStatement> Parser::parseLEDStatement() {
    consume("LED");
    Token stateToken = consume();
    std::string state = stateToken.value;

    if (state != "on" && state != "off") {
        throw SemanticException(
            "LED state must be 'on' or 'off'",
            "Found: " + state
        );
    }

    std::string color = "";
    if (check(TokenType::IDENTIFIER)) {
        color = consume().value;
    }

    return std::make_shared<LEDStatement>(state, color);
}

std::shared_ptr<ServoStatement> Parser::parseServoStatement() {
    consume("SERVO");
    Token nameToken = consume();
    consume("TO");
    Token angleToken = consume();

    if (angleToken.type != TokenType::NUMBER) {
        throw ParserException(
            "Servo angle must be a number",
            angleToken.line,
            angleToken.column,
            "NUMBER",
            angleToken.typeToString()
        );
    }

    int angle = std::stoi(angleToken.value);
    return std::make_shared<ServoStatement>(nameToken.value, angle);
}

std::shared_ptr<MotorStatement> Parser::parseMotorStatement() {
    consume("MOTOR");
    Token nameToken = consume();
    consume("SPEED");
    Token speedToken = consume();

    if (speedToken.type != TokenType::NUMBER) {
        throw ParserException(
            "Motor speed must be a number",
            speedToken.line,
            speedToken.column,
            "NUMBER",
            speedToken.typeToString()
        );
    }

    int speed = std::stoi(speedToken.value);
    if (speed < 0 || speed > 100) {
        throw SemanticException(
            "Motor speed must be between 0 and 100",
            "Found: " + std::to_string(speed)
        );
    }

    return std::make_shared<MotorStatement>(nameToken.value, speed);
}

std::shared_ptr<WaitStatement> Parser::parseWaitStatement() {
    consume("WAIT");
    Token durationToken = consume();

    if (durationToken.type != TokenType::NUMBER) {
        throw ParserException(
            "Wait duration must be a number",
            durationToken.line,
            durationToken.column,
            "NUMBER",
            durationToken.typeToString()
        );
    }

    int duration = std::stoi(durationToken.value);
    return std::make_shared<WaitStatement>(duration);
}

std::shared_ptr<FunctionDef> Parser::parseFunctionDef() {
    consume("FUNCTION");
    Token nameToken = consume();
    std::string name = nameToken.value;

    declaredFunctions.insert(name);

    auto body = parseBlock("END");
    consume("END");

    auto func = std::make_shared<FunctionDef>(name);
    func->body = body;
    return func;
}

std::shared_ptr<CallStatement> Parser::parseCallStatement() {
    consume("CALL");
    Token nameToken = consume();
    std::string name = nameToken.value;

    calledFunctions.insert(name);

    return std::make_shared<CallStatement>(name);
}

std::shared_ptr<SendStatement> Parser::parseSendStatement() {
    consume("SEND");
    consume("message");
    Token msgToken = consume();

    if (msgToken.type != TokenType::STRING) {
        throw ParserException(
            "Send message must be a string",
            msgToken.line,
            msgToken.column,
            "STRING",
            msgToken.typeToString()
        );
    }

    return std::make_shared<SendStatement>(msgToken.value);
}

std::shared_ptr<Statement> Parser::parseStatement() {
    const Token& token = peek();

    if (token.type == TokenType::END_OF_FILE) {
        return nullptr;
    }

    // Skip END tokens that are not part of a block control structure
    if (check("END")) {
        return nullptr;
    }

    try {
        if (check("ROBOT")) {
            return parseRobotDeclaration();
        } else if (check("MOVE")) {
            return parseMoveStatement();
        } else if (check("TURN")) {
            return parseTurnStatement();
        } else if (check("STOP")) {
            return parseStopStatement();
        } else if (check("IF")) {
            return parseIfStatement();
        } else if (check("WHILE")) {
            return parseWhileStatement();
        } else if (check("REPEAT")) {
            return parseRepeatStatement();
        } else if (check("LED")) {
            return parseLEDStatement();
        } else if (check("SERVO")) {
            return parseServoStatement();
        } else if (check("MOTOR")) {
            return parseMotorStatement();
        } else if (check("WAIT")) {
            return parseWaitStatement();
        } else if (check("FUNCTION")) {
            return parseFunctionDef();
        } else if (check("CALL")) {
            return parseCallStatement();
        } else if (check("SEND")) {
            return parseSendStatement();
        } else if (check("END")) {
            consume("END");
            return nullptr;
        } else {
            throw ParserException(
                "Unknown statement type",
                token.line,
                token.column,
                "Valid statement keyword",
                token.value
            );
        }
    } catch (const ParserException&) {
        throw;
    } catch (const SemanticException&) {
        throw;
    } catch (const std::exception& e) {
        throw ParserException(
            std::string("Error parsing statement: ") + e.what(),
            token.line,
            token.column,
            "Valid statement",
            token.value
        );
    }
}

void Parser::semanticAnalysis(const std::shared_ptr<Program>& program) {
    // Check for undefined function calls
    for (const auto& funcName : calledFunctions) {
        if (declaredFunctions.find(funcName) == declaredFunctions.end()) {
            std::string availableFuncs = "none";
            if (!declaredFunctions.empty()) {
                availableFuncs = "";
                for (const auto& f : declaredFunctions) {
                    if (!availableFuncs.empty()) availableFuncs += ", ";
                    availableFuncs += f;
                }
            }
            throw SemanticException(
                "Function '" + funcName + "' is called but never defined",
                "Available functions: " + availableFuncs
            );
        }
    }
}

std::shared_ptr<Program> Parser::parse() {
    auto program = std::make_shared<Program>();

    while (!check("EOF") && peek().type != TokenType::END_OF_FILE && !check("END")) {
        auto stmt = parseStatement();
        if (stmt) {
            program->statements.push_back(stmt);
        }
    }

    // Consume final END if present
    if (check("END")) {
        consume("END");
    }

    semanticAnalysis(program);

    return program;
}
