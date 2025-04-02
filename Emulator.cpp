#include <bits/stdc++.h>
using namespace std;
#define vi vector<int>
#define fr(i, b) for (int i = 0; i < (int)(b); i++)
#define fr1(i, a, b) for (int i = (int)(a); i < (int)(b); i++)
#define vvi vector<vector<int>>
#define endl "\n"

// Conversion function: decimal to hexadecimal
string dechex(int num) {
    unsigned int num1 = num;
    string res = "";
    for (int i = 0; i < 8; i++) {
        int rem = num1 % 16;
        res += (rem < 10) ? char(rem + '0') : char(rem - 10 + 'a');
        num1 /= 16;
    }
    reverse(res.begin(), res.end());
    return res;
}

// Memory and register definitions
int mem[20000005];
int pc, sp, a, b;
vector<int> machinecodes;

// Memory access logs
struct MemoryRead { int address, value; };
struct MemoryWrite { int address, previous, current; };
vector<MemoryRead> read_log;
vector<MemoryWrite> write_log;

// Traces the program in the .o file until halted or a fault occurs
void trace(int program_size) {
    pc = sp = a = b = 0;
    bool halt_flag = false, fault_flag = false;
    int instruction_count = 0;

    while (pc < program_size && !halt_flag) {
        int instruction = machinecodes[pc];

        // Extract opcode (type) and operand (offset)
        int type = instruction & 0xFF;
        int temp=instruction>>8;
		int offset = 0;
		for(int i=8; i<31; i++)
		{
			if(instruction & (1<<i))
				offset+=(1<<(i-8));

		}
        if (instruction & (1 << 31)) offset -= (1 << 23);

        int prev_val = -1;

        // Perform operation based on the decoded opcode
        switch (type) {
            case 0: b = a; a = offset; break;
            case 1: a += offset; break;
            case 2: b = a; a = mem[sp + offset]; read_log.push_back({sp + offset, a}); break;
            case 3: prev_val = mem[sp + offset]; mem[sp + offset] = a; a = b;
                    write_log.push_back({sp + offset, prev_val, mem[sp + offset]}); break;
            case 4: prev_val = a; a = mem[a + offset]; read_log.push_back({prev_val + offset, a}); break;
            case 5: prev_val = mem[a + offset]; mem[a + offset] = b;
                    write_log.push_back({a + offset, prev_val, b}); break;
            case 6: a += b; break;
            case 7: a = b - a; break;
            case 8: a = (b << a); break;
            case 9: a = (b >> a); break;
            case 10: sp += offset; break;
            case 11: sp = a; a = b; break;
            case 12: b = a; a = sp; break;
            case 13: b = a; a = pc; pc += offset; break;
            case 14: pc = a; a = b; break;
            case 15: if (a == 0) pc += offset; break;
            case 16: if (a < 0) pc += offset; break;
            case 17: pc += offset; break;
            case 18: halt_flag = true; break;
            // default: fault_flag = true; break;
        }

        // Increment program counter and log register state
        pc++;
        cout << "PC=" << dechex(pc) << " SP=" << dechex(sp) << " A=" << dechex(a) << " B=" << dechex(b) << endl;

        // Check for program termination conditions(indefinite loop/wrong jump)
        if (pc < 0 || instruction_count > (1 << 24)) {
            fault_flag = true;
            break;
        }
        instruction_count++;
    }

    if (fault_flag) {
        cout << "Segmentation fault" << endl;
        exit(0);
    }

    cout << "Total instructions executed: " << instruction_count << endl;
}

// Displays the Instruction Set Architecture (ISA) supported by the emulator
void show_isa() {
    cout << "Opcode  Mnemonic   Operand" << endl;
    cout << "0       ldc        value" << endl;
    cout << "1       adc        value" << endl;
    cout << "2       ldl        value" << endl;
    cout << "3       stl        value" << endl;
    cout << "4       ldnl       value" << endl;
    cout << "5       stnl       value" << endl;
    cout << "6       add" << endl;
    cout << "7       sub" << endl;
    cout << "8       shl" << endl;
    cout << "9       shr" << endl;
    cout << "10      adj        value" << endl;
    cout << "11      a2sp" << endl;
    cout << "12      sp2a" << endl;
    cout << "13      call       offset" << endl;
    cout << "14      return" << endl;
    cout << "15      brz        offset" << endl;
    cout << "16      brlz       offset" << endl;
    cout << "17      br         offset" << endl;
    cout << "18      HALT" << endl;
    cout << "       SET         value" << endl;
}

int main() {
    // Requesting input file from user
    string file_name;
    cout << "Enter the filename with extension: ";
    cin >> file_name;

    ifstream file(file_name, ios::binary);
    if (!file.is_open()) {
        cout << "Error: could not open file." << endl;
        return 1;
    }

    // Loading machine codes from file into memory and machinecodes vector
    int counter = 0;
    unsigned int cur;
    while (file.read((char*)&cur, sizeof(int))) {
        machinecodes.push_back(cur);
        mem[counter++] = cur;
    }
    int program_size = machinecodes.size();

    // Menu for emulator functionalities
    while (true) {
        cout << "\nMENU\n";
        cout << "1: Run Trace\n2: Display ISA\n3: Reset Registers\n4: Show Pre-Execution Memory Dump\n";
        cout << "5: Show Post-Execution Memory Dump\n6: Show Memory Reads\n7: Show Memory Writes\n8: Exit\n";
        cout << "Enter option: ";
        int choice;
        cin >> choice;

        switch (choice) {
            case 1: trace(program_size); break;
            case 2: show_isa(); break;
            case 3: pc = sp = a = b = 0; cout << "Registers reset.\n"; break;
            case 4:
                cout << "Memory dump before execution:" << endl;
                for (int i = 0; i < program_size; i += 4) {
                    cout << dechex(i) << " ";
                    for (int j = i; j < min(program_size, i + 4); ++j)
                        cout << dechex(machinecodes[j]) << " ";
                    cout << endl;
                }
                break;
            case 5:
                cout << "Memory dump after execution:" << endl;
                for (int i = 0; i < program_size; i += 4) {
                    cout << dechex(i) << " ";
                    for (int j = i; j < min(program_size, i + 4); ++j)
                        cout << dechex(mem[j]) << " ";
                    cout << endl;
                }
                break;
            case 6:
                for (const auto& read : read_log)
                    cout << "Read from memory[" << dechex(read.address) << "] = " << dechex(read.value) << endl;
                break;
            case 7:
                for (const auto& write : write_log)
                    cout << "Memory[" << dechex(write.address) << "] changed from " 
                         << dechex(write.previous) << " to " << dechex(write.current) << endl;
                break;
            case 8: return 0;
            default: cout << "Invalid option. Try again.\n"; break;
        }
    }

    return 0;
}
