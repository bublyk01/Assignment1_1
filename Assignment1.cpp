#include <iostream>
#include <string>
#include <vector>
#include <fstream>

int text_input();
int save_text(const std::string& filename);
int load_text(const std::string& filename);

std::vector<std::string> text_lines;

int main() {
    std::string command;

    while (true) {
        std::cout << "Choose the command: ";
        std::cin >> command;
        std::cin.ignore();

        if (command == "1") {
            std::cout << "Enter text:\n";
            text_input();
        }
        else if (command == "2") {
            std::cout << "New line is started\n";
            std::cout << "\n";
        }
        else if (command == "3") {
            std::cout << "Enter the file name for saving:\n";
            std::string filename;
            std::cin >> filename;
            save_text(filename);
            std::cout << "Text has been saved successfully\n";
        }
        else if (command == "4") {
            std::cout << "Enter the file name for loading:\n";
            std::string filename;
            std::cin >> filename;
            load_text(filename);
            std::cout << "Text has been loaded successfully\n";
        }
        else if (command == "5") {
            // implement functionality for command 5
        }
        else if (command == "6") {
            // implement functionality for command 6
        }
        else if (command == "7") {
            // implement functionality for command 7
        }
        else if (command == "help") {
            std::cout << "1 - text typewriter, 2 - new line, 3 - save the file, 4 - load the file\n";
        }
        else if (command == "exit") {
            std::cout << "Exit\n";
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

    std::getline(std::cin, text);
    if (text != "/0") {
        text_lines.push_back(text);
    }

    std::cout << "You wrote:\n";
    for (const auto& line : text_lines) {
        std::cout << line << "\n";
    }

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
