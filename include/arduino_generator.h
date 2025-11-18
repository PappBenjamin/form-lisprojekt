#ifndef ROBOSCRIPT_ARDUINO_GENERATOR_H
#define ROBOSCRIPT_ARDUINO_GENERATOR_H

#include "ast.h"
#include <string>
#include <vector>
#include <memory>

/**
 * @brief Generates Arduino C++ code from RoboScript AST
 *
 * This class converts an Abstract Syntax Tree into executable Arduino code
 * that can be uploaded to an Arduino board. It handles:
 * - Pin configurations
 * - Motor control
 * - Servo control
 * - LED control
 * - Sensor reading
 * - Loop structures
 */
class ArduinoGenerator {
private:
    int indentLevel;
    std::vector<std::string> generatedCode;
    std::vector<std::string> setupCode;
    std::vector<std::string> variableDeclarations;

    // Pin configurations (can be customized)
    static constexpr int SERVO_PIN = 9;
    static constexpr int LED_PIN = 13;
    static constexpr int MOTOR_LEFT_FORWARD = 5;
    static constexpr int MOTOR_LEFT_BACKWARD = 6;
    static constexpr int MOTOR_RIGHT_FORWARD = 10;
    static constexpr int MOTOR_RIGHT_BACKWARD = 11;
    static constexpr int DISTANCE_SENSOR_PIN = 14;  // A0
    static constexpr int LIGHT_SENSOR_PIN = 15;     // A1

    /**
     * @brief Add indentation to code based on current level
     */
    std::string indent() const;

    /**
     * @brief Add a line of code with proper indentation
     */
    void addLine(const std::string& code);

    /**
     * @brief Add a line to setup code
     */
    void addSetupLine(const std::string& code);

    /**
     * @brief Increase indentation level
     */
    void increaseIndent() { indentLevel++; }

    /**
     * @brief Decrease indentation level
     */
    void decreaseIndent() { indentLevel--; }

    /**
     * @brief Generate code for a statement
     */
    void generateStatement(const std::shared_ptr<Statement>& stmt);

    /**
     * @brief Generate code for robot declaration
     */
    void generateRobotDeclaration(const std::shared_ptr<RobotDeclaration>& stmt);

    /**
     * @brief Generate code for move statement
     */
    void generateMove(const std::shared_ptr<MoveStatement>& stmt);

    /**
     * @brief Generate code for turn statement
     */
    void generateTurn(const std::shared_ptr<TurnStatement>& stmt);

    /**
     * @brief Generate code for stop statement
     */
    void generateStop(const std::shared_ptr<StopStatement>& stmt);

    /**
     * @brief Generate code for if statement
     */
    void generateIf(const std::shared_ptr<IfStatement>& stmt);

    /**
     * @brief Generate code for while loop
     */
    void generateWhile(const std::shared_ptr<WhileStatement>& stmt);

    /**
     * @brief Generate code for repeat loop
     */
    void generateRepeat(const std::shared_ptr<RepeatStatement>& stmt);

    /**
     * @brief Generate code for LED control
     */
    void generateLED(const std::shared_ptr<LEDStatement>& stmt);

    /**
     * @brief Generate code for servo control
     */
    void generateServo(const std::shared_ptr<ServoStatement>& stmt);

    /**
     * @brief Generate code for motor control
     */
    void generateMotor(const std::shared_ptr<MotorStatement>& stmt);

    /**
     * @brief Generate code for wait statement
     */
    void generateWait(const std::shared_ptr<WaitStatement>& stmt);

    /**
     * @brief Generate code for function definition
     */
    void generateFunction(const std::shared_ptr<FunctionDef>& stmt);

    /**
     * @brief Generate code for function call
     */
    void generateCall(const std::shared_ptr<CallStatement>& stmt);

    /**
     * @brief Generate code for send statement
     */
    void generateSend(const std::shared_ptr<SendStatement>& stmt);

    /**
     * @brief Generate condition expression for if/while
     */
    std::string generateCondition(const std::shared_ptr<Condition>& condition);

    /**
     * @brief Generate code for a block of statements
     */
    void generateBlock(const std::vector<std::shared_ptr<Statement>>& statements);

public:
    ArduinoGenerator();

    /**
     * @brief Generate Arduino code from AST
     * @param program The parsed program AST
     * @return Arduino C++ code as a string
     */
    std::string generate(const std::shared_ptr<Program>& program);

    /**
     * @brief Get the setup code
     */
    std::string getSetupCode() const;

    /**
     * @brief Get the main loop code
     */
    std::string getLoopCode() const;

    /**
     * @brief Get variable declarations
     */
    std::string getVariableDeclarations() const;
};

#endif // ROBOSCRIPT_ARDUINO_GENERATOR_H
