#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <windows.h>

#define SUCCESS 0
#define FAILURE 1
#define MAX_COMMAND_LENGTH 256
#define MAX_TIMESTAMP 30

typedef struct {
    char name[MAX_COMMAND_LENGTH];
    int exitCode;
    time_t executionTime;
} CommandResult;

void setConsoleCodePage() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    setlocale(LC_ALL, "bg_BG.UTF-8");
}

void printUsage(const char* programName) {
    printf("Употреба: %s <команда1> <команда2> <име_на_файл>\n", programName);
    printf("\nОписание:\n");
    printf("  Програмата изпълнява две команди последователно и\n");
    printf("  записва името на първата команда във файл, ако тя завърши успешно.\n");
    printf("\nПримери:\n");
    printf("  %s dir echo output.txt\n", programName);
    printf("  %s whoami hostname results.log\n", programName);
}

void getCurrentTimestamp(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

int checkAndCreateFile(const char* filename) {
    FILE* file = fopen(filename, "r");

    if (file != NULL) {
        // Файлът съществува
        printf("Информация: Файлът '%s' вече съществува.\n", filename);
        fclose(file);
        return 1;
    } else {
        // Файлът не съществува, опит за създаване
        file = fopen(filename, "w");
        if (file == NULL) {
            printf("Грешка: Не може да се създаде файл '%s'.\n", filename);
            return 0;
        }

        printf("Информация: Файлът '%s' беше създаден успешно.\n", filename);
        fclose(file);
        return 1;
    }
}

CommandResult executeCommand(const char* command) {
    CommandResult result;
    strncpy(result.name, command, MAX_COMMAND_LENGTH - 1);
    result.name[MAX_COMMAND_LENGTH - 1] = '\0'; // Осигуряваме, че низът е правилно терминиран
    result.executionTime = time(NULL);

    printf("Изпълнява се команда: %s\n", command);

    // Изпълняваме командата и запазваме резултата
    int returnCode = system(command);

    // За Windows среда, 0 означава успех
    if (returnCode == 0) {
        result.exitCode = SUCCESS;
    } else {
        result.exitCode = FAILURE;
    }

    return result;
}

int appendCommandToFile(const char* filename, const CommandResult* result) {
    FILE* file = fopen(filename, "a");

    if (file == NULL) {
        printf("Грешка: Не може да се отвори файл '%s' за запис.\n", filename);
        return 0;
    }

    char timestamp[MAX_TIMESTAMP];
    struct tm* tm_info = localtime(&result->executionTime);
    strftime(timestamp, MAX_TIMESTAMP, "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(file, "[%s] %s (Код: %d)\n", timestamp, result->name, result->exitCode);

    if (ferror(file)) {
        printf("Грешка: Проблем при запис във файл '%s'.\n", filename);
        fclose(file);
        return 0;
    }

    fclose(file);
    return 1;
}

void printCommandResult(const CommandResult* result) {
    char timestamp[MAX_TIMESTAMP];
    struct tm* tm_info = localtime(&result->executionTime);
    strftime(timestamp, MAX_TIMESTAMP, "%Y-%m-%d %H:%M:%S", tm_info);

    if (result->exitCode == SUCCESS) {
        printf("Успех: Командата '%s' е изпълнена успешно в %s.\n",
               result->name, timestamp);
    } else {
        printf("Грешка: Командата '%s' завърши неуспешно с код %d в %s.\n",
               result->name, result->exitCode, timestamp);
    }
}

int main(int argc, char* argv[]) {
    setConsoleCodePage();

    if (argc != 4) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    const char* command1 = argv[1];
    const char* command2 = argv[2];
    const char* filename = argv[3];

    char startTimestamp[MAX_TIMESTAMP];
    getCurrentTimestamp(startTimestamp, MAX_TIMESTAMP);

    printf("======= Стартиране на програмата в %s =======\n", startTimestamp);

    if (!checkAndCreateFile(filename)) {
        printf("Критична грешка: Неуспех при работа с файл '%s'. Програмата се прекратява.\n", filename);
        return EXIT_FAILURE;
    }

    printf("\nИнформация: Изпълнява се първа команда: %s\n", command1);

    CommandResult result1 = executeCommand(command1);
    printCommandResult(&result1);

    if (result1.exitCode == SUCCESS) {
        if (appendCommandToFile(filename, &result1)) {
            printf("Информация: Името на командата '%s' е добавено към файл '%s'\n",
                   command1, filename);
        }
    }

    printf("\nИнформация: Изпълнява се втора команда: %s\n", command2);

    CommandResult result2 = executeCommand(command2);
    printCommandResult(&result2);

    char endTimestamp[MAX_TIMESTAMP];
    getCurrentTimestamp(endTimestamp, MAX_TIMESTAMP);

    printf("\n======= Завършване на програмата в %s =======\n", endTimestamp);

    return EXIT_SUCCESS;
}