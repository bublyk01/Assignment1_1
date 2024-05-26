#include <iostream>
#include <string>
#include <vector>
#include <fstream>

int text_input();
int save_text(const std::string& filename);
int load_text(const std::string& filename);
char getCharAt(const std::vector<std::string>& lines, int lineNumber, int index);

std::vector<std::string> text_lines;

int main() {
    std::string command;

    while (true) {
        std::cout << "Choose the command (or type 'exit' to quit): ";
        std::cin >> command;
        std::cin.ignore();

        if (command == "1") {
            text_input();
        }
        else if (command == "2") {
            std::cout << "Enter new line text: ";
            std::string new_line;
            std::getline(std::cin, new_line);
            text_lines.push_back(new_line);
        }
        else if (command == "3") {
            std::cout << "Enter the file name for saving: ";
            std::string filename;
            std::cin >> filename;
            save_text(filename);
            std::cout << "Text has been saved successfully\n";
        }
        else if (command == "4") {
            std::cout << "Enter the file name for loading: ";
            std::string filename;
            std::cin >> filename;
            load_text(filename);
            std::cout << "Text has been loaded successfully\n";
        }
        else if (command == "5") {
            std::cout << "You wrote:\n";
            for (const auto& line : text_lines) {
                std::cout << line << "\n";
            }
        }
        else if (command == "6") {
            int lineNumber, index;
            std::cout << "Enter line number: ";
            std::cin >> lineNumber;
            std::cout << "Enter index: ";
            std::cin >> index;
            std::cin.ignore();

            char result = getCharAt(text_lines, lineNumber - 1, index);
            if (result != '\0') {
                std::cout << "Character at line " << lineNumber << " and index " << index << " is: " << result << std::endl;
            }
        }
        else if (command == "7") {
            // implement functionality for command 7
        }
        else if (command == "help") {
            std::cout << "1 - text typewriter, 2 - new line, 3 - save the file, 4 - load the file, 5 - show what you wrote, 6 - insert, 7 - search\n";
        }
        else if (command == "exit") {
            std::cout << "Exiting the program...\n";
            break;
        }
        else {
            std::cout << "Invalid command\n";
        }
    }

    return 0;
}

int text_input() {
    std::string text;
    std::cout << "Enter text: ";
    std::getline(std::cin, text);
    text_lines.push_back(text);
    return 0;
}

int save_text(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto& line : text_lines) {
            file << line << "\n";
        }
        file.close();
    }
    else {
        std::cout << "Error opening file for saving\n";
    }
    return 0;
}

int load_text(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        text_lines.clear();
        while (std::getline(file, line)) {
            text_lines.push_back(line);
        }
        file.close();
        std::cout << "Loaded text:\n";
        for (const auto& line : text_lines) {
            std::cout << line << "\n";
        }
    }
    else {
        std::cout << "Error opening file for loading\n";
    }
    return 0;
}

char getCharAt(const std::vector<std::string>& lines, int lineNumber, int index) {
    if (lineNumber < 0 || lineNumber >= lines.size()) {
        std::cerr << "Line number out of range" << std::endl;
        return '\0';
    }

    const std::string& line = lines[lineNumber];
    if (index < 0 || index >= line.size()) {
        std::cerr << "Index out of range" << std::endl;
        return '\0';
    }

    return line[index];
}
