#include <iostream>
#include <string>

int text_input();
int save_text();
int load_text();

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
        // Code to save the file (not implemented)
        std::cout << "Text has been saved successfully\n";
    }
    else if (command == "4") {
        std::cout << "Enter the file name for loading:\n";
        std::string filename;
        std::cin >> filename;
        // Code to load the file (not implemented)
        std::cout << "Text has been loaded successfully\n";
    }
    else if (command == "5") {
        // Implement functionality for command 5
    }
    else if (command == "6") {
        // Implement functionality for command 6
    }
    else if (command == "7") {
        // Implement functionality for command 7
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
int save_text()
{
    FILE* file;
    file = fopen("myfile.txt", "w");
    if (file != NULL)
    {
        fputs("Hello, files world!", file);
        fclose(file);
    }
    return
        0;
}
int load_text()
{
    FILE* file;
    char mystring[100];
    file = fopen("MyFile.txt", "r");
    if (file == NULL)
    {
        printf("Error opening file");
    }
    else
    {
        if (fgets(mystring, 100, file) != NULL)
        {
            printf("%s", mystring);
        }
        fclose(file);
    }
    return
        0;
}