# Assembler-Emulator
The aim of this project is to create a two pass assembler for a reduced simple Assembly instruction set, and then to write and test programs in this simple assembly language. The latter part of the project works on building an Emulator for this simple machine.

***

# The simple assembly language and its instruction set
## Assembly Language
This assembly language is for a machine with four registers,

* Two registers, A & B, arranged as an internal stack.
* A program counter, PC
* A stack pointer, SP
  
These registers are 32 bits in size. Instructions have either no operands or a single operand. The operand is a signed 2's complement value. The encoding uses the bottom 8 bits for opcode and the upper 24 bits for operand.

As with most assembly languages, this is line based (one statement per line). Comments begin with a ';' and anything on the line after the ';' is ignored. Blank lines and lines containing only a comment are permitted (and ignored).

Label definitions consist of the label name followed by a ':', and an optional statement. A valid label name is an alphanumeric string beginning with a letter . An operand is either a label or a number, which can be decimal, hex or octal.

## The Simple Instruction-Set

| Mnemonic | Opcode | Operand | Formal Specification | Description |
|----------|--------|---------|----------------------|-------------|
| **data** | -      | value   | -                    | Reserve a memory location, initialized to the value specified |
| **ldc**  | 0      | value   | `B := A; A := value;`  | Load accumulator with the value specified |
| **adc**  | 1      | value   | `A := A + value;`      | Add the value specified to the accumulator |
| **ldl**  | 2      | offset  | `B := A; A := memory[SP + offset];` | Load local |
| **stl**  | 3      | offset  | `memory[SP + offset] := A; A := B;` | Store local |
| **ldnl** | 4      | offset  | `A := memory[A + offset];` | Load non-local |
| **stnl** | 5      | offset  | `memory[A + offset] := B;` | Store non-local |
| **add**  | 6      | -       | `A := B + A;`          | Addition |
| **sub**  | 7      | -       | `A := B - A;`          | Subtraction |
| **shl**  | 8      | -       | `A := B << A;`         | Shift left |
| **shr**  | 9      | -       | `A := B >> A;`         | Shift right |
| **adj**  | 10     | value   | `SP := SP + value;`    | Adjust SP |
| **a2sp** | 11     | -       | `SP := A; A := B;`     | Transfer A to SP |
| **sp2a** | 12     | -       | `B := A; A := SP;`     | Transfer SP to A |
| **call** | 13     | offset  | `B := A; A := PC; PC := PC + offset;` | Call procedure |
| **return** | 14   | -       | `PC := A; A := B;`     | Return from procedure |
| **brz**  | 15     | offset  | `if A == 0 then PC := PC + offset;` | If accumulator is zero, branch to specified offset |
| **brlz** | 16     | offset  | `if A < 0 then PC := PC + offset;` | If accumulator is less than zero, branch to specified offset |
| **br**   | 17     | offset  | `PC := PC + offset;`   | Branch to specified offset |
| **HALT** | 18     | -       | -                    | Stop the emulator. This is not a 'real' instruction, but needed to tell your emulator when to finish. |
| **SET**  | -      | value   | -                    | Set the label on this line to the specified value (rather than the PC). |

***
# Files Generated and Compilation+Run Instructions
The Assembler generates 3 files:

* .log file for error, warning info.
* .lst listing file.
* .o object file for machine code object.

**The .lst and .o files only generate if no errors are found in the .log file.** The .o object file generated will be used as an input to the Emulator.

## Compilation instructions

To Compile Assembler code and create executable file:
```
g++ Assembler.cpp
```
To Compile Emulator code and create executable file:
```
g++ Emulator.cpp
```
To run a created executable file:

(Windows)
```
./a.exe
```
(Linux)
```
./a.out
```
