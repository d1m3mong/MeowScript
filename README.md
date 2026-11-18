# MeowScript

MeowScript is a small, compiled programming language with the `.meow` file extension. Its compiler is written in **C++20**.

## Features

- Statically typed language
- AOT compilation (planned)
- Explicit variable declarations
- Arithmetic expressions
- Unary negation

## Example

```meow
catnip: u64 = 100
price: f32 = 9.99
name: string = "Meow"
alive: bool = true

totalCatCount: i32 = (1 + 2) * 4
```

## Types

| Type | Description |
|---|---|
| `i8`, `i16`, `i32`, `i64` | Signed integers |
| `u8`, `u16`, `u32`, `u64` | Unsigned integers |
| `f32`, `f64` | Floating-point numbers |
| `bool` | Boolean (`true` / `false`) |
| `string` | Strings |

Since MeowScript is statically typed, variable types are determined at compile time and will be checked before code generation.

## Operators

| Operator | Description | Precedence |
|---|---|---|
| `*`, `/` | Multiplication and division | Higher |
| `+`, `-` | Addition and subtraction | Lower |
| `-` (prefix) | Unary negation | Highest |

### Requirements

- C++20 compatible compiler
- GCC 11 or newer (or another C++20 compatible compiler)
- CMake 3.20 or newer

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
**Windows:**
```powershell
build\meowscript.exe input.meow
```

**Linux / macOS:**
```bash
./build/meowscript input.meow
```

The input file must use the `.meow` extension.

## In the making

- Complete lexer implementation
- Expression parsing
- Variable declarations
- Assignments
- print statements
- Semantic analysis
- Type checking
- Code generation
- Native executable generation
- Control flow (if, loops)
- Functions
- Garbage collector(optionally toggled using flags)

## License

No license has been selected yet.
