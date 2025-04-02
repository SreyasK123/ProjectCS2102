# DLCO Lab Mini Project

## Overview
The DLCO Lab Mini Project is an assembler and emulator designed to process assembly language code, generate machine code, and execute it. This project includes various components such as libraries for opcode handling, label management, error checking, and memory emulation. This is my second year course project for CS2102 at IIT Patna. 

## Table of Contents
- [Features](#features)
- [File Structure](#file-structure)
- [How to Use](#how-to-use)
- [Warnings and Errors](#warnings-and-errors)

## Features
- **Two-Pass Assembler**: 
  - The first pass checks for syntax errors and creates a label table.
  - The second pass resolves label addressing and generates machine code.
- **Error Handling**: 
  - Detects improper label names, duplicate labels, and other syntax errors.
- **Memory Emulator**: 
  - Executes the generated machine code and produces a memory dump.
- **Normalization**: 
  - Corrects formatting issues in assembly code for better processing.

## The Simple Instruction Set

This instruction set defines operations for a stack-based machine with four registers:
- **A & B**: Act as an internal stack.
- **PC (Program Counter)**: Tracks execution flow.
- **SP (Stack Pointer)**: Manages the stack.

Instructions are **32-bit**, with the **lowest 8 bits as the opcode** and the **upper 24 bits as the operand**. Some instructions **do not require an operand**.

### Instruction Table

| Mnemonic  | Opcode | Operand  | Formal Specification                          | Description                                                               |
|-----------|--------|----------|-----------------------------------------------|--------------                                                             |
| **data**  | -      | value    | -                                             | Reserves a memory location initialized to the specified value.            |                                                  
| **ldc**   | 0      | value    | `B := A; A := value;`                         | Load accumulator with a constant value.                                   |                           
| **adc**   | 1      | value    | `A := A + value;`                             | Add a constant value to the accumulator.                                  |                            
| **ldl**   | 2      | offset   | `B := A; A := memory[SP + offset];`           | Load from a stack offset.                                                 |             
| **stl**   | 3      | offset   | `memory[SP + offset] := A; A := B;`           | Store to a stack offset.                                                  |            
| **ldnl**  | 4      | offset   | `A := memory[A + offset];`                    | Load from a non-local memory location.                                    |                          
| **stnl**  | 5      | offset   | `memory[A + offset] := B;`                    | Store to a non-local memory location.                                     |                         
| **add**   | 6      | -        | `A := B + A;`                                 | Perform addition.                                                         |     
| **sub**   | 7      | -        | `A := B - A;`                                 | Perform subtraction.                                                      |        
| **shl**   | 8      | -        | `A := B << A;`                                | Shift left (logical).                                                     |         
| **shr**   | 9      | -        | `A := B >> A;`                                | Shift right (logical).                                                    |          
| **adj**   | 10     | value    | `SP := SP + value;`                           | Adjust the stack pointer.                                                 |             
| **a2sp**  | 11     | -        | `SP := A; A := B;`                            | Transfer register A to the stack pointer.                                 |                             
| **sp2a**  | 12     | -        | `B := A; A := SP;`                            | Transfer the stack pointer to register A.                                 |                             
| **call**  | 13     | offset   | `B := A; A := PC; PC := PC + offset;`         | Call a subroutine.                                                        |      
| **return**| 14     | -        | `PC := A; A := B;`                            | Return from a subroutine.                                                 |             
| **brz**   | 15     | offset   | `if A == 0 then PC := PC + offset;`           | Branch if accumulator is zero.                                            |                  
| **brlz**  | 16     | offset   | `if A < 0 then PC := PC + offset;`            | Branch if accumulator is negative.                                        |                      
| **br**    | 17     | offset   | `PC := PC + offset;`                          | Unconditional branch.                                                     |         
| **HALT**  | 18     | -        | -                                             | Stop execution (used for emulator termination).                           |                                   
| **SET**   | -      | value    | -                                             | Assign a label to a specific value instead of the PC.                     |                                         
-------------------------------------------------------------------------------------------------------------------------------------------------------------

## Files Generated and Compilation Instructions

The assembler generates **three output files**:

- **`.log` file** – Contains error, warning, and info messages.
- **`.lst` file** – Listing file (only generated if no errors are found).
- **`.o` file** – Machine code object file (used as input for the emulator).

### Compilation Instructions

To compile the **Assembler**:
```sh
g++ Assembler.cpp -o assembler
```


## How to Use
1. **Open the Project**: Open the `DLCO_LAB_MINI_PROJEECT` folder in your preferred IDE (e.g., VS Code).
2. **Edit Code**: Write your assembly code in `code.txt`.

3. **Compile the Project**:
   - Open a terminal or command prompt.
   - Navigate to the `DLCO_LAB_MINI_PROJEECT` directory.
   - Use the following command to compile the project, including all necessary libraries:
     ```bash
     g++ main.cpp libraries/Assembler_lib.cpp libraries/emu_lib.cpp libraries/error_library.cpp libraries/lable_lib.cpp libraries/opcode_lib.cpp libraries/reading_lib.cpp libraries/support_lib.cpp -o assembler
     ```
   - This command compiles `main.cpp` along with all the library files and creates an executable named `assembler`.
4. **Run the Executable**:
   - After successful compilation, run the executable with the following command:
     ```bash
     ./assembler
     ```
   - Choose the run type:
     - Type `-mac` for machine code generation.
     - Type `-emu` for emulation.

## Warnings and Errors
- The emulator may produce large memory dumps (up to 200-300 MB).
- Ensure proper formatting of assembly code to avoid errors:
  - Use labels correctly.
  - Avoid using data declarations in incorrect formats.


