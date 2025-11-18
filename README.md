# RoboScript Parser - C++ Implementation

A complete domain-specific language (DSL) parser for robotics control, implemented in C++ with proper separation of concerns using header (.h) and source (.cpp) files.

## 🎯 Project Overview

This project implements a full compiler pipeline for the RoboScript language:
- **Lexer** (Lexical Analysis): Converts raw text into tokens
- **Parser** (Syntax Analysis): Converts tokens into an Abstract Syntax Tree (AST)
- **Semantic Analyzer**: Validates logic and meaning
- **JSON Output**: Serializes the AST to JSON format

## 📁 Project Structure

```
form-lisprojekt/
├── CMakeLists.txt          # Build configuration
├── main.cpp                # Entry point and CLI
├── README.md              # This file
├── include/               # Header files (.h)
│   ├── exceptions.h       # Custom exception classes
│   ├── lexer.h           # Lexer interface
│   ├── parser.h          # Parser interface
│   └── ast.h             # Abstract Syntax Tree definitions
└── src/                  # Implementation files (.cpp)
    ├── exceptions.cpp    # Exception implementations
    ├── lexer.cpp         # Lexer implementation
    ├── parser.cpp        # Parser implementation
    └── ast.cpp           # AST node serialization
```

## 🏗️ Architecture Explanation

### 1. **Exceptions (`include/exceptions.h`, `src/exceptions.cpp`)**

Custom exception hierarchy for different error types:

```
RoboScriptException (base)
├── LexerException      → Invalid characters/tokens
├── ParserException     → Syntax errors
└── SemanticException   → Logic/validation errors
```

**Key Methods:**
- `toString()` - Format error with context (line, column, expected vs found)
- `getLine()`, `getColumn()` - Error location
- `getExpected()`, `getFound()` - What was expected vs what was found

### 2. **Lexer (`include/lexer.h`, `src/lexer.cpp`)**

Converts source code into a stream of tokens.

**Main Classes:**
- `Token` - Represents a single token with type, value, line, and column
- `TokenType` enum - Token classifications (KEYWORD, IDENTIFIER, NUMBER, STRING, OPERATOR, etc.)
- `Lexer` - The lexical analyzer

**Key Methods:**
```cpp
std::vector<Token> tokenize()     // Tokenize entire source
Token getNextToken()              // Get next single token
```

**Example:**
```
Input:  MOVE forward 100
Output: [KEYWORD:"MOVE", KEYWORD:"forward", NUMBER:"100"]
```

### 3. **Abstract Syntax Tree (`include/ast.h`, `src/ast.cpp`)**

Represents the hierarchical structure of the program.

**Base Class:**
- `ASTNode` - Abstract base with `toJson()` method

**Statement Types:**
- `RobotDeclaration` - Robot definition
- `MoveStatement` - Move command
- `TurnStatement` - Turn command
- `IfStatement` - Conditional blocks
- `WhileStatement` - Loop blocks
- `FunctionDef` - Function definition
- And many more...

**Key Feature:**
Each node can serialize itself to JSON:
```cpp
json toJson() const override;
```

### 4. **Parser (`include/parser.h`, `src/parser.cpp`)**

Converts tokens into an Abstract Syntax Tree.

**Main Class:**
- `Parser` - Syntax analyzer

**Key Methods:**
```cpp
std::shared_ptr<Program> parse()           // Main entry point
std::shared_ptr<Statement> parseStatement() // Parse single statement
std::vector<std::shared_ptr<Statement>> parseBlock(const std::string& terminator)
void semanticAnalysis(const std::shared_ptr<Program>& program)
```

**Parsing Strategy:**
1. **Peek** - Look at current token without consuming
2. **Check** - Verify token type/value
3. **Consume** - Take token and advance
4. **Parse** - Create appropriate AST node

## 🚀 Building the Project

### Prerequisites
- CMake 3.30+
- C++17 compatible compiler
- Make or Ninja

