#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <utility>
#include <cstring>

#define CLEAR_COMMAND "cls"

class TextEditor {
private:
    char** text_lines;
    int line_count;
    char clipboard[1024];
    struct Command {
        char name[10];
        char text[1024];
        int lineNumber;
        int index;
        int numChars;
    };
    Command commandHistory[3];
    int commandIndex;
    Command redoHistory[3];
    int redoIndex;
    bool redoAvailable;

public:
    TextEditor() : text_lines(nullptr), line_count(0), commandIndex(0), redoIndex(0), redoAvailable(false) {
        std::memset(clipboard, 0, sizeof(clipboard));
    }

    ~TextEditor() {
        freeTextLines();
    }

    int append_text() {
        char text[1024];
        printf("Enter text to append (type 'end' on a new line to finish):\n");
        while (true) {
            if (!fgets(text, 1024, stdin)) break;
            text[strcspn(text, "\n")] = '\0';
            if (strcmp(text, "end") == 0) {
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
            freeTextLines();
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

    void insertTextAt(int lineNumber, int index, const char* text) {
        if (lineNumber < 0 || lineNumber >= line_count) {
            fprintf(stderr, "Line number out of range\n");
            return;
        }

        char* line = text_lines[lineNumber];
        int lineLength = (int)strlen(line);
        int textLength = (int)strlen(text);

        if (index < 0 || index > lineLength) {
            fprintf(stderr, "Index out of range\n");
            return;
        }

        line = (char*)realloc(line, (lineLength + textLength + 1) * sizeof(char));
        memmove(line + index + textLength, line + index, lineLength - index + 1);
        std::memcpy(line + index, text, textLength);
        text_lines[lineNumber] = line;
    }

    std::pair<int, int>* searchText(const char* searchText, int& resultCount) {
        resultCount = 0;
        std::pair<int, int>* positions = nullptr;
        size_t searchLength = strlen(searchText);

        for (int i = 0; i < line_count; ++i) {
            const char* line = text_lines[i];
            for (const char* pos = strstr(line, searchText); pos; pos = strstr(pos + 1, searchText)) {
                positions = (std::pair<int, int>*)realloc(positions, (resultCount + 1) * sizeof(std::pair<int, int>));
                positions[resultCount] = std::make_pair(i, (int)(pos - line));
                ++resultCount;
            }
        }

        return positions;
    }

    void clearConsole() {
        system(CLEAR_COMMAND);
    }

    void freeTextLines() {
        for (int i = 0; i < line_count; ++i) {
            free(text_lines[i]);
        }
        free(text_lines);
        text_lines = nullptr;
    }

    void deleteText(int lineNumber, int index, int numChars) {
        if (lineNumber < 0 || lineNumber >= line_count) {
            fprintf(stderr, "This line is not in the text\n");
            return;
        }

        char* line = text_lines[lineNumber];
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

    void insertWithReplacement(int lineNumber, int index, const char* text) {
        int length = strlen(text);
        deleteText(lineNumber, index, length);
        insertTextAt(lineNumber, index, text);
    }

    void cutText(int lineNumber, int index, int numChars) {
        if (lineNumber < 0 || lineNumber >= line_count || index < 0 || index >= strlen(text_lines[lineNumber])) {
            printf("Invalid line number or index.\n");
            return;
        }

        strncpy_s(clipboard, sizeof(clipboard), text_lines[lineNumber] + index, numChars);
        clipboard[numChars] = '\0';

        deleteText(lineNumber, index, numChars);
    }

    void copyText(int lineNumber, int index, int numChars) {
        if (lineNumber < 0 || lineNumber >= line_count || index < 0 || index >= strlen(text_lines[lineNumber])) {
            printf("Invalid line number or index.\n");
            return;
        }

        strncpy_s(clipboard, sizeof(clipboard), text_lines[lineNumber] + index, numChars);
        clipboard[numChars] = '\0';
    }

    void pasteText(int lineNumber, int index) {
        insertTextAt(lineNumber, index, clipboard);
    }

    const char* getClipboard() const {
        return clipboard;
    }

    void addCommandToHistory(const char* name, const char* text = "", int lineNumber = -1, int index = -1, int numChars = -1) {
        strcpy_s(commandHistory[commandIndex].name, sizeof(commandHistory[commandIndex].name), name);
        strcpy_s(commandHistory[commandIndex].text, sizeof(commandHistory[commandIndex].text), text);
        commandHistory[commandIndex].lineNumber = lineNumber;
        commandHistory[commandIndex].index = index;
        commandHistory[commandIndex].numChars = numChars;
        commandIndex = (commandIndex + 1) % 3;
    }

    void addCommandToRedoHistory(const char* name, const char* text = "", int lineNumber = -1, int index = -1, int numChars = -1) {
        strcpy_s(redoHistory[redoIndex].name, sizeof(redoHistory[redoIndex].name), name);
        strcpy_s(redoHistory[redoIndex].text, sizeof(redoHistory[redoIndex].text), text);
        redoHistory[redoIndex].lineNumber = lineNumber;
        redoHistory[redoIndex].index = index;
        redoHistory[redoIndex].numChars = numChars;
        redoIndex = (redoIndex + 1) % 3;
    }

    void undoLastCommand() {
        if (commandIndex == 0 && strlen(commandHistory[2].name) == 0) {
            printf("No commands to undo.\n");
            return;
        }

        Command lastCommand = commandHistory[(commandIndex + 2) % 3];

        if (strcmp(lastCommand.name, "insert") == 0) {
            deleteText(lastCommand.lineNumber, lastCommand.index, (int)strlen(lastCommand.text));
            addCommandToRedoHistory("insert", lastCommand.text, lastCommand.lineNumber, lastCommand.index);
        }
        else if (strcmp(lastCommand.name, "delete") == 0) {
            insertTextAt(lastCommand.lineNumber, lastCommand.index, lastCommand.text);
            addCommandToRedoHistory("delete", "", lastCommand.lineNumber, lastCommand.index, lastCommand.numChars);
        }
        else if (strcmp(lastCommand.name, "cut") == 0) {
            insertTextAt(lastCommand.lineNumber, lastCommand.index, clipboard);
            addCommandToRedoHistory("cut", "", lastCommand.lineNumber, lastCommand.index, lastCommand.numChars);
        }
        else if (strcmp(lastCommand.name, "paste") == 0) {
            deleteText(lastCommand.lineNumber, lastCommand.index, (int)strlen(clipboard));
            addCommandToRedoHistory("paste", clipboard, lastCommand.lineNumber, lastCommand.index);
        }
        strcpy_s(commandHistory[commandIndex].name, sizeof(commandHistory[commandIndex].name), "");
        redoAvailable = true;
    }

    void redoLastCommand() {
        if (!redoAvailable) {
            printf("No commands to redo.\n");
            return;
        }

        Command lastRedoCommand = redoHistory[(redoIndex + 2) % 3];

        if (strcmp(lastRedoCommand.name, "insert") == 0) {
            insertTextAt(lastRedoCommand.lineNumber, lastRedoCommand.index, lastRedoCommand.text);
        }
        else if (strcmp(lastRedoCommand.name, "delete") == 0) {
            deleteText(lastRedoCommand.lineNumber, lastRedoCommand.index, lastRedoCommand.numChars);
        }
        else if (strcmp(lastRedoCommand.name, "cut") == 0) {
            deleteText(lastRedoCommand.lineNumber, lastRedoCommand.index, lastRedoCommand.numChars);
        }
        else if (strcmp(lastRedoCommand.name, "paste") == 0) {
            pasteText(lastRedoCommand.lineNumber, lastRedoCommand.index);
        }

        strcpy_s(redoHistory[(redoIndex + 2) % 3].name, sizeof(redoHistory[(redoIndex + 2) % 3].name), "");
        redoAvailable = false;
    }

    void displayMenu() {
        printf("\n--- Text Editor Menu ---\n");
        printf("1. Insert text\n");
        printf("2. Delete text\n");
        printf("3. Cut text\n");
        printf("4. Copy text\n");
        printf("5. Paste text\n");
        printf("6. Undo last command\n");
        printf("7. Redo last command\n");
        printf("8. View text\n");
        printf("9. New line\n");
        printf("10. Exit\n");
    }

    void viewText() {
        printf("Current Text\n");
        for (int i = 0; i < line_count; ++i) {
            printf("%d: %s\n", i + 1, text_lines[i]);
        }
    }
};

int main() {
    TextEditor editor;

    int choice;
    do {
        editor.displayMenu();
        printf("Enter the command you want to do: ");
        scanf_s("%d", &choice);
        getchar();

        int lineNumber, index, numChars;
        char text[1024];
        switch (choice) {
        case 1:
            printf("Enter line number, index and text to insert: ");
            scanf_s("%d %d", &lineNumber, &index);
            getchar();
            fgets(text, 1024, stdin);
            text[strcspn(text, "\n")] = '\0';
            editor.insertTextAt(lineNumber, index, text);
            editor.addCommandToHistory("insert", text, lineNumber, index);
            break;
        case 2:
            printf("Enter line number, index and number of characters to delete: ");
            scanf_s("%d %d %d", &lineNumber, &index, &numChars);
            getchar();
            editor.deleteText(lineNumber, index, numChars);
            editor.addCommandToHistory("delete", "", lineNumber, index, numChars);
            break;
        case 3:
            printf("Enter line number, index and number of characters to cut: ");
            scanf_s("%d %d %d", &lineNumber, &index, &numChars);
            getchar();
            editor.cutText(lineNumber, index, numChars);
            editor.addCommandToHistory("cut", "", lineNumber, index, numChars);
            break;
        case 4:
            printf("Enter line number, index and number of characters to copy: ");
            scanf_s("%d %d %d", &lineNumber, &index, &numChars);
            getchar();
            editor.copyText(lineNumber, index, numChars);
            editor.addCommandToHistory("copy", "", lineNumber, index, numChars);
            break;
        case 5:
            printf("Enter line number and index to paste: ");
            scanf_s("%d %d", &lineNumber, &index);
            getchar();
            editor.pasteText(lineNumber, index);
            editor.addCommandToHistory("paste", editor.getClipboard(), lineNumber, index);
            break;
        case 6:
            editor.undoLastCommand();
            break;
        case 7:
            editor.redoLastCommand();
            break;
        case 8:
            editor.viewText();
            break;
        case 9:
            editor.append_text();
            break;
        case 10:
            break;
        default:
            printf("This command is not here\n");
            break;
        }
    } while (choice != 10);

    return 0;
}
