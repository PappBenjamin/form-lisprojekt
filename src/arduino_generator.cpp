#include "../include/arduino_generator.h"
#include <sstream>
#include <algorithm>

ArduinoGenerator::ArduinoGenerator() : indentLevel(0) {}

std::string ArduinoGenerator::indent() const {
    return std::string(indentLevel * 2, ' ');
}

void ArduinoGenerator::addLine(const std::string& code) {
    generatedCode.push_back(indent() + code);
}

void ArduinoGenerator::addSetupLine(const std::string& code) {
    setupCode.push_back(indent() + code);
}

void ArduinoGenerator::generateStatement(const std::shared_ptr<Statement>& stmt) {
    if (!stmt) return;

    if (auto robotStmt = std::dynamic_pointer_cast<RobotDeclaration>(stmt)) {
        generateRobotDeclaration(robotStmt);
    } else if (auto moveStmt = std::dynamic_pointer_cast<MoveStatement>(stmt)) {
        generateMove(moveStmt);
    } else if (auto turnStmt = std::dynamic_pointer_cast<TurnStatement>(stmt)) {
        generateTurn(turnStmt);
    } else if (auto stopStmt = std::dynamic_pointer_cast<StopStatement>(stmt)) {
        generateStop(stopStmt);
    } else if (auto ifStmt = std::dynamic_pointer_cast<IfStatement>(stmt)) {
        generateIf(ifStmt);
    } else if (auto whileStmt = std::dynamic_pointer_cast<WhileStatement>(stmt)) {
        generateWhile(whileStmt);
    } else if (auto repeatStmt = std::dynamic_pointer_cast<RepeatStatement>(stmt)) {
        generateRepeat(repeatStmt);
    } else if (auto ledStmt = std::dynamic_pointer_cast<LEDStatement>(stmt)) {
        generateLED(ledStmt);
    } else if (auto servoStmt = std::dynamic_pointer_cast<ServoStatement>(stmt)) {
        generateServo(servoStmt);
    } else if (auto motorStmt = std::dynamic_pointer_cast<MotorStatement>(stmt)) {
        generateMotor(motorStmt);
    } else if (auto waitStmt = std::dynamic_pointer_cast<WaitStatement>(stmt)) {
        generateWait(waitStmt);
    } else if (auto funcStmt = std::dynamic_pointer_cast<FunctionDef>(stmt)) {
        generateFunction(funcStmt);
    } else if (auto callStmt = std::dynamic_pointer_cast<CallStatement>(stmt)) {
        generateCall(callStmt);
    } else if (auto sendStmt = std::dynamic_pointer_cast<SendStatement>(stmt)) {
        generateSend(sendStmt);
    }
}

void ArduinoGenerator::generateRobotDeclaration(const std::shared_ptr<RobotDeclaration>& stmt) {
    addLine("// Robot: " + stmt->name);
    addLine("// Initializing robot systems...");
}

void ArduinoGenerator::generateMove(const std::shared_ptr<MoveStatement>& stmt) {
    int distance = stmt->distance;

    if (stmt->direction == "forward") {
        addLine("// Move forward: " + std::to_string(distance) + " units");
        addLine("digitalWrite(" + std::to_string(MOTOR_LEFT_FORWARD) + ", HIGH);");
        addLine("digitalWrite(" + std::to_string(MOTOR_RIGHT_FORWARD) + ", HIGH);");
        addLine("digitalWrite(" + std::to_string(MOTOR_LEFT_BACKWARD) + ", LOW);");
        addLine("digitalWrite(" + std::to_string(MOTOR_RIGHT_BACKWARD) + ", LOW);");
        // Calculate time based on distance (assuming ~10ms per unit)
        int delayTime = distance * 10;
        addLine("delay(" + std::to_string(delayTime) + ");");
    } else if (stmt->direction == "backward") {
        addLine("// Move backward: " + std::to_string(distance) + " units");
        addLine("digitalWrite(" + std::to_string(MOTOR_LEFT_FORWARD) + ", LOW);");
        addLine("digitalWrite(" + std::to_string(MOTOR_RIGHT_FORWARD) + ", LOW);");
        addLine("digitalWrite(" + std::to_string(MOTOR_LEFT_BACKWARD) + ", HIGH);");
        addLine("digitalWrite(" + std::to_string(MOTOR_RIGHT_BACKWARD) + ", HIGH);");
        int delayTime = distance * 10;
        addLine("delay(" + std::to_string(delayTime) + ");");
    }
}