### Build Steps

```bash
cd form-lisprojekt
mkdir -p cmake-build-debug
cd cmake-build-debug
cmake ..
make -j4
```

### Output
Executable: `form_lisprojekt` (or `form_lisprojekt.exe` on Windows)

## 💻 Usage

### Run with Default Example
```bash
./form_lisprojekt
```

### Run with Specific Example
```bash
./form_lisprojekt --example 1    # Full program
./form_lisprojekt --example 2    # Simple movement
./form_lisprojekt --example 3    # Obstacle avoidance
```

### Parse from File
```bash
./form_lisprojekt --file script.robo
```

### Parse Inline Code
```bash
./form_lisprojekt --code "ROBOT bot1
MOVE forward 50
STOP
END"
```

### Show Help
```bash
./form_lisprojekt --help
```

## 📝 RoboScript Language Reference

### Keywords
- **Control**: `ROBOT`, `FUNCTION`, `CALL`, `END`
- **Movement**: `MOVE`, `TURN`, `STOP`
- **Flow**: `IF`, `THEN`, `ELSE`, `WHILE`, `DO`, `REPEAT`, `TIMES`
- **Devices**: `LED`, `SERVO`, `MOTOR`, `WAIT`, `SEND`

### Syntax Examples

```roboscript
# Robot declaration
ROBOT myRobot

# Basic movement
MOVE forward 100      # Distance in units
TURN left 90         # Angle in degrees
STOP

# Conditionals
IF sensor.distance < 30 THEN
  STOP
  TURN right 180
ELSE
  MOVE forward 50
END

# Loops
REPEAT 3 TIMES
  LED on RED
  WAIT 500
  LED off
END

WHILE sensor.light > 50 DO
  MOVE forward 10
  WAIT 100
END

# Functions
FUNCTION avoidObstacle
  IF sensor.distance < 20 THEN
    TURN right 45
  END
END

CALL avoidObstacle

# Communication
SEND message "Task complete"

END
```

## 🔍 Example: Complete Parse Walkthrough

### Input Code
```
ROBOT bot1
MOVE forward 50
STOP
END
```

### Stage 1: Lexing
```
Token[0]: KEYWORD "ROBOT" (line 1, col 1)
Token[1]: IDENTIFIER "bot1" (line 1, col 7)
Token[2]: KEYWORD "MOVE" (line 2, col 1)
Token[3]: KEYWORD "forward" (line 2, col 6)
Token[4]: NUMBER "50" (line 2, col 14)
Token[5]: KEYWORD "STOP" (line 3, col 1)
Token[6]: KEYWORD "END" (line 4, col 1)
Token[7]: EOF
```

### Stage 2: Parsing
Creates AST nodes from tokens following grammar rules.

### Stage 3: Output (JSON)
```json
{
  "type": "Program",
  "statements": [
    {
      "type": "RobotDeclaration",
      "name": "bot1"
    },
    {
      "type": "Move",
      "direction": "forward",
      "distance": 50
    },
    {
      "type": "Stop"
    }
  ]
}
```

## 🛠️ For Beginners: Understanding Key Concepts

### 1. **Header Files (.h)**
- **Purpose**: Define interfaces (what functions do)
- **Contains**: Class definitions, method declarations, type definitions
- **Analogy**: Blueprint of a house - shows structure but not details

### 2. **Source Files (.cpp)**
- **Purpose**: Implement functionality (how things work)
- **Contains**: Method implementations, helper functions
- **Analogy**: Actual house construction - the detailed work

### 3. **Smart Pointers (`std::shared_ptr`)**
- **Purpose**: Automatic memory management
- **Benefit**: No manual `delete` needed
- **Example**: `std::make_shared<MoveStatement>("forward", 50)`

### 4. **Exceptions**
- **Purpose**: Handle errors gracefully
- **Usage**: Throw when something goes wrong, catch and handle
- **Example**: 
  ```cpp
  try {
    auto ast = parser.parse();
  } catch (const ParserException& e) {
    std::cerr << e.toString();
  }
  ```

