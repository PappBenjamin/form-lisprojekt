#include "../include/peglib_parser.h"
#include "../include/peglib.h"
#include <iostream>

using namespace peg;

PeglibParser::PeglibParser(const std::string& sourceCode)
    : source(sourceCode) {}

std::shared_ptr<Program> PeglibParser::parse() {
    auto program = std::make_shared<Program>();

    // Create PEG parser with simplified RoboScript grammar
    parser parser(R"(
        Program         <- _ Stmt* _
        Stmt            <- Robot / Move / Turn / Stop / If / While / Repeat /
                           LED / Servo / Motor / Wait / Func / Call / Send

        Robot           <- 'ROBOT' _ Name _
        Name            <- < [a-zA-Z_][a-zA-Z0-9_]* >

        Move            <- 'MOVE' _ Dir _ Num _
        Dir             <- 'forward' / 'backward'
        Turn            <- 'TURN' _ TDir _ Num _
        TDir            <- 'left' / 'right'
        Stop            <- 'STOP' _

        If              <- 'IF' _ Cond _ 'THEN' _ Block ('ELSE' _ Block)? 'END' _
        While           <- 'WHILE' _ Cond _ 'DO' _ Block 'END' _
        Repeat          <- 'REPEAT' _ Num _ 'TIMES' _ Block 'END' _
        Block           <- Stmt*

        Cond            <- Ident _ Op _ (Num / Ident)
        Op              <- '<' / '>' / '==' / '!='
        Ident           <- < [a-zA-Z_][a-zA-Z0-9_.]* >

        LED             <- 'LED' _ State _ Col? _
        State           <- 'on' / 'off'
        Col             <- < [a-zA-Z]+ >

        Servo           <- 'SERVO' _ SName _ 'TO' _ Num _
        SName           <- < [a-zA-Z_][a-zA-Z0-9_]* >

        Motor           <- 'MOTOR' _ MName _ 'SPEED' _ Num _
        MName           <- 'left' / 'right'

        Wait            <- 'WAIT' _ Num _

        Func            <- 'FUNCTION' _ FName _ Block 'END' _
        FName           <- < [a-zA-Z_][a-zA-Z0-9_]* >

        Call            <- 'CALL' _ FName _

        Send            <- 'SEND' _ 'message' _ Str _
        Str             <- '"' < (~'"' .)* > '"'

        Num             <- < [0-9]+ >

        _               <- ([ \t\r\n] / Comment)*
        Comment         <- '#' (~'\n' .)*
    )");

    if (!parser) {
        throw ParserException("Failed to initialize PEG grammar", 0, 0, "grammar", "unknown");
    }

    // Setup semantic actions
    parser["Robot"] = [program](const SemanticValues& vs) {
        std::string name(vs.token());
        program->statements.push_back(std::make_shared<RobotDeclaration>(name));
    };

    parser["Move"] = [program](const SemanticValues& vs) {
        std::string direction(vs[0].get<std::string>());
        std::string distStr(vs[1].get<std::string_view>());
        int distance = std::stoi(distStr);

        if (distance < 0) {
            throw SemanticException("Distance must be positive", "Found: " + std::to_string(distance));
        }

        program->statements.push_back(std::make_shared<MoveStatement>(direction, distance));
    };

    parser["Turn"] = [program](const SemanticValues& vs) {
        std::string direction(vs[0].get<std::string>());
        std::string angleStr(vs[1].get<std::string_view>());
        int angle = std::stoi(angleStr);

        program->statements.push_back(std::make_shared<TurnStatement>(direction, angle));
    };

    parser["Stop"] = [program](const SemanticValues& vs) {
        program->statements.push_back(std::make_shared<StopStatement>());
    };

    parser["LED"] = [program](const SemanticValues& vs) {
        std::string state(vs[0].get<std::string>());
        std::string color;
        if (vs.size() > 1) {
            color = std::string(vs[1].get<std::string_view>());
        }
        program->statements.push_back(std::make_shared<LEDStatement>(state, color));
    };

    parser["Servo"] = [program](const SemanticValues& vs) {
        std::string name(vs.token());
        std::string angleStr(vs[1].get<std::string_view>());
        int angle = std::stoi(angleStr);

        program->statements.push_back(std::make_shared<ServoStatement>(name, angle));
    };

    parser["Motor"] = [program](const SemanticValues& vs) {
        std::string name(vs[0].get<std::string>());
        std::string speedStr(vs[1].get<std::string_view>());
        int speed = std::stoi(speedStr);

        if (speed < 0 || speed > 100) {
            throw SemanticException("Speed must be 0-100", "Found: " + std::to_string(speed));
        }

        program->statements.push_back(std::make_shared<MotorStatement>(name, speed));
    };

    parser["Wait"] = [program](const SemanticValues& vs) {
        std::string durationStr(vs[0].get<std::string_view>());
        int duration = std::stoi(durationStr);

        program->statements.push_back(std::make_shared<WaitStatement>(duration));
    };

    parser["Func"] = [this, program](const SemanticValues& vs) {
        std::string name(vs.token());
        declaredFunctions.insert(name);

        auto func = std::make_shared<FunctionDef>(name);
        program->statements.push_back(func);
    };

    parser["Call"] = [this, program](const SemanticValues& vs) {
        std::string name(vs.token());
        calledFunctions.insert(name);

        program->statements.push_back(std::make_shared<CallStatement>(name));
    };

    parser["Send"] = [program](const SemanticValues& vs) {
        std::string message(vs.token());
        program->statements.push_back(std::make_shared<SendStatement>(message));
    };

    // Parse the source code
    if (!parser.parse(source.c_str())) {
        throw ParserException("Failed to parse RoboScript", 0, 0, "token", source);
    }

    // Perform semantic analysis
    semanticAnalysis(program);

    return program;
}

void PeglibParser::semanticAnalysis(const std::shared_ptr<Program>& program) {
    // Check for undefined function calls
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