void ArduinoGenerator::generateTurn(const std::shared_ptr<TurnStatement>& stmt) {
    int angle = stmt->angle;

    if (stmt->direction == "left") {
        addLine("// Turn left: " + std::to_string(angle) + " degrees");
        addLine("digitalWrite(" + std::to_string(MOTOR_LEFT_FORWARD) + ", LOW);");
        addLine("digitalWrite(" + std::to_string(MOTOR_RIGHT_FORWARD) + ", HIGH);");
        addLine("digitalWrite(" + std::to_string(MOTOR_LEFT_BACKWARD) + ", HIGH);");
        addLine("digitalWrite(" + std::to_string(MOTOR_RIGHT_BACKWARD) + ", LOW);");
    } else if (stmt->direction == "right") {
        addLine("// Turn right: " + std::to_string(angle) + " degrees");
        addLine("digitalWrite(" + std::to_string(MOTOR_LEFT_FORWARD) + ", HIGH);");
        addLine("digitalWrite(" + std::to_string(MOTOR_RIGHT_FORWARD) + ", LOW);");
        addLine("digitalWrite(" + std::to_string(MOTOR_LEFT_BACKWARD) + ", LOW);");
        addLine("digitalWrite(" + std::to_string(MOTOR_RIGHT_BACKWARD) + ", HIGH);");
    }
    // Calculate delay based on angle (assuming ~5ms per degree)
    int delayTime = angle * 5;
    addLine("delay(" + std::to_string(delayTime) + ");");
}

void ArduinoGenerator::generateStop(const std::shared_ptr<StopStatement>& stmt) {
    addLine("// Stop all motors");
    addLine("digitalWrite(" + std::to_string(MOTOR_LEFT_FORWARD) + ", LOW);");
    addLine("digitalWrite(" + std::to_string(MOTOR_LEFT_BACKWARD) + ", LOW);");
    addLine("digitalWrite(" + std::to_string(MOTOR_RIGHT_FORWARD) + ", LOW);");
    addLine("digitalWrite(" + std::to_string(MOTOR_RIGHT_BACKWARD) + ", LOW);");
}

std::string ArduinoGenerator::generateCondition(const std::shared_ptr<Condition>& condition) {
    std::string left = condition->left;
    std::string op = condition->op;
    std::string right = condition->right;

    // Replace sensor names with analog reads
    if (left.find("sensor.distance") != std::string::npos) {
        left = "analogRead(" + std::to_string(DISTANCE_SENSOR_PIN) + ")";
    } else if (left.find("sensor.light") != std::string::npos) {
        left = "analogRead(" + std::to_string(LIGHT_SENSOR_PIN) + ")";
    }

    if (right.find("sensor.distance") != std::string::npos) {
        right = "analogRead(" + std::to_string(DISTANCE_SENSOR_PIN) + ")";
    } else if (right.find("sensor.light") != std::string::npos) {
        right = "analogRead(" + std::to_string(LIGHT_SENSOR_PIN) + ")";
    }

    return "(" + left + " " + op + " " + right + ")";
}

void ArduinoGenerator::generateIf(const std::shared_ptr<IfStatement>& stmt) {
    addLine("if " + generateCondition(stmt->condition) + " {");
    increaseIndent();
    generateBlock(stmt->thenBody);
    decreaseIndent();

    if (!stmt->elseBody.empty()) {
        addLine("} else {");
        increaseIndent();
        generateBlock(stmt->elseBody);
        decreaseIndent();
    }

    addLine("}");
}

void ArduinoGenerator::generateWhile(const std::shared_ptr<WhileStatement>& stmt) {
    addLine("while " + generateCondition(stmt->condition) + " {");
    increaseIndent();
    generateBlock(stmt->body);
    decreaseIndent();
    addLine("}");
}

void ArduinoGenerator::generateRepeat(const std::shared_ptr<RepeatStatement>& stmt) {
    addLine("for (int i = 0; i < " + std::to_string(stmt->times) + "; i++) {");
    increaseIndent();
    generateBlock(stmt->body);
    decreaseIndent();
    addLine("}");
}

void ArduinoGenerator::generateLED(const std::shared_ptr<LEDStatement>& stmt) {
    if (stmt->state == "on") {
        addLine("// LED on");
        if (!stmt->color.empty()) {
            addLine("// Color: " + stmt->color);
        }
        addLine("digitalWrite(" + std::to_string(LED_PIN) + ", HIGH);");
    } else if (stmt->state == "off") {
        addLine("// LED off");
        addLine("digitalWrite(" + std::to_string(LED_PIN) + ", LOW);");
    }
}

void ArduinoGenerator::generateServo(const std::shared_ptr<ServoStatement>& stmt) {
    addLine("// Servo " + stmt->name + " to angle " + std::to_string(stmt->angle));
    addLine("servo.write(" + std::to_string(stmt->angle) + ");");
    addLine("delay(100);");
}

void ArduinoGenerator::generateMotor(const std::shared_ptr<MotorStatement>& stmt) {
    int speed = stmt->speed;
    int pwmValue = (speed * 255) / 100;  // Convert percentage to PWM (0-255)

    addLine("// Motor " + stmt->name + " speed: " + std::to_string(speed) + "%");

    if (stmt->name == "left") {
        addLine("analogWrite(" + std::to_string(MOTOR_LEFT_FORWARD) + ", " + std::to_string(pwmValue) + ");");
    } else if (stmt->name == "right") {
        addLine("analogWrite(" + std::to_string(MOTOR_RIGHT_FORWARD) + ", " + std::to_string(pwmValue) + ");");
    }
}

