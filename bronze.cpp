#include <fstream>
#include <iostream>
#include <string.h>
#include <map>

// Proudly made without the use of generative AI

int main()
{
    std::ifstream file("latest version.bze");

    if (!file) {
        std::cout << "Could not open file";
        return 1;
    }

    int slot_range = 64;
    int taken_slots = 0;
    std::map<int, int> storage;

    std::map<int, std::string> aliases;

    // The index of each command corresponds to its ID
    std::string command_table[8] = {
        "log",
        "set",
        "read",
        "write",
        "sum",
        "diff",
        "product",
        "quotient"
    };

    // MAKE SURE THIS MATCHES THE NUMBER OF COMMANDS IN THE TABLE.
    int command_range = 8;



    // Ignores characters when true. Toggled when asterisks are found
    bool comment_ignore = false;
    // Whether the current line is invalid and should be ignored
    bool line_invalid = false;
    // Indicates whether the parameter part of the current line has been reached yet.
    bool loading_params = false;

    // The name of the command of the current line
    std::string line_command_name;
    // The current raw parameter text of the current line
    std::string line_param;


    // & is capture by reference, accesses variables in same scope as lambda
    auto addRawFunctionName = [&]()
    {
        int command_id = -1; // If command_id is still -1 after the search, that means the command was not found and is invalid
        for (int i=0; i<command_range; i++) {
            if (line_command_name == command_table[i]) {
                // Set the command_id to the index of the matching name
                command_id = i;
                break;
            }
        }

        // If command_id was not changed from -1, the command is invalid
        if (command_id == -1) {
            std::cout << "\"" << line_command_name << "\" is an invalid command.";
            line_invalid = true; // Signal to ignore this line as it's command invalid
            return;
        }

        std::cout << "\nname: " << line_command_name << " id: " << command_id << "\n";

        //if (line_command_name in this.command_table) {
        //    let command_id = this.command_table[line_command_name].id
        //    line.push(command_id)
        //} else {
        //    this.logWarning(`"${line_command_name}" is not a recognized command`)
        //    line_invalid = true;
        //}

        // Reset for next line
        line_command_name.clear();
    };

    auto addLineParameter = [&]() 
    {
        // Only process if raw parameter is not empty
        if (!line_param.empty()) {
            // If the parameter starts with a #, it means the ID of the memory slot associated with an alias or name should be used
            // If used for the first time, a memory slot ID will be associated with it
            if (line_param.substr(0, 1) == "#") {
                // Names for memory slots are called aliases, and memory slots are referred to using aliases in the unprocessed script
                // The alias for the slot being referred to should immediately follow the #.
                std::string alias = line_param.substr(1);

                int slot_id = -1;
                for (int i=0; i<slot_range; i++) {
                    if (alias == aliases[i]) { // If it finds a match, use the ID associated
                        slot_id = i;
                        break;
                    } else {
                        if (taken_slots <= i) {
                            // If it finds an available empty slot, assign this alias to that slot and use its ID

                            aliases[i] = alias;
                            slot_id = i;
                            taken_slots++;
                            break;
                        }
                    }
                }

                std::cout << "Parameter refers to the slot ID " << slot_id << "\n";

                // Add the slot ID value
                //line.push([0, slot_id])

            } else if (line_param.substr(0, 1) == "$") {
                // Or if the parameter starts with a $, the value stored in the memory slot
                // associated with the alias should be used

                std::string alias = line_param.substr(1);

                int slot_id = -1;
                for (int i=0; i<taken_slots; i++) {
                    if (alias == aliases[i]) { // If it finds a match, use the ID associated
                        slot_id = i;
                        break;
                    }
                }

                // If the slot_id is still -1, the alias does not exist
                if (slot_id == -1) {
                    std::cout << "\"" << alias << "\" is not associated with an address";
                    return;
                }

                std::cout << "Parameter refers to the value of slot #" << slot_id << " (" << aliases[slot_id] << ")\n";
                
                // Type 1 is used because the ID should be used to get the value of the memory slot
                //line.push([1, slot_id])

            } else {
                std::cout << "Parameter is a plain value: " << line_param << "\n";
                // If there is no tag and only a number, interpret literally as a number
                // The first number 0 tells the interpreter that this is just a number
                //line.push([0, parseFloat(line_param)])
            }                
        }

        // Reset for next parameter
        line_param.clear();
    };


    char ch;
    // Iterate file character by character
    while (file.get(ch)) {
        // Toggle comment ignore if an asterisk is found
        if (ch == '*') {
            comment_ignore = !comment_ignore;
            continue;
        } 

        // Ignore characters inside comments
        if (comment_ignore) {
            continue;
        }

        // Ignore newlines
        if (ch == '\n') {
            continue;
        }

        // Ignore whitespaces
        if (ch == ' ') {
            continue;
        }


        if (ch == ';') {
            // A semicolon indicates the end of a line

            // Wrap up command names or params for the current line
            if (loading_params) {
                addLineParameter();
            } else {
                addRawFunctionName();
            }

            //if (!line_invalid) {
            //    this.script.push(line);
            //}

            // Everything should be reset for the next line
            loading_params = false;
            //line = []
            line_invalid = false;

            // Continue so that the semicolon itself is not processed below
            continue;
        }


        if (!loading_params) {
            if (ch == ':') {
                // By the time a colon is found, the command name
                // should be completely loaded into the line_command_name string
                
                // Try to find command ID and add to the parsed line
                addRawFunctionName();

                // Now it is loading the parameter part of the line
                loading_params = true;

                continue;
            } else {
                line_command_name += ch;
            }
        } else {
            // Otherwise if the loader is reading the parameter part of the line
            
            // A comma marks the end of a parameter
            if (ch == ',') {
                addLineParameter();

                continue;
            } else {
                line_param += ch;
            }
        }
    }
    std::cout << std::endl;


    file.close();
    return 0;
}