#include <calculator_math/calculator_math.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace calculator_app
{

enum Operation
{
    operationUnknown,
    operationAdd,
    operationSubtract,
    operationMultiply,
    operationDivide,
    operationPower,
    operationFactorial
};

enum TaskStatus
{
    taskStatusOk,
    taskStatusHelp,
    taskStatusParseError,
    taskStatusUnknownOperation,
    taskStatusMissingArgument,
    taskStatusCalculationError
};

struct Task
{
    int firstNumber;
    int secondNumber;
    int integerResult;
    double floatingResult;
    Operation operation;
    TaskStatus status;
    bool firstNumberSet;
    bool secondNumberSet;
};

static constexpr int successCode = 0;
static constexpr int errorCode = 1;
static constexpr int decimalBase = 10;
static constexpr int getoptEnd = -1;

static constexpr int optionHelp = 'h';
static constexpr int optionOperation = 'o';
static constexpr int optionFirstNumber = 'a';
static constexpr int optionSecondNumber = 'b';

void printHelp(const char* appName)
{

    printf("Usage:\n"
           "  %s --operation <op> --first <number> --second <number>\n"
           "  %s --operation fact --first <number>\n\n"
           "Options:\n"
           "  -h, --help                 Show help\n"
           "  -o, --operation, --op      Operation: add, sub, mul, div, pow, "
           "fact\n"
           "  -a, --first                First integer number\n"
           "  -b, --second               Second integer number\n\n"
           "Examples:\n"
           "  %s --op add -a 2 -b 3\n"
           "  %s --op div -a 5 -b 2\n"
           "  %s --op fact -a 5\n",
           appName, appName, appName, appName, appName);
}

Operation parseOperation(const char* text)
{
    if (text == nullptr)
    {
        return operationUnknown;
    }

    if (strcmp(text, "add") == successCode)
    {
        return operationAdd;
    }

    if (strcmp(text, "sub") == successCode)
    {
        return operationSubtract;
    }

    if (strcmp(text, "mul") == successCode)
    {
        return operationMultiply;
    }

    if (strcmp(text, "div") == successCode)
    {
        return operationDivide;
    }

    if (strcmp(text, "pow") == successCode)
    {
        return operationPower;
    }

    if (strcmp(text, "fact") == successCode)
    {
        return operationFactorial;
    }

    return operationUnknown;
}

bool parseInteger(const char* text, int* value)
{
    if (text == nullptr || value == nullptr || text[0] == '\0')
    {
        return false;
    }

    errno = successCode;

    char* end = nullptr;
    const long parsedValue = strtol(text, &end, decimalBase);

    if (errno == ERANGE || end == text || *end != '\0')
    {
        return false;
    }

    if (parsedValue > INT_MAX || parsedValue < INT_MIN)
    {
        return false;
    }

    *value = static_cast<int>(parsedValue);
    return true;
}

Task makeEmptyTask()
{
    Task task{};

    task.firstNumber = 0;
    task.secondNumber = 0;
    task.integerResult = 0;
    task.floatingResult = 0.0;
    task.operation = operationUnknown;
    task.status = taskStatusOk;
    task.firstNumberSet = false;
    task.secondNumberSet = false;

    return task;
}

Task makeTask(int argc, char** argv)
{
    Task task = makeEmptyTask();

    static const option longOptions_[] = {
        {"help", no_argument, nullptr, optionHelp},
        {"operation", required_argument, nullptr, optionOperation},
        {"op", required_argument, nullptr, optionOperation},
        {"first", required_argument, nullptr, optionFirstNumber},
        {"second", required_argument, nullptr, optionSecondNumber},
        {nullptr, successCode, nullptr, successCode},
    };

    opterr = successCode;
    optind = 1;

    while (true)
    {
        int optionIndex = 0;
        const int optionValue =
            getopt_long(argc, argv, "ho:a:b:", longOptions_, &optionIndex);

        if (optionValue == getoptEnd)
        {
            break;
        }

        switch (optionValue)
        {
            case optionHelp:
                task.status = taskStatusHelp;
                return task;

            case optionOperation:
                task.operation = parseOperation(optarg);

                if (task.operation == operationUnknown)
                {
                    task.status = taskStatusUnknownOperation;
                    return task;
                }

                break;

            case optionFirstNumber:
                if (!parseInteger(optarg, &task.firstNumber))
                {
                    task.status = taskStatusParseError;
                    return task;
                }

                task.firstNumberSet = true;
                break;

            case optionSecondNumber:
                if (!parseInteger(optarg, &task.secondNumber))
                {
                    task.status = taskStatusParseError;
                    return task;
                }

                task.secondNumberSet = true;
                break;

            default:
                task.status = taskStatusParseError;
                return task;
        }
    }

    if (task.operation == operationUnknown)
    {
        task.status = taskStatusUnknownOperation;
        return task;
    }

    if (!task.firstNumberSet)
    {
        task.status = taskStatusMissingArgument;
        return task;
    }

    if (task.operation != operationFactorial && !task.secondNumberSet)
    {
        task.status = taskStatusMissingArgument;
        return task;
    }

    return task;
}

bool makeCalculate(Task* task)
{
    if (task == nullptr)
    {
        return false;
    }

    bool calculationStatus = false;

    switch (task->operation)
    {
        case operationAdd:
            calculationStatus = calculator_math::add(
                task->firstNumber, task->secondNumber, &task->integerResult);

            break;

        case operationSubtract:
            calculationStatus = calculator_math::subtract(
                task->firstNumber, task->secondNumber, &task->integerResult);

            break;

        case operationMultiply:
            calculationStatus = calculator_math::multiply(
                task->firstNumber, task->secondNumber, &task->integerResult);

            break;

        case operationDivide:
            calculationStatus = calculator_math::divide(
                task->firstNumber, task->secondNumber, &task->integerResult);

            break;

        case operationPower:
            calculationStatus = calculator_math::power(
                task->firstNumber, task->secondNumber, &task->integerResult);

            break;

        case operationFactorial:
            calculationStatus = calculator_math::factorial(
                task->firstNumber, &task->integerResult);

            break;

        default:
            task->status = taskStatusUnknownOperation;
            return false;
    }

    if (!calculationStatus)
    {
        task->status = taskStatusCalculationError;
        return false;
    }

    task->status = taskStatusOk;
    return true;
}

void printResult(const Task* task)
{
    if (task == nullptr)
    {
        return;
    }

    printf("%d\n", task->integerResult);
}

const char* getErrorMessage(TaskStatus status)
{
    switch (status)
    {
        case taskStatusParseError:
            return "Error: failed to parse arguments";

        case taskStatusUnknownOperation:
            return "Error: unknown operation";

        case taskStatusMissingArgument:
            return "Error: missing required argument";

        case taskStatusCalculationError:
            return "Error: calculation failed";

        default:
            return "Error: unexpected application state";
    }
}

void printError(TaskStatus status)
{
    fputs(getErrorMessage(status), stderr);
    fputc('\n', stderr);
}

const char* getApplicationName(int argc, char** argv)
{
    if (argc <= successCode || argv == nullptr)
    {
        return "calculator";
    }

    if (argv[0] == nullptr)
    {
        return "calculator";
    }

    return argv[0];
}

int applicationRun(int argc, char** argv)
{
    const char* appName = getApplicationName(argc, argv);
    Task task = makeTask(argc, argv);

    if (task.status == taskStatusHelp)
    {
        printHelp(appName);
        return successCode;
    }

    if (task.status != taskStatusOk)
    {
        printError(task.status);
        printHelp(appName);
        return errorCode;
    }

    if (!makeCalculate(&task))
    {
        printError(task.status);
        return errorCode;
    }

    printResult(&task);
    return successCode;
}

} // namespace calculator_app

int main(int argc, char** argv)
{
    return calculator_app::applicationRun(argc, argv);
}