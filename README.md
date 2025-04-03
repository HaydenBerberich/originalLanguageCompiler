# Original Language Compiler for UMSL Target

## Overview

This project is a compiler for a custom, imperative programming language. It takes source code written in this language and translates it into assembly code compatible with a specific UMSL (University of Missouri–St. Louis) target machine/assembler.

The compiler implements several standard phases:
1.  **Lexical Analysis (Scanning):** Tokenizes the input source code (`scanner.cpp`).
2.  **Syntax Analysis (Parsing):** Builds a parse tree based on the language's grammar using recursive descent (`parser.cpp`).
3.  **Abstract Syntax Tree (AST):** Uses a simple tree structure defined in `node.h/cpp`.
4.  **Static Semantic Analysis:** Performs checks like variable declaration verification using a symbol table (`statSem.cpp`).
5.  **Code Generation:** Traverses the parse tree and generates corresponding UMSL assembly instructions (`codeGen.cpp`).

## Features

### Language Features

*   **Variable Declaration:** `create Identifier` (optionally initialized with `:= Integer`). Variables are statically scoped within blocks (`{}`).
*   **Basic I/O:** `cin Identifier` for input, `cout <expr>` for output.
*   **Arithmetic Expressions:** Supports `+`, `-`, `*`, `/`. Includes unary negation (`^`). Operator precedence is handled by the grammar structure.
*   **Assignment:** `set Identifier = <expr>` or `Identifier = <expr>`.
*   **Control Flow:**
    *   Conditional: `if [ <expr> <RO> <expr> ] then <stat>`
    *   Conditional Choice: `pick <expr> <stat> : <stat>` (Executes the first `<stat>` if `<expr>` is non-zero, otherwise the second `<stat>`).
    *   While Loop: `while [ <expr> <RO> <expr> ] <stat>`
    *   Repeat-Until Loop: `repeat <stat> until [ <expr> <RO> <expr> ]`
*   **Relational Operators (`<RO>`):** `<`, `>`, `==`, `=!=`, `.` (Checks if operands have the same parity - both even or both odd).
*   **Labels and Jumps:** `label Identifier` defines a label, `jump Identifier` transfers control.
*   **Basic Functions:** `func Identifier <block>` defines a function block. Control implicitly returns (or via `jump IdentifierReturn`). No explicit parameter passing or return values shown in this snippet.
*   **Comments:** Line comments start with `//`.
*   **Program Structure:** Global variables, optional function definitions (`func`), followed by the main execution `block`, marked by the `tape` keyword separation.

### Compiler Features

*   Generates assembly code for a specific UMSL target.
*   Uses temporary variables (`T0`, `T1`, ...) and labels (`L0`, `L1`, ...) for intermediate results and control flow.
*   Basic static semantic checking (variable definition before use, duplicate definition check).
*   Error reporting for syntax and semantic errors.

## Project Structure

```
.
├── main.cpp          # Main driver, handles I/O and calls compiler phases
├── scanner.h/cpp     # Lexical analyzer (Tokenizer)
├── token.h           # Definition of the Token struct
├── parser.h/cpp      # Syntax analyzer (Recursive Descent Parser)
├── node.h/cpp        # Definition of the Parse Tree Node structure
├── statSem.h/cpp     # Static semantic analyzer
├── codeGen.h/cpp     # Code generator for UMSL assembly
└── (Makefile)        # Optional: For easier building
```

## Requirements

*   A C++ compiler supporting C++11 or later (e.g., g++).
*   The UMSL assembler and virtual machine environment to assemble and run the generated `.asm` file.

## How to Build

You can compile the project using a C++ compiler like g++.

```bash
g++ main.cpp scanner.cpp parser.cpp node.cpp statSem.cpp codeGen.cpp -o compiler -std=c++11
```

This will create an executable named `compiler`.

## How to Run

1.  **Compile your source code:**
    Create a file (e.g., `my_program.lang`) with code written in the defined language. Then, run the compiler:

    ```bash
    ./compiler my_program.lang
    ```

    This will generate an assembly file named `my_program.asm` in the same directory.

