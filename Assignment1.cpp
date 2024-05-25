#include <iostream>
#include <stdio.h>
#include <print>

int text_input();
int main() {
    int command;

    printf("Start the new line\n");
    printf("Choose the command: ");
    scanf_s("%d", &command);

    if (command == 1) {
        printf("Enter text to append: \n");
        text_input();
    }
    else if (command == 2) {
        printf("New line is started");
    }
    else if (command == 3) {
        printf("Enter the file name for saving:\n Text has been saved succesfully");
    }
    else if (command == 4) {
        printf("Enter the file name for loading:\n Text has been loaded succesfully");
    }
    else if (command == 5) {

    }
    else if (command == 6) {

    }
    else if (command == 7) {

    }
    else {
        printf("Invalid command\n");
    }

    return 0;
}
int text_input()
{
    std::string text;
    std::cout << "Enter the text: ";
    std::cin >> text;
    std::cout << text;

    return 0;
}