### 5. **JSON Serialization**
- **Library**: nlohmann/json (header-only)
- **Usage**: Convert objects to JSON for display/debugging
- **Example**: `json j = statement->toJson();`

## 📊 Data Flow Diagram

```
Source Code
    ↓
┌─────────────┐
│   LEXER     │  → Tokenization
└─────────────┘
    ↓
Token Stream
    ↓
┌─────────────┐
│   PARSER    │  → Syntax Analysis
└─────────────┘
    ↓
Abstract Syntax Tree (AST)
    ↓
┌─────────────┐
│  SEMANTIC   │  → Validation
│  ANALYZER   │
└─────────────┘
    ↓
┌─────────────┐
│   OUTPUT    │  → JSON Serialization
└─────────────┘
    ↓
JSON Output
```

## 🐛 Error Handling

The parser catches three types of errors:

### LexerException
When invalid characters are found:
```
LexerException at line 2, column 5:
  Invalid character: '@'
```

### ParserException
When syntax is invalid:
```
ParserException at line 2, column 1:
  Unexpected token
  Expected: NUMBER
  Found: IDENTIFIER
```

### SemanticException
When logic is invalid:
```
SemanticException:
  Motor speed must be between 0 and 100
  Context: Found: 150
```

## 🔧 Extending the Parser

To add a new statement type:

1. **Add to AST** (`include/ast.h`):
   ```cpp
   class MyStatement : public Statement {
       std::string data;
       json toJson() const override;
       std::string getNodeType() const override { return "MyStatement"; }
   };
   ```

2. **Implement JSON** (`src/ast.cpp`):
   ```cpp
   json MyStatement::toJson() const {
       json j;
       j["type"] = "MyStatement";
       j["data"] = data;
       return j;
   }
   ```

3. **Add Parser Method** (`include/parser.h`):
   ```cpp
   std::shared_ptr<MyStatement> parseMyStatement();
   ```

4. **Implement Parser** (`src/parser.cpp`):
   ```cpp
   std::shared_ptr<MyStatement> Parser::parseMyStatement() {
       consume("MY_KEYWORD");
       // Parse logic here
       return std::make_shared<MyStatement>(/* args */);
   }
   ```

5. **Add to Keywords** (`src/lexer.cpp`):
   ```cpp
   const std::unordered_set<std::string> Lexer::KEYWORDS = {
       // ... existing keywords
       "MY_KEYWORD"
   };
   ```

## 📚 Dependencies

- **nlohmann/json** (v3.11.2) - Header-only JSON library
- **Standard C++17** - STL containers and memory management

All dependencies are automatically downloaded via CMake's FetchContent.

## ✅ Testing

Run the provided examples:
```bash
# Simple movement
./form_lisprojekt --example 2

# Obstacle avoidance with conditions
./form_lisprojekt --example 3

# Full program with functions
./form_lisprojekt
```

## 📖 Learning Resources

- **Compilers & Interpreters**: Classic compiler theory
- **Parsing**: Recursive descent parsing pattern
- **C++**: Modern C++ with smart pointers and STL
- **DSL**: Domain-Specific Language design

## 🎓 Key Takeaways for Beginners

1. **Separation of Concerns**: Each file has one responsibility
2. **Interfaces vs Implementation**: Headers define what, sources define how
3. **Error Handling**: Multiple exception types for different errors
4. **Data Structures**: AST represents program structure hierarchically
5. **Memory Management**: Smart pointers prevent memory leaks
6. **Testing**: Multiple examples test different language features

## 📝 License

Educational project - Feel free to modify and extend!

## 🤝 Contributing

To improve this parser:
1. Add more language features to `lexer.cpp`
2. Extend AST nodes in `ast.h`
3. Add parser rules to `parser.cpp`
4. Test with examples in `main.cpp`

---

Happy parsing! 🚀