void ArduinoGenerator::generateWait(const std::shared_ptr<WaitStatement>& stmt) {
    addLine("delay(" + std::to_string(stmt->duration) + ");  // Wait " + std::to_string(stmt->duration) + "ms");
}

void ArduinoGenerator::generateFunction(const std::shared_ptr<FunctionDef>& stmt) {
    // Generate function prototype at the beginning
    variableDeclarations.push_back("void " + stmt->name + "() {");

    std::string funcCode;
    int savedIndent = indentLevel;
    indentLevel = 1;

    for (const auto& s : stmt->body) {
        int savedSize = generatedCode.size();
        generateStatement(s);

        // Extract newly generated code
        while (generatedCode.size() > savedSize) {
            funcCode += generatedCode.back() + "\n";
            generatedCode.pop_back();
        }
    }

    indentLevel = savedIndent;

    variableDeclarations.push_back(funcCode);
    variableDeclarations.push_back("}");
    variableDeclarations.push_back("");
}

void ArduinoGenerator::generateCall(const std::shared_ptr<CallStatement>& stmt) {
    addLine(stmt->name + "();  // Call function");
}

void ArduinoGenerator::generateSend(const std::shared_ptr<SendStatement>& stmt) {
    addLine("Serial.println(\"" + stmt->message + "\");");
}

void ArduinoGenerator::generateBlock(const std::vector<std::shared_ptr<Statement>>& statements) {
    for (const auto& stmt : statements) {
        generateStatement(stmt);
    }
}

std::string ArduinoGenerator::generate(const std::shared_ptr<Program>& program) {
    generatedCode.clear();
    setupCode.clear();
    variableDeclarations.clear();
    indentLevel = 0;

    // Generate setup code
    addSetupLine("// Initialize pins");
    addSetupLine("pinMode(" + std::to_string(LED_PIN) + ", OUTPUT);");
    addSetupLine("pinMode(" + std::to_string(MOTOR_LEFT_FORWARD) + ", OUTPUT);");
    addSetupLine("pinMode(" + std::to_string(MOTOR_LEFT_BACKWARD) + ", OUTPUT);");
    addSetupLine("pinMode(" + std::to_string(MOTOR_RIGHT_FORWARD) + ", OUTPUT);");
    addSetupLine("pinMode(" + std::to_string(MOTOR_RIGHT_BACKWARD) + ", OUTPUT);");
    addSetupLine("Serial.begin(9600);");
    addSetupLine("Serial.println(\"Robot initialized\");");

    // Generate main code from statements
    for (const auto& stmt : program->statements) {
        generateStatement(stmt);
    }

    // Combine everything into final Arduino code
    std::stringstream result;

    result << "#include <Servo.h>\n\n";
    result << "// Pin Definitions\n";
    result << "#define LED_PIN " << LED_PIN << "\n";
    result << "#define MOTOR_LEFT_FORWARD " << MOTOR_LEFT_FORWARD << "\n";
    result << "#define MOTOR_LEFT_BACKWARD " << MOTOR_LEFT_BACKWARD << "\n";
    result << "#define MOTOR_RIGHT_FORWARD " << MOTOR_RIGHT_FORWARD << "\n";
    result << "#define MOTOR_RIGHT_BACKWARD " << MOTOR_RIGHT_BACKWARD << "\n";
    result << "#define DISTANCE_SENSOR_PIN " << DISTANCE_SENSOR_PIN << "\n";
    result << "#define LIGHT_SENSOR_PIN " << LIGHT_SENSOR_PIN << "\n\n";

    result << "// Global Variables\n";
    result << "Servo servo;\n\n";

    // Add variable declarations (includes functions)
    for (const auto& var : variableDeclarations) {
        result << var << "\n";
    }

    // Setup function
    result << "void setup() {\n";
    for (const auto& line : setupCode) {
        result << line << "\n";
    }
    result << "}\n\n";

    // Loop function
    result << "void loop() {\n";
    if (!generatedCode.empty()) {
        for (const auto& line : generatedCode) {
            result << line << "\n";
        }
    } else {
        result << "  // Your robot code here\n";
    }
    result << "}\n";

    return result.str();
}

std::string ArduinoGenerator::getSetupCode() const {
    std::stringstream ss;
    for (const auto& line : setupCode) {
        ss << line << "\n";
    }
    return ss.str();
}

std::string ArduinoGenerator::getLoopCode() const {
    std::stringstream ss;
    for (const auto& line : generatedCode) {
        ss << line << "\n";
    }
    return ss.str();
}

std::string ArduinoGenerator::getVariableDeclarations() const {
    std::stringstream ss;
    for (const auto& var : variableDeclarations) {
        ss << var << "\n";
    }
    return ss.str();
}

