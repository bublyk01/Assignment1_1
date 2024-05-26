#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#define CLEAR_COMMAND "cls"

int text_input();
int save_text(const std::string& filename);
int load_text(const std::string& filename);
char getCharAt(const std::vector<std::string>& lines, int lineNumber, int index);
void insertTextAt(std::vector<std::string>& lines, int lineNumber, int index, const std::string& text);
std::vector<std::pair<int, int>> searchText(const std::vector<std::string>& lines, const std::string& searchText);
void clearConsole();

std::vector<std::string> text_lines;

int main() {
    std::string command;

    while (true) {
        clearConsole();
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
            std::cout << "Enter line number and index: ";
            std::cin >> lineNumber >> index;
            std::cin.ignore();

            std::cout << "Enter text to insert: ";
            std::string text;
            std::getline(std::cin, text);

            insertTextAt(text_lines, lineNumber - 1, index, text);
        }
        else if (command == "7") {
            std::cout << "Enter text to search: ";
            std::string search_text;
            std::getline(std::cin, search_text);

            std::vector<std::pair<int, int>> positions = searchText(text_lines, search_text);
            if (positions.empty()) {
                std::cout << "Text not found\n";
            }
            else {
                for (const auto& pos : positions) {
                    std::cout << "Text was found in this position: " << pos.first + 1 << " " << pos.second << "\n";
                }
            }
        }
        else if (command == "help") {
            std::cout << "1 - text typewriter, 2 - new line, 3 - save the file, 4 - load the file, 5 - show what you wrote, 6 - insert text at position, 7 - search\n";
        }
        else if (command == "exit") {
            std::cout << "Exiting the program...\n";
            break;
        }
        else {
            std::cout << "Invalid command\n";
        }

        std::cout << "Press Enter to continue...";
        std::cin.ignore();
    }

    return 0;
}

void clearConsole() {
    system(CLEAR_COMMAND);
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
        std::cout << "Could not create the file\n";
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
        std::cout << "Could not open the file\n";
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

void insertTextAt(std::vector<std::string>& lines, int lineNumber, int index, const std::string& text) {
    if (lineNumber < 0 || lineNumber >= lines.size()) {
        std::cerr << "Line number out of range" << std::endl;
        return;
    }

    std::string& line = lines[lineNumber];
    if (index < 0 || index > line.size()) {
        std::cerr << "Index out of range" << std::endl;
        return;
    }

    line.insert(index, text);
}

std::vector<std::pair<int, int>> searchText(const std::vector<std::string>& lines, const std::string& searchText) {
    std::vector<std::pair<int, int>> positions;
    for (int i = 0; i < lines.size(); ++i) {
        size_t pos = lines[i].find(searchText);
        while (pos != std::string::npos) {
            positions.emplace_back(i, pos);
            pos = lines[i].find(searchText, pos + 1);
        }
    }
    return positions;
}
