#include <iostream>
#include <string>
#include <cstdio>

int text_input();
int save_text(const std::string& filename);
int load_text(const std::string& filename);

int main() {
    std::string command;
    std::cout << "Start the new line\n";
    std::cout << "Choose the command: ";
    std::cin >> command;

    if (command == "1") {
        std::cout << "Enter text to append:\n";
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
    else {
        std::cout << "Invalid command\n";
    }

    return 0;
}

int text_input() {
    std::string text;
    std::cout << "Enter the text: ";
    std::cin.ignore();
    std::getline(std::cin, text);
    std::cout << "You entered: " << text << "\n";

    return 0;
}

int save_text(const std::string& filename) {
    FILE* file;
    errno_t err = fopen_s(&file, filename.c_str(), "w"); //fopen_s to fix the issue with visual studio not liking fopen
    if (err == 0 && file != NULL) {
        fputs("Hello, files world!", file);
        fclose(file);
    }
    else {
        std::cout << "Error opening file for saving\n";
    }
    return 0;
}

int load_text(const std::string& filename) {
    FILE* file;
    char mystring[100];
    errno_t err = fopen_s(&file, filename.c_str(), "r");
    if (err != 0 || file == NULL) {
        std::cout << "Error opening file for loading\n";
    }
    else {
        if (fgets(mystring, 100, file) != NULL) {
            std::cout << mystring << "\n";
        }
        fclose(file);
    }
    return 0;
}
