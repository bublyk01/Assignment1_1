#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <utility>

#define CLEAR_COMMAND "cls"

int text_input();
int save_text(const char* filename);
int load_text(const char* filename);
void insertTextAt(char**& lines, int& lineCount, int lineNumber, int index, const char* text);
std::pair<int, int>* searchText(char** lines, int lineCount, const char* searchText, int& resultCount);
void clearConsole();
void freeTextLines(char**& lines, int lineCount);
void deleteText(char**& lines, int& lineCount, int lineNumber, int index, int numChars);
void cutText(char**& lines, int& lineCount, int lineNumber, int index, int numChars);
void copyText(char** lines, int lineNumber, int index, int numChars);
void pasteText(char**& lines, int& lineCount, int lineNumber, int index);
void insertWithReplacement(char**& lines, int& lineCount, int lineNumber, int index, const char* text);

char** text_lines = nullptr;
int line_count = 0;
char clipboard[1024] = { 0 };

struct Command {
    char name[10];
    char text[1024];
    int lineNumber;
    int index;
    int numChars;
};

Command commandHistory[3];
int commandIndex = 0;

Command redoHistory[3];
int redoIndex = 0;
bool redoAvailable = false;

size_t strlen(const char* str) {
    const char* s = str;
    while (*s) ++s;
    return s - str;
}

