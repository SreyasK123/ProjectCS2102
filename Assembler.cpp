#include <bits/stdc++.h>
using namespace std;
#define vi vector<int>
#define fr(i, b) for (int i = 0; i < (int)(b); i++)
#define fr1(i, a, b) for (int i = (int)(a); i < (int)(b); i++)
#define vvi vector<vector<int>>
#define endl "\n"

map<string, pair<string, int>> opcode;                 // stores opcode for all instructions in format explained further.
map<int, string> warnings;                             // stores warnings in format warnings[line]=warning.
map<int, string> errors;                               // stores errors in format errors[line]=error.
map<int, pair<string, pair<string, string>>> lineinfo; // this stores info of each line in format lineinfo[prctr]={label,{mnemonic,operand}};
map<string, vi> label_positions;                       // this stores location of each label call.
struct listing
{
    string prctr, mcode, text; // stores each instruction in the format to be put in lst file.
};
// prctr-> program counter.  mcode-> machinecode.

map<string, string> setinstr;          // this data structure is used to store values for set instruction
vector<string> machinecode;            // this stores the machine code for each instruction, which will be used in the machinecode.o file
vector<listing> lst_vec;               // this stores data of each instruction for lst file.
map<string, pair<int, int>> labelinfo; // to store location of each label in format labelinfo[label]={prctr, linenum}.

void opcodes()
{
    /*Key->opcode,   value-> pair where first part refers to conversion of opcode in hex and second part refers
    to type of opcode requiring nothing(1) or value(2) or offset(3)*/
    opcode.insert({"data", {"", 2}});
    opcode.insert({"ldc", {"00", 2}});
    opcode.insert({"adc", {"01", 2}});
    opcode.insert({"ldl", {"02", 3}});
    opcode.insert({"stl", {"03", 3}});
    opcode.insert({"ldnl", {"04", 3}});
    opcode.insert({"stnl", {"05", 3}});
    opcode.insert({"add", {"06", 1}});
    opcode.insert({"sub", {"07", 1}});
    opcode.insert({"shl", {"08", 1}});
    opcode.insert({"shr", {"09", 1}});
    opcode.insert({"adj", {"0a", 2}});
    opcode.insert({"a2sp", {"0b", 1}});
    opcode.insert({"sp2a", {"0c", 1}});
    opcode.insert({"call", {"0d", 3}});
    opcode.insert({"return", {"0e", 1}});
    opcode.insert({"brz", {"0f", 3}});
    opcode.insert({"brlz", {"10", 3}});
    opcode.insert({"br", {"11", 3}});
    opcode.insert({"HALT", {"12", 1}});
    opcode.insert({"SET", {"", 2}});
}

