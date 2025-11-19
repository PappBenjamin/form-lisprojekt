#ifndef ROBOSCRIPT_AST_H
#define ROBOSCRIPT_AST_H

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * @brief Base class for all AST nodes
 */

class ASTNode {
public:
    virtual ~ASTNode() = default;

    /**
     * @brief Convert node to JSON representation
     */
    virtual json toJson() const = 0;

    /**
     * @brief Get node type as string
     */
    virtual std::string getNodeType() const = 0;
};

// Forward declarations
class Program;
class Statement;
class Expression;

/**
 * @brief Represents the entire program
 */
class Program : public ASTNode {
public:
    std::vector<std::shared_ptr<Statement>> statements;

    json toJson() const override;
    std::string getNodeType() const override { return "Program"; }
};

/**
 * @brief Base class for statements
 */
class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};

/**
 * @brief Robot declaration: ROBOT myRobot
 */
class RobotDeclaration : public Statement {
public:
    std::string name;

    RobotDeclaration(const std::string& n) : name(n) {}

    json toJson() const override;
    std::string getNodeType() const override { return "RobotDeclaration"; }
};

/**
 * @brief Move command: MOVE forward 100
 */
class MoveStatement : public Statement {
public:
    std::string direction;  // forward or backward
    int distance;

    MoveStatement(const std::string& dir, int dist) : direction(dir), distance(dist) {}

    json toJson() const override;
    std::string getNodeType() const override { return "Move"; }
};

/**
 * @brief Turn command: TURN left 90
 */
class TurnStatement : public Statement {
public:
    std::string direction;  // left or right
    int angle;

    TurnStatement(const std::string& dir, int ang) : direction(dir), angle(ang) {}

    json toJson() const override;
    std::string getNodeType() const override { return "Turn"; }
};

/**
 * @brief Stop command
 */
class StopStatement : public Statement {
public:
    json toJson() const override;
    std::string getNodeType() const override { return "Stop"; }
};

/**
 * @brief Condition expression
 */
class Condition : public ASTNode {
public:
    std::string left;
    std::string op;
    std::string right;

    Condition(const std::string& l, const std::string& o, const std::string& r)
        : left(l), op(o), right(r) {}

    json toJson() const override;
    std::string getNodeType() const override { return "Condition"; }
};

/**
 * @brief If-Then-Else statement
 */
class IfStatement : public Statement {
public:
    std::shared_ptr<Condition> condition;
    std::vector<std::shared_ptr<Statement>> thenBody;
    std::vector<std::shared_ptr<Statement>> elseBody;

    json toJson() const override;
    std::string getNodeType() const override { return "If"; }
};

/**
 * @brief While loop
 */
class WhileStatement : public Statement {
public:
    std::shared_ptr<Condition> condition;
    std::vector<std::shared_ptr<Statement>> body;

    json toJson() const override;
    std::string getNodeType() const override { return "While"; }
};

/**
 * @brief Repeat N times loop
 */
class RepeatStatement : public Statement {
public:
    int times;
    std::vector<std::shared_ptr<Statement>> body;

    RepeatStatement(int t) : times(t) {}

    json toJson() const override;
    std::string getNodeType() const override { return "Repeat"; }
};

/**
 * @brief LED control: LED on RED
 */
class LEDStatement : public Statement {
public:
    std::string state;  // on or off
    std::string color;  // optional

    LEDStatement(const std::string& s, const std::string& c = "")
        : state(s), color(c) {}

    json toJson() const override;
    std::string getNodeType() const override { return "LED"; }
};

/**
 * @brief Servo control: SERVO arm TO 90
 */
class ServoStatement : public Statement {
public:
    std::string name;
    int angle;

    ServoStatement(const std::string& n, int a) : name(n), angle(a) {}

    json toJson() const override;
    std::string getNodeType() const override { return "Servo"; }
};

/**
 * @brief Motor control: MOTOR left SPEED 75
 */
class MotorStatement : public Statement {
public:
    std::string name;
    int speed;

    MotorStatement(const std::string& n, int s) : name(n), speed(s) {}

    json toJson() const override;
    std::string getNodeType() const override { return "Motor"; }
};

/**
 * @brief Wait command: WAIT 500
 */
class WaitStatement : public Statement {
public:
    int duration;

    WaitStatement(int d) : duration(d) {}

    json toJson() const override;
    std::string getNodeType() const override { return "Wait"; }
};

/**
 * @brief Function definition
 */
class FunctionDef : public Statement {
public:
    std::string name;
    std::vector<std::shared_ptr<Statement>> body;

    FunctionDef(const std::string& n) : name(n) {}

    json toJson() const override;
    std::string getNodeType() const override { return "Function"; }
};

/**
 * @brief Function call: CALL myFunc
 */
class CallStatement : public Statement {
public:
    std::string name;

    CallStatement(const std::string& n) : name(n) {}

    json toJson() const override;
    std::string getNodeType() const override { return "Call"; }
};

/**
 * @brief Send command: SEND message "text"
 */
class SendStatement : public Statement {
public:
    std::string message;

    SendStatement(const std::string& msg) : message(msg) {}

    json toJson() const override;
    std::string getNodeType() const override { return "Send"; }
};

#endif // ROBOSCRIPT_AST_H