void strcpy(char* dest, size_t destsz, const char* src) {
    if (destsz == 0) return;
    size_t i;
    for (i = 0; i < destsz - 1 && src[i] != '\0'; ++i) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

size_t strcspn(const char* str, const char* reject) {
    const char* s = str;
    while (*s) {
        for (const char* r = reject; *r; ++r) {
            if (*s == *r) {
                return s - str;
            }
        }
        ++s;
    }
    return s - str;
}

const char* strstr(const char* haystack, const char* needle) {
    if (!*needle) return haystack;
    for (; *haystack; ++haystack) {
        if ((*haystack == *needle) && !std::memcmp(haystack, needle, strlen(needle))) {
            return haystack;
        }
    }
    return nullptr;
}

void* memmove(void* dest, const void* src, size_t n) {
    if (dest == nullptr || src == nullptr) return nullptr;
    char* d = (char*)dest;
    const char* s = (const char*)src;
    if (d < s) {
        while (n--) {
            *d++ = *s++;
        }
    }
    else {
        const char* lasts = s + (n - 1);
        char* lastd = d + (n - 1);
        while (n--) {
            *lastd-- = *lasts--;
        }
    }
    return dest;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        ++s1;
        ++s2;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void strncpy_s(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; ++i) {
        dest[i] = src[i];
    }
    for (; i < n; ++i) {
        dest[i] = '\0';
    }
}

void addCommandToHistory(const char* name, const char* text = "", int lineNumber = -1, int index = -1, int numChars = -1) {
    strcpy(commandHistory[commandIndex].name, 10, name);
    strcpy(commandHistory[commandIndex].text, 1024, text);
    commandHistory[commandIndex].lineNumber = lineNumber;
    commandHistory[commandIndex].index = index;
    commandHistory[commandIndex].numChars = numChars;
    commandIndex = (commandIndex + 1) % 3;
}

void addCommandToRedoHistory(const char* name, const char* text = "", int lineNumber = -1, int index = -1, int numChars = -1) {
    strcpy(redoHistory[redoIndex].name, 10, name);
    strcpy(redoHistory[redoIndex].text, 1024, text);
    redoHistory[redoIndex].lineNumber = lineNumber;
    redoHistory[redoIndex].index = index;
    redoHistory[redoIndex].numChars = numChars;
    redoIndex = (redoIndex + 1) % 3;
}

void undoLastCommand() {
    if (commandIndex == 0) {
        commandIndex = 2;
    }
    else {
        commandIndex--;
    }
    Command lastCommand = commandHistory[commandIndex];

    if (strcmp(lastCommand.name, "insert") == 0) {
        deleteText(text_lines, line_count, lastCommand.lineNumber, lastCommand.index, strlen(lastCommand.text));
        addCommandToRedoHistory("insert", lastCommand.text, lastCommand.lineNumber, lastCommand.index);
    }
    else if (strcmp(lastCommand.name, "delete") == 0) {
        insertTextAt(text_lines, line_count, lastCommand.lineNumber, lastCommand.index, lastCommand.text);
        addCommandToRedoHistory("delete", lastCommand.text, lastCommand.lineNumber, lastCommand.index, lastCommand.numChars);
    }
    else if (strcmp(lastCommand.name, "cut") == 0) {
        insertTextAt(text_lines, line_count, lastCommand.lineNumber, lastCommand.index, clipboard);
        addCommandToRedoHistory("cut", clipboard, lastCommand.lineNumber, lastCommand.index, lastCommand.numChars);
    }
    else if (strcmp(lastCommand.name, "paste") == 0) {
        deleteText(text_lines, line_count, lastCommand.lineNumber, lastCommand.index, strlen(clipboard));
        addCommandToRedoHistory("paste", clipboard, lastCommand.lineNumber, lastCommand.index);
    }
    strcpy(commandHistory[commandIndex].name, 10, "");
    redoAvailable = true;
}


void redoLastCommand() {
    if (!redoAvailable) {
        printf("No commands to redo.\n");
        return;
    }

    if (redoIndex == 0) {
        redoIndex = 2;
    }
    else {
        redoIndex--;
    }
    Command lastCommand = redoHistory[redoIndex];

    if (strcmp(lastCommand.name, "insert") == 0) {
        insertTextAt(text_lines, line_count, lastCommand.lineNumber, lastCommand.index, lastCommand.text);
        addCommandToHistory("insert", lastCommand.text, lastCommand.lineNumber, lastCommand.index);
    }
    else if (strcmp(lastCommand.name, "delete") == 0) {
        deleteText(text_lines, line_count, lastCommand.lineNumber, lastCommand.index, lastCommand.numChars);
        addCommandToHistory("delete", lastCommand.text, lastCommand.lineNumber, lastCommand.index, lastCommand.numChars);
    }
    else if (strcmp(lastCommand.name, "cut") == 0) {
        cutText(text_lines, line_count, lastCommand.lineNumber, lastCommand.index, lastCommand.numChars);
        addCommandToHistory("cut", lastCommand.text, lastCommand.lineNumber, lastCommand.index, lastCommand.numChars);
    }
    else if (strcmp(lastCommand.name, "paste") == 0) {
        pasteText(text_lines, line_count, lastCommand.lineNumber, lastCommand.index);
        addCommandToHistory("paste", lastCommand.text, lastCommand.lineNumber, lastCommand.index);
    }
    strcpy(redoHistory[redoIndex].name, 10, "");
}


int main() {
    char command[10];

    while (true) {
        clearConsole();
        printf("Choose the command: ");
        scanf_s("%9s", command, (unsigned)_countof(command));
        getchar();

        if (strcmp(command, "1") == 0) {
            text_input();
            addCommandToHistory("input");
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
            addCommandToHistory("insert", text, lineNumber - 1, index);
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
        else if (strcmp(command, "7") == 0) {
            int lineNumber, index, numChars;
            printf("Choose line, index and number of symbols: ");
            scanf_s("%d %d %d", &lineNumber, &index, &numChars);
            getchar();

            char* deletedText = new char[numChars + 1];
            strncpy_s(deletedText, &text_lines[lineNumber - 1][index], numChars);
            deletedText[numChars] = '\0';

            deleteText(text_lines, line_count, lineNumber - 1, index, numChars);
            addCommandToHistory("delete", deletedText, lineNumber - 1, index, numChars);

            delete[] deletedText;
            printf("Text has been deleted successfully\n");
        }
        else if (strcmp(command, "help") == 0) {
            printf("1 - text typewriter, 2 - new line, 3 - save the file, 4 - load the file, 5 - show what you wrote, 6 - insert text at position, 7 - search, 8 - undo, 9 - redo, 10 - cut, 11 - copy, 12 - paste, 13 - insert with replacement\n");
        }
        else if (strcmp(command, "8") == 0) {
            undoLastCommand();
            printf("Last command undone\n");
        }
        else if (strcmp(command, "9") == 0) {
            redoLastCommand();
            printf("Last command redone\n");
        }
        else if (strcmp(command, "exit") == 0) {
            printf("Exiting the program...\n");
            break;
        }
        else if (strcmp(command, "10") == 0) {
            int lineNumber, index, numChars;
            printf("Enter the line number, index, and number of characters to cut: ");
            scanf_s("%d %d %d", &lineNumber, &index, &numChars);
            getchar();

            cutText(text_lines, line_count, lineNumber - 1, index, numChars);
            addCommandToHistory("cut", clipboard, lineNumber - 1, index, numChars);
        }
        else if (strcmp(command, "11") == 0) {
            int lineNumber, index, numChars;
            printf("Enter the line number, index, and number of characters to copy: ");
            scanf_s("%d %d %d", &lineNumber, &index, &numChars);
            getchar();

            copyText(text_lines, lineNumber - 1, index, numChars);
            printf("Copied text: %s\n", clipboard);
        }
        else if (strcmp(command, "12") == 0) {
            int lineNumber, index;
            printf("Enter the line number and index to paste: ");
            scanf_s("%d %d", &lineNumber, &index);
            getchar();

            pasteText(text_lines, line_count, lineNumber - 1, index);
            addCommandToHistory("paste", clipboard, lineNumber - 1, index);
            }
        else if (strcmp(command, "13") == 0) {
            int lineNumber, index;
            printf("Enter the line number and index to insert with replacement: ");
            scanf_s("%d %d", &lineNumber, &index);
            getchar();

            printf("Enter text to insert: ");
            char text[1024];
            fgets(text, 1024, stdin);
            text[strcspn(text, "\n")] = '\0';

            insertWithReplacement(text_lines, line_count, lineNumber - 1, index, text);
            addCommandToHistory("replace", text, lineNumber - 1, index);
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

void cutText(char**& lines, int& lineCount, int lineNumber, int index, int numChars) {
    if (lineNumber < 0 || lineNumber >= lineCount || index < 0 || index >= strlen(lines[lineNumber])) {
        printf("Invalid line number or index.\n");
        return;
    }

    strncpy_s(clipboard, lines[lineNumber] + index, numChars);
    clipboard[numChars] = '\0';

    deleteText(lines, lineCount, lineNumber, index, numChars);
}

void copyText(char** lines, int lineNumber, int index, int numChars) {
    if (lineNumber < 0 || lineNumber >= line_count || index < 0 || index >= strlen(lines[lineNumber])) {
        printf("Invalid line number or index.\n");
        return;
    }

    strncpy_s(clipboard, lines[lineNumber] + index, numChars);
    clipboard[numChars] = '\0';
}

void pasteText(char**& lines, int& lineCount, int lineNumber, int index) {
    insertTextAt(lines, lineCount, lineNumber, index, clipboard);
}

void clearConsole() {
    system(CLEAR_COMMAND);
}

int text_input() {
    char text[1024];
    printf("Enter text (type 'end' to finish):\n");
    while (true) {
        if (!fgets(text, 1024, stdin)) break;
        text[strcspn(text, "\n")] = '\0';
        if (strcmp(text, "end") == 0) {
            break;
        }

        text_lines = (char**)realloc(text_lines, (line_count + 1) * sizeof(char*));
        text_lines[line_count] = (char*)malloc((strlen(text) + 1) * sizeof(char));
        strcpy(text_lines[line_count], strlen(text) + 1, text);
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
            strcpy(text_lines[line_count], strlen(buffer) + 1, buffer);
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
    std::memcpy(line + index, text, textLength);
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

void deleteText(char**& lines, int& lineCount, int lineNumber, int index, int numChars) {
    if (lineNumber < 0 || lineNumber >= lineCount) {
        fprintf(stderr, "This line is not in the text\n");
        return;
    }

    char* line = lines[lineNumber];
    int lineLength = (int)strlen(line);

    if (index < 0 || index >= lineLength) {
        fprintf(stderr, "This index is not in the text\n");
        return;
    }

    if (index + numChars > lineLength) {
        numChars = lineLength - index;
    }

    memmove(line + index, line + index + numChars, lineLength - index - numChars + 1);
}
void insertWithReplacement(char**& lines, int& lineCount, int lineNumber, int index, const char* text) {
    int length = strlen(text);
    deleteText(lines, lineCount, lineNumber, index, length);
    insertTextAt(lines, lineCount, lineNumber, index, text);
}