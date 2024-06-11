#include <vector>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define CLEAR_COMMAND "cls"

int text_input();
int save_text(const char* filename);
int load_text(const char* filename);
void insertTextAt(std::vector<std::vector<char>>& lines, int lineNumber, int index, const char* text);
std::vector<std::pair<int, int>> searchText(const std::vector<std::vector<char>>& lines, const char* searchText);
void clearConsole();

std::vector<std::vector<char>> text_lines;

int main() {
    char command[10];

    while (true) {
        clearConsole();
        printf("Choose the command: ");
        scanf_s("%9s", command, (unsigned)_countof(command));
        getchar();

        if (strcmp(command, "1") == 0) {
            text_input();
        }
        else if (strcmp(command, "2") == 0) {
            printf("Enter the file name for saving: ");
            char filename[256];
            scanf_s("%255s", filename, (unsigned)_countof(filename));
            save_text(filename);
            printf("Text has been saved successfully\n");
        }
        else if (strcmp(command, "3") == 0) {
            printf("Enter the file name for loading: ");
            char filename[256];
            scanf_s("%255s", filename, (unsigned)_countof(filename));
            load_text(filename);
            printf("Text has been loaded successfully\n");
        }
        else if (strcmp(command, "4") == 0) {
            printf("You wrote:\n");
            for (const auto& line : text_lines) {
                for (char c : line) {
                    putchar(c);
                }
                putchar('\n');
            }
        }
        else if (strcmp(command, "5") == 0) {
            int lineNumber, index;
            printf("Enter the line number along with index: ");
            scanf_s("%d %d", &lineNumber, &index);
            getchar();

            printf("Enter text to insert: ");
            char text[1024];
            fgets(text, 1024, stdin);
            text[strcspn(text, "\n")] = '\0';

            insertTextAt(text_lines, lineNumber - 1, index, text);
        }
        else if (strcmp(command, "6") == 0) {
            printf("Enter text to search: ");
            char search_text[256];
            fgets(search_text, 256, stdin);
            search_text[strcspn(search_text, "\n")] = '\0';

            std::vector<std::pair<int, int>> positions = searchText(text_lines, search_text);
            if (positions.empty()) {
                printf("Text not found\n");
            }
            else {
                for (const auto& pos : positions) {
                    printf("Text was found in this position: %d %d\n", pos.first + 1, pos.second + 1);
                }
            }
        }
        else if (strcmp(command, "help") == 0) {
            printf("1 - text typewriter, 2 - new line, 3 - save the file, 4 - load the file, 5 - show what you wrote, 6 - insert text at position, 7 - search\n");
        }
        else if (strcmp(command, "exit") == 0) {
            printf("Exiting the program...\n");
            break;
        }
        else {
            printf("Command was not found\n");
        }

        printf("Press Enter to continue...");
        getchar();
    }

    return 0;
}

void clearConsole() {
    system(CLEAR_COMMAND);
}

int text_input() {
    char text[1024];
    printf("Enter text:\n");
    while (true) {
        if (!fgets(text, 1024, stdin)) break;
        text[strcspn(text, "\n")] = '\0';
        if (text[0] == '\0') {
            break;
        }
        text_lines.push_back(std::vector<char>(text, text + strlen(text)));
    }
    return 0;
}

int save_text(const char* filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto& line : text_lines) {
            for (char c : line) {
                file.put(c);
            }
            file.put('\n');
        }
        file.close();
    }
    else {
        printf("Could not create the file\n");
    }
    return 0;
}

int load_text(const char* filename) {
    FILE* file;
    if (fopen_s(&file, filename, "r") == 0) {
        char buffer[1024];
        text_lines.clear();
        while (fgets(buffer, sizeof(buffer), file)) {
            buffer[strcspn(buffer, "\n")] = '\0';
            text_lines.push_back(std::vector<char>(buffer, buffer + strlen(buffer)));
        }
        fclose(file);
        printf("Loaded text:\n");
        for (const auto& line : text_lines) {
            for (char c : line) {
                putchar(c);
            }
            putchar('\n');
        }
    }
    else {
        printf("Could not open the file\n");
    }
    return 0;
}

void insertTextAt(std::vector<std::vector<char>>& lines, int lineNumber, int index, const char* text) {
    if (lineNumber < 0 || lineNumber >= lines.size()) {
        fprintf(stderr, "Line number out of range\n");
        return;
    }

    std::vector<char>& line = lines[lineNumber];
    if (index < 0 || index > line.size()) {
        fprintf(stderr, "Index out of range\n");
        return;
    }

    line.insert(line.begin() + index, text, text + strlen(text));
}

std::vector<std::pair<int, int>> searchText(const std::vector<std::vector<char>>& lines, const char* searchText) {
    std::vector<std::pair<int, int>> positions;
    size_t searchLength = strlen(searchText);

    for (size_t i = 0; i < lines.size(); ++i) {
        const std::vector<char>& line = lines[i];
        for (size_t j = 0; j <= line.size() - searchLength; ++j) {
            if (strncmp(&line[j], searchText, searchLength) == 0) {
                positions.emplace_back(i, j);
            }
        }
    }

    return positions;
}