2.  **Compile from Standard Input:**
    If you run the compiler without arguments, it reads from standard input until EOF (Ctrl+D on Linux/macOS, Ctrl+Z then Enter on Windows) and outputs to `kb.asm`.

    ```bash
    ./compiler < my_program.lang
    # Output will be in kb.asm
    ```

3.  **Assemble and Run the Output:**
    Use the UMSL assembler and virtual machine tools to process the generated `.asm` file. (Refer to the specific documentation for your UMSL environment).

    *Example (commands may vary based on the UMSL toolchain):*
    ```bash
    # Assuming 'as' is the assembler and 'vm' is the virtual machine
    as my_program.asm
    vm my_program.obj # Or whatever the assembler output format is
    ```

## Language Grammar (BNF)

The parser implements the following grammar (derived from comments in `codeGen.cpp` and parser function names):

```bnf
<program>   -> <vars> tape <func> <block> | <vars> tape <block>
<func>      -> func Identifier <block>
<block>     -> { <vars> <stats> }
<vars>      -> empty | create Identifier <vars2>
<vars2>     -> ; | := Integer ; <vars>
<expr>      -> <N> <expr'>
<expr'>     -> - <expr> | ε
<N>         -> <A> <N'>
<N'>        -> / <A> <N'> | + <A> <N'> | ε
<A>         -> <M> <A'>
<A'>        -> * <A> | ε
<M>         -> ^ <M> | <R>         // ^ appears to be unary negation
<R>         -> ( <expr> ) | Identifier | Integer
<stats>     -> <stat> <mStat>
<mStat>     -> empty | <stat> <mStat>
<stat>      -> <in> ; | <out> ; | <block> | <if> ; | <loop1> ; | <loop2> ; | <assign> ; | <goto> ; | <label> ; | <pick> ;
<in>        -> cin Identifier
<out>       -> cout <expr>
<if>        -> if [ <expr> <RO> <expr> ] then <stat>
<pick>      -> pick <expr> <pickbody>
<pickbody>  -> <stat> : <stat>
<loop1>     -> while [ <expr> <RO> <expr> ] <stat>
<loop2>     -> repeat <stat> until [ <expr> <RO> <expr> ]
<assign>    -> set Identifier = <expr> | Identifier = <expr>
<RO>        -> < | > | == | . . . | =!=      // '...' is represented by a single '.' token in the codeGen, checking parity
<label>     -> label Identifier
<goto>      -> jump Identifier
```

## Target Assembly (UMSL)

The compiler generates assembly code using instructions like:

*   `LOAD`, `STORE`
*   `ADD`, `SUB`, `MULT`, `DIV`
*   `READ`, `WRITE`
*   `BR` (Branch Unconditional)
*   `BRZERO`, `BRPOS`, `BRNEG` (Branch Conditional on Accumulator)
*   `BRZPOS`, `BRZNEG` (Branch Conditional on Accumulator - including zero)
*   `STOP`
*   `NOOP` (Used for labels)

Temporary variables (`T0`, `T1`, ...) and labels (`L0`, `L1`, ...) are generated as needed. All user-defined variables and generated temporaries are declared at the end of the assembly file, initialized to 0.

## Example

**Source Code (`example.lang`)**

```
// Simple program to read two numbers and output their sum
create x;
create y;
create sum := 0;

tape

{
  cin x;
  cin y;
  sum = x + y;
  cout sum;
}
```

**Potential Generated Assembly (`example.asm`)**

*(Note: Specific temporary/label names might vary)*

```assembly
// Compiler Generated UMSL Assembly

          // <vars> handling (implicit, variables declared at end)
          // tape keyword processed
          // <block> starts
          READ x          // cin x;
          READ y          // cin y;
          LOAD x          // sum = x + y; (Start of <expr>)
          STORE T0        // Store x result temporarily
          LOAD y          // Load y
          STORE T1        // Store y result temporarily
          LOAD T0         // Load x back
          ADD T1          // Add y
          STORE sum       // Store result in 'sum'
          LOAD sum        // cout sum; (Start of <expr>)
          STORE T2        // Store 'sum' result temporarily
          WRITE T2        // Write the temporary holding 'sum'
          STOP

// Variable declarations
x         0
y         0
sum       0
T0        0
T1        0
T2        0
```
