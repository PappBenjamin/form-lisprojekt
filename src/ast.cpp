#include "../include/ast.h"

json Program::toJson() const {
    json j;
    j["type"] = "Program";
    j["statements"] = json::array();
    for (const auto& stmt : statements) {
        j["statements"].push_back(stmt->toJson());
    }
    return j;
}

json RobotDeclaration::toJson() const {
    json j;
    j["type"] = "RobotDeclaration";
    j["name"] = name;
    return j;
}

json MoveStatement::toJson() const {
    json j;
    j["type"] = "Move";
    j["direction"] = direction;
    j["distance"] = distance;
    return j;
}

json TurnStatement::toJson() const {
    json j;
    j["type"] = "Turn";
    j["direction"] = direction;
    j["angle"] = angle;
    return j;
}

json StopStatement::toJson() const {
    json j;
    j["type"] = "Stop";
    return j;
}

json Condition::toJson() const {
    json j;
    j["type"] = "Condition";
    j["left"] = left;
    j["operator"] = op;
    j["right"] = right;
    return j;
}

json IfStatement::toJson() const {
    json j;
    j["type"] = "If";
    j["condition"] = condition->toJson();
    j["thenBody"] = json::array();
    for (const auto& stmt : thenBody) {
        j["thenBody"].push_back(stmt->toJson());
    }
    j["elseBody"] = json::array();
    for (const auto& stmt : elseBody) {
        j["elseBody"].push_back(stmt->toJson());
    }
    return j;
}

json WhileStatement::toJson() const {
    json j;
    j["type"] = "While";
    j["condition"] = condition->toJson();
    j["body"] = json::array();
    for (const auto& stmt : body) {
        j["body"].push_back(stmt->toJson());
    }
    return j;
}

json RepeatStatement::toJson() const {
    json j;
    j["type"] = "Repeat";
    j["times"] = times;
    j["body"] = json::array();
    for (const auto& stmt : body) {
        j["body"].push_back(stmt->toJson());
    }
    return j;
}

json LEDStatement::toJson() const {
    json j;
    j["type"] = "LED";
    j["state"] = state;
    if (!color.empty()) {
        j["color"] = color;
    }
    return j;
}

json ServoStatement::toJson() const {
    json j;
    j["type"] = "Servo";
    j["name"] = name;
    j["angle"] = angle;
    return j;
}

json MotorStatement::toJson() const {
    json j;
    j["type"] = "Motor";
    j["name"] = name;
    j["speed"] = speed;
    return j;
}

json WaitStatement::toJson() const {
    json j;
    j["type"] = "Wait";
    j["duration"] = duration;
    return j;
}

json FunctionDef::toJson() const {
    json j;
    j["type"] = "Function";
    j["name"] = name;
    j["body"] = json::array();
    for (const auto& stmt : body) {
        j["body"].push_back(stmt->toJson());
    }
    return j;
}

json CallStatement::toJson() const {
    json j;
    j["type"] = "Call";
    j["name"] = name;
    return j;
}

json SendStatement::toJson() const {
    json j;
    j["type"] = "Send";
    j["message"] = message;
    return j;
}