class checker
{
public:
    // checks if character is a digit
    int isdigit(char c)
    {
        if (c >= '0' && c <= '9')
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    // checks if character is an alphabet
    int isalphabet(char c)
    {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_')
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    // checks if string is in decimal representation
    int isdecimal(string s)
    {
        if (s[0] == '0' && s.size() != 1) // to check out for octal.
            return 0;
        fr1(i, 1, s.size())
        {
            if (s[i] >= '0' && s[i] <= '9')
            {
                continue;
            }
            else
            {
                return 0;
            }
        }
        return 1;
    }
    // checks if string is in octal representation
    int isoctal(string s)
    {
        int siz = s.size();
        if (siz < 2 || s[0] != '0')
            return 0;
        fr1(i, 1, s.size())
        {
            if (s[i] >= '0' && s[i] <= '7')
                continue;
            else
                return 0;
        }

        return 1;
    }
    // checks if string is in hexadecimal representation
    int ishex(string s)
    {
        int siz = s.size();
        if (siz < 3 || s[0] != '0' || tolower(s[1]) != 'x')
            return 0;
        fr1(i, 2, s.size())
        {
            if ((s[i] >= '0' && s[i] <= '9') || (tolower(s[i]) >= 'a' && tolower(s[i]) <= 'f'))
                continue;
            else
                return 0;
        }
        return 1;
    }
    // checks if a string is a valid label. i.e, first character is an alphabet, and remaing are alphanumeric.
    int label(string s)
    {
        if (!s.size()) // empty string
            return 0;
        if (!isalphabet(s[0])) // first character has to be an alphabet.
            return 0;
        fr1(i, 1, s.size())
        {
            if (isalphabet(s[i]) || isdigit(s[i]))
                continue;
            else
                return 0;
        }
        return 1;
    }
};
checker check; // object defined for the checker class.

/*Class to convert between different numerical bases. First half of each
function defined refers to current base, and second half refers to converted base.*/
class converter
{
public:
    string octdec(string s)
    {
        int fin = 0, pow = 1;
        for (int i = s.size() - 1; i >= 0; i--)
        {
            fin += (pow * (s[i] - '0'));
            pow *= 8;
        }

        string dec = to_string(fin);
        return dec;
    }

    string hexdec(string s)
    {
        int fin = 0, pow = 1;
        for (int i = s.size() - 1; i >= 0; i--)
        {
            if (check.isdigit(s[i]))
                fin += (pow * (s[i] - '0'));
            else
                fin += (pow * ((tolower(s[i]) - 'a') + 10));
            pow *= 16;
        }

        string dec = to_string(fin);
        return dec;
    }

    string dechex(int num)
    {
        unsigned int num1 = num;
        string fin;
        fr(i, 8)
        {
            int rem = num1 % 16;
            if (rem <= 9)
            {
                char ch = rem + '0';
                fin += ch;
            }
            else
            {
                char ch = rem - 10 + 'a';
                fin += ch;
            }
            num1 /= 16;
        }
        reverse(fin.begin(), fin.end());
        return fin;
    }
};
converter conv; // object defined for converter class.

// This class contains few functions to check for validity of instructions given in pass-1.
class valid
{
public:
    // checks for some errors and stores label positions
    void valid_label(string labe, int prctr, int linenum)
    {
        if (labe.size() > 0)
        {
            if (!check.label(labe)) // Check if label name is valid
            {
                errors[linenum] = "Invalid Label name";
                return;
            }

            auto it = labelinfo.find(labe);
            if (it != labelinfo.end() && it->second.first != -1) // If label has been seen and defined before.
            {
                errors[linenum] = "Duplicate Label Definition";
            }
            else
            {
                labelinfo[labe] = {prctr, linenum};
            }
        }
    }

    // gets the decimal conversion of operand
    string valid_operand(string temp)
    {
        string num;
        if (check.ishex(temp))
        {
            num = conv.hexdec(temp.substr(2)); // Convert hexadecimal to decimal
        }
        else if (check.isoctal(temp))
        {
            num = conv.octdec(temp.substr(1)); // Convert octal to decimal
        }
        else if (check.isdecimal(temp))
        {
            return temp; // Decimal remains the same
        }

        return num;
    }

    // checks for some errors as well as returns 1 if a instruction is valid and 0 if not valid
    int valid_mnemonic(string mnemonic, string &operand, int prct, int linenum)
    {
        int typ = opcode[mnemonic].second;

        if (typ > 1) // operand is expected to be non-empty
        {
            string temp;

            if (check.label(operand)) // operand is a label
            {
                if (labelinfo.find(operand) == labelinfo.end()) // if label is new.
                {
                    labelinfo[operand] = {-1, linenum};
                }
                label_positions[operand].push_back(linenum);
                temp = operand;
            }
            else // operand is expected to be a number
            {
                temp = (operand[0] == '+' || operand[0] == '-') ? operand[0] + valid_operand(operand.substr(1)) : valid_operand(operand);
            }

            if (temp.empty()) // if typ > 1 but neither a valid label nor a value
            {
                errors[linenum] = "Invalid Format";
                return 0;
            }

            return 1;
        }

        else // type is 1
        {
            if (!operand.empty()) // typ=1, but still there is an unexpected operand present
            {
                errors[linenum] = "Unexpected operand";
                return 0;
            }
            else
            {
                return 1;
            }
        }
    }
};
valid sol; // Object initialised for the valid class.

// This function extracts words from each line and returns it in format of vector of strings
vector<string> eachline(string line)
{
    if (line.empty()) // Check if the line is empty, return empty vector
        return {};

    vector<string> words;
    stringstream cur(line);
    string word;

    while (cur >> word) // takes each word of the line, one by one and pushes it into the variable word.
    {
        if (word.empty()) // Skip if word is empty (caused by multiple spaces)
            continue;

        if (word[0] == ';') // Stop at comments, not included in words
            break;

        auto i = word.find(':');                     // Check if it’s a label definition
        if (i != string::npos && word.back() != ':') // Handling issue where no space is given after label.
        {
            words.push_back(word.substr(0, i + 1)); // Add label to vector
            word = word.substr(i + 1);              // Update word to only contain part after label.
        }

        auto j = word.find(';'); // Look for any inline comment
        if (j != string::npos)   // If there's no space between comment and instruction
        {
            word = word.substr(0, j); // Remove comment part from word
            words.push_back(word);
            break; // Ignore anything after comment
        }

        words.push_back(word); // Add word to vector if it’s valid
    }
    return words;
}

vector<string> instrvec; // This stores the content from file in which each string is a line of instruction.


// Helper functions for pass1 defined below.

void errchk() // Additional errors and warnings checks

{
    for (const auto &[label1, label_data] : labelinfo)
    {
        if (label_data.first == -1) // Label declared in instructions but not defined
        {
            for (int linenum : label_positions[label1])
            {
                errors[linenum] = "Label definition not found."; // Update all occurrences with an error
            }
        }
        else if (!label_positions.count(label1))
        {
            warnings[label_data.second] = "Unused Label."; // Unused label, declared pointlessly. Not an error but a warning
        }
    }
}

int mneprocess(string mnemonic, string operand, int prctr, int lineno, int siz, int it)
{
    if (opcode.find(mnemonic) == opcode.end()) // Check if mnemonic exists in the valid instruction set
    {
        errors[lineno] = "Invalid Mnemonic";
    }
    else
    {
        if (opcode[mnemonic].second > 1) // Operand is expected
        { 
            if (operand.empty())
            {
                errors[lineno] = "Missing operand";
            }
            else if (siz > it) // More words in instruction than expected
            {
                errors[lineno] = "Extra at end of instruction.";
            }
        }
    }
    return sol.valid_mnemonic(mnemonic, operand, prctr, lineno); // Validate mnemonic and operand and return 0/1
}

void sethandle(string label, string operand, int lineno)
{
    if (label.empty()) // SET instruction requires a label to set its value
    {
        errors[lineno] = "Missing label name.";
    }
    else
    {
        setinstr[label] = operand; // Store SET instruction information
    }
}
void pass1()
{
    int prctr = 0, lineno = 0;

    for (const string &line : instrvec)
    {
        lineno++;
        auto cur = eachline(line);

        if (cur.empty())
            continue; // Skip if line is empty

        int it = 0;
        const int siz = cur.size();

        // Assigning different parts of a line
        string label, mnemonic, operand;

        if (cur[it].back() == ':') // If ends with ':', then it's a label
        {
            label = cur[it].substr(0, cur[it].size() - 1); // Remove the ':' from the label
            ++it;
        }
        if (it < siz)
            mnemonic = cur[it++]; // Assign mnemonic
        if (it < siz)
            operand = cur[it++]; // Assign operand

        // Checking for errors and updating program counter
        sol.valid_label(label, prctr, lineno);
        int cnt = 0;

        if (!mnemonic.empty())
        {
            cnt = mneprocess(mnemonic, operand, prctr, lineno, siz, it); 
        }

        lineinfo[prctr] = {label, {mnemonic, operand}};
        prctr += cnt; // Update program counter if no errors

        // SET instruction handling
        if (cnt && mnemonic == "SET")
        {
            sethandle(label, operand, lineno);
        }
    }

    errchk(); // Helper function defined above
}

// Below I have defined helper functions that have use in the pass2 function written below them.

// Helper function to generate machine code for type 1 instructions in pass 2
string type1mcode(const string &mnemonic)
{
    return "000000" + opcode[mnemonic].first;
}

// Helper function to generate machine code for type 2 instructions in pass 2
string type2mcode(const string &mnemonic, const string &operand)
{
    int val;
    if (mnemonic == "data" || mnemonic == "SET")
    {
        return conv.dechex(stoi(operand));
    }
    if (!labelinfo.count(operand))
    {
        val = stoi(operand);
    }
    else
    {
        val = labelinfo[operand].first;
    }
    string mcode = conv.dechex(val).substr(2) + opcode[mnemonic].first;
    if (setinstr.count(operand))
    {
        mcode = conv.dechex(stoi(setinstr[operand])).substr(2) + opcode[mnemonic].first;
    }
    return mcode;
}

// Helper function to generate machine code for type 3 instructions in pass 2
string type3mcode(const string &mnemonic, const string &operand, int pctr)
{
    int offset;
    if (!labelinfo.count(operand))
    {
        offset = stoi(operand);
    }
    else
    {
        offset = labelinfo[operand].first - (pctr + 1);
    }
    return conv.dechex(offset).substr(2) + opcode[mnemonic].first;
}

// this function is for pass 2
void pass2()
{
    for (const auto &it : lineinfo)
    {
        // Extract stored information from pass1
        int pctr = it.first;
        const auto &[label, mnemonic_operand] = it.second;
        const auto &[mnemonic, operand] = mnemonic_operand;

        // Initialize type based on mnemonic, default to -100
        int type = mnemonic.empty() ? -100 : opcode[mnemonic].second;
        string mcode = "       ";

        // Choose case based on type (1/2/3) value to assign the machine code
        switch (type)
        {
        case 1:
            mcode = type1mcode(mnemonic);
            break;

        case 2:
            mcode = type2mcode(mnemonic, operand);
            break;

        case 3:
            mcode = type3mcode(mnemonic, operand, pctr);
            break;

        default:
            break;
        }

        // Collect machine code
        machinecode.push_back(mcode);

        // Format label, mnemonic, and operand for writing in lst files
        string formatted_label = label.empty() ? "" : label + ": ";
        string formatted_mnemonic = mnemonic.empty() ? "" : mnemonic + " ";
        string text = formatted_label + formatted_mnemonic + operand;

        // Add entry to lst_vec with converted pctr, machine code, and formatted text
        lst_vec.push_back({conv.dechex(pctr), mcode, text});
    }
}

// This function writes into the logfile if errors are there
void writeerrors(string file_name)
{
    ofstream printerrors(file_name + ".log");

    if (errors.empty()) // if no errors found
    {
        printerrors << "NO ERRORS" << endl;

        for (const auto &warning : warnings)
        {
            printerrors << "Line Number " << warning.first << " WARNING: " << warning.second << endl;
        }
    }
    else
    {
        for (const auto &error : errors) // if any errors have been located.
        {
            printerrors << "Line Number " << error.first << " ERROR: " << error.second << endl;
        }
    }

    printerrors.close();
}

int main()
{
    opcodes();

    // Taking input from file
    string file_name;
    cout << "Enter asm file to take input from, with exact file name and extension: ";
    cin >> file_name;
    ifstream inpfile(file_name);

    if (!inpfile)
    {
        cout << "Input file doesn't exist in directory" << endl;
        return 0;
    }
    string temp_file_name; // For creating the log and lst files with same base file name
    for (int i = 0; i < file_name.size(); i++)
    {
        if (file_name[i] == '.')
        {
            break;
        }
        temp_file_name.push_back(file_name[i]);
    }

    string lines;
    while (getline(inpfile, lines))
    {
        instrvec.push_back(lines); // Filling instrvec with each line of instruction.
    }
    inpfile.close();

    pass1();                     // Calling pass1
    writeerrors(temp_file_name); // Error file filling

    if (errors.empty()) // If no errors are found, proceed to pass2
    {
        cout << "errorfile generated - no errors found." << endl;
        pass2();

        // Writing to the listing file
        ofstream writelist(temp_file_name + ".lst");
        for (const auto &entry : lst_vec)
        {
            writelist << entry.prctr << " " << entry.mcode << " " << entry.text << endl;
        }

        cout << "Listfile generated" << endl;
        writelist.close();

        // Writing to the .o file
        ofstream printmcode(temp_file_name + ".o", ios::binary | ios::out);
        for (const auto &code : machinecode)
        {
            if (!code.empty() && code != "        ")
            {
                int cur = stoi(conv.hexdec(code)); // Convert hex string to integer
                printmcode.write(reinterpret_cast<const char *>(&cur), sizeof(int));
            }
        }
        printmcode.close();
        cout << "Machinecode file generated" << endl;
    }
    else
    {
        cout << "Errors detected (Did not proceed to pass-2). View logfile." << endl;
    }

    return 0;
}
