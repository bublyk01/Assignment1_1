#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

#define CLEAR_COMMAND "cls"

int text_input();
int save_text(const char* filename);
int load_text(const char* filename);
void insertTextAt(char**& lines, int& lineCount, int lineNumber, int index, const char* text);
std::pair<int, int>* searchText(char** lines, int lineCount, const char* searchText, int& resultCount);
void clearConsole();
void freeTextLines(char**& lines, int lineCount);

char** text_lines = nullptr;
int line_count = 0;

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
            for (int i = 0; i < line_count; ++i) {
                printf("%s\n", text_lines[i]);
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

            insertTextAt(text_lines, line_count, lineNumber - 1, index, text);
        }
        else if (strcmp(command, "6") == 0) {
            printf("Enter text to search: ");
            char search_text[256];
            fgets(search_text, 256, stdin);
            search_text[strcspn(search_text, "\n")] = '\0';

            int resultCount;
            std::pair<int, int>* positions = searchText(text_lines, line_count, search_text, resultCount);
            if (resultCount == 0) {
                printf("Text not found\n");
            }
            else {
                for (int i = 0; i < resultCount; ++i) {
                    printf("Text was found in this position: %d %d\n", positions[i].first + 1, positions[i].second + 1);
                }
            }
            delete[] positions;
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

    freeTextLines(text_lines, line_count);

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

        text_lines = (char**)realloc(text_lines, (line_count + 1) * sizeof(char*));
        text_lines[line_count] = (char*)malloc((strlen(text) + 1) * sizeof(char));
        strcpy_s(text_lines[line_count], strlen(text) + 1, text);
        ++line_count;
    }
    return 0;
}

int save_text(const char* filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (int i = 0; i < line_count; ++i) {
            file << text_lines[i] << '\n';
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
        freeTextLines(text_lines, line_count);
        text_lines = nullptr;
        line_count = 0;
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file)) {
            buffer[strcspn(buffer, "\n")] = '\0';
            text_lines = (char**)realloc(text_lines, (line_count + 1) * sizeof(char*));
            text_lines[line_count] = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
            strcpy_s(text_lines[line_count], strlen(buffer) + 1, buffer);
            ++line_count;
        }
        fclose(file);
        printf("Loaded text:\n");
        for (int i = 0; i < line_count; ++i) {
            printf("%s\n", text_lines[i]);
        }
    }
    else {
        printf("Could not open the file\n");
    }
    return 0;
}

void insertTextAt(char**& lines, int& lineCount, int lineNumber, int index, const char* text) {
    if (lineNumber < 0 || lineNumber >= lineCount) {
        fprintf(stderr, "Line number out of range\n");
        return;
    }

    char* line = lines[lineNumber];
    int lineLength = (int)strlen(line);
    int textLength = (int)strlen(text);

    if (index < 0 || index > lineLength) {
        fprintf(stderr, "Index out of range\n");
        return;
    }

    line = (char*)realloc(line, (lineLength + textLength + 1) * sizeof(char));
    memmove(line + index + textLength, line + index, lineLength - index + 1);
    memcpy(line + index, text, textLength);
    lines[lineNumber] = line;
}

std::pair<int, int>* searchText(char** lines, int lineCount, const char* searchText, int& resultCount) {
    resultCount = 0;
    std::pair<int, int>* positions = nullptr;
    size_t searchLength = strlen(searchText);

    for (int i = 0; i < lineCount; ++i) {
        const char* line = lines[i];
        for (const char* pos = strstr(line, searchText); pos; pos = strstr(pos + 1, searchText)) {
            positions = (std::pair<int, int>*)realloc(positions, (resultCount + 1) * sizeof(std::pair<int, int>));
            positions[resultCount] = std::make_pair(i, (int)(pos - line));
            ++resultCount;
        }
    }

    return positions;
}

void freeTextLines(char**& lines, int lineCount) {
    for (int i = 0; i < lineCount; ++i) {
        free(lines[i]);
    }
    free(lines);
    lines = nullptr;
}
