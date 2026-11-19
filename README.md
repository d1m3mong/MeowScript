# MeowScript

MeowScript is a small, statically typed, compiled programming language with the `.meow` file extension. Its compiler is written in **C++20** and currently uses C++ code generation to produce native executables.

## Features

- Statically typed language
- Lexer and tokenizer
- Recursive parser
- AST based architecture using std::variant
- Explicit variable declarations
- Variable assignments
- Arithmetic expressions
- Unary negation
- Boolean and string literals
- print statements
- C++ source code generation
- Native executable generation through GCC

## Example

```meow
catnip: u64 = 100
price: f32 = 9.99
name: string = "Meow"
alive: bool = true

totalCatCount: i32 = (1 + 2) * 4

print(catnip)
print(name)
print(totalCatCount)
```

## Types

| Type | Description |
|---|---|
| `i8`, `i16`, `i32`, `i64` | Signed integers |
| `u8`, `u16`, `u32`, `u64` | Unsigned integers |
| `f32`, `f64` | Floating-point numbers |
| `bool` | Boolean (`true` / `false`) |
| `string` | Strings |

MeowScript is statically typed. Variable types are determined at compile time and are intended to be validated before code generation.

## Operators

| Operator | Description | Precedence |
|---|---|---|
| `*`, `/` | Multiplication and division | Higher |
| `+`, `-` | Addition and subtraction | Lower |
| `-` (prefix) | Unary negation | Highest |

## Requirements

- C++ 20 compatible compiler
- GCC 11 or newer (or another C++20 compatible compiler)
- CMake 3.20 or newer

## Building

### Windows (MinGW)

```bash
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=g++
cmake --build build
```

### Linux / macOS

```bash
cmake -S . -B build
cmake --build build
```

## Usage

MeowScript compiles `.meow` source files into native executables using GCC.

### Windows

```powershell
build\meowscript.exe input.meow -o output.exe
```

### Linux / macOS

```bash
./build/meowscript input.meow -o output
```

The compiler generates an intermediate C++ source file (`output.cpp`) and then invokes GCC to compile it into the requested executable.

The input file must use the `.meow` extension.

## Planned Features

- Semantic analysis
- Complete type checking
- Improved error reporting
- Control flow (if, loops)
- Functions
- Concurrency
- Garbage collection (optionally configurable through compiler flags)
- Direct native code generation (potential future alternative to C++ code generation)

## License

No license has been selected yet.
