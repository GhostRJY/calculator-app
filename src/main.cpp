#include <calculator_math/calculator_math.h>

#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace calculator_app {

enum Operation {
  operationUnknown,
  operationAdd,
  operationSubtract,
  operationMultiply,
  operationDivide,
  operationPower,
  operationFactorial
};

enum TaskStatus {
  taskStatusOk,
  taskStatusHelp,
  taskStatusParseError,
  taskStatusUnknownOperation,
  taskStatusMissingArgument,
  taskStatusCalculationError
};

struct Task {
  int firstNumber_;
  int secondNumber_;
  int integerResult_;
  double floatingResult_;
  Operation operation_;
  TaskStatus status_;
  bool firstNumberSet_;
  bool secondNumberSet_;
  bool floatingResultUsed_;
};

static constexpr int successCode_ = 0;
static constexpr int errorCode_ = 1;
static constexpr int decimalBase_ = 10;
static constexpr int getoptEnd_ = -1;

static constexpr int optionHelp_ = 'h';
static constexpr int optionOperation_ = 'o';
static constexpr int optionFirstNumber_ = 'a';
static constexpr int optionSecondNumber_ = 'b';

void printHelp(const char *appName) {

  printf(
      "Usage:\n"
      "  %s --operation <op> --first <number> --second <number>\n"
      "  %s --operation fact --first <number>\n\n"
      "Options:\n"
      "  -h, --help                 Show help\n"
      "  -o, --operation, --op      Operation: add, sub, mul, div, pow, fact\n"
      "  -a, --first                First integer number\n"
      "  -b, --second               Second integer number\n\n"
      "Examples:\n"
      "  %s --op add -a 2 -b 3\n"
      "  %s --op div -a 5 -b 2\n"
      "  %s --op fact -a 5\n",
      appName, appName, appName, appName, appName);
}

Operation parseOperation(const char *text) {
  if (text == nullptr) {
    return operationUnknown;
  }

  if (strcmp(text, "add") == successCode_) {
    return operationAdd;
  }

  if (strcmp(text, "sub") == successCode_) {
    return operationSubtract;
  }

  if (strcmp(text, "mul") == successCode_) {
    return operationMultiply;
  }

  if (strcmp(text, "div") == successCode_) {
    return operationDivide;
  }

  if (strcmp(text, "pow") == successCode_) {
    return operationPower;
  }

  if (strcmp(text, "fact") == successCode_) {
    return operationFactorial;
  }

  return operationUnknown;
}

bool parseInteger(const char *text, int *value) {
  if (text == nullptr || value == nullptr || text[0] == '\0') {
    return false;
  }

  errno = successCode_;

  char *end = nullptr;
  const long parsedValue = strtol(text, &end, decimalBase_);

  if (errno == ERANGE || end == text || *end != '\0') {
    return false;
  }

  if (parsedValue > INT_MAX || parsedValue < INT_MIN) {
    return false;
  }

  *value = static_cast<int>(parsedValue);
  return true;
}

Task makeEmptyTask() {
  Task task{};

  task.firstNumber_ = 0;
  task.secondNumber_ = 0;
  task.integerResult_ = 0;
  task.floatingResult_ = 0.0;
  task.operation_ = operationUnknown;
  task.status_ = taskStatusOk;
  task.firstNumberSet_ = false;
  task.secondNumberSet_ = false;
  task.floatingResultUsed_ = false;

  return task;
}

Task makeTask(int argc, char **argv) {
  Task task = makeEmptyTask();

  static const option longOptions[] = {
      {"help", no_argument, nullptr, optionHelp_},
      {"operation", required_argument, nullptr, optionOperation_},
      {"op", required_argument, nullptr, optionOperation_},
      {"first", required_argument, nullptr, optionFirstNumber_},
      {"second", required_argument, nullptr, optionSecondNumber_},
      {nullptr, successCode_, nullptr, successCode_},
  };

  opterr = successCode_;
  optind = 1;

  while (true) {
    int optionIndex = 0;
    const int optionValue =
        getopt_long(argc, argv, "ho:a:b:", longOptions, &optionIndex);

    if (optionValue == getoptEnd_) {
      break;
    }

    switch (optionValue) {
    case optionHelp_:
      task.status_ = taskStatusHelp;
      return task;

    case optionOperation_:
      task.operation_ = parseOperation(optarg);

      if (task.operation_ == operationUnknown) {
        task.status_ = taskStatusUnknownOperation;
        return task;
      }

      break;

    case optionFirstNumber_:
      if (!parseInteger(optarg, &task.firstNumber_)) {
        task.status_ = taskStatusParseError;
        return task;
      }

      task.firstNumberSet_ = true;
      break;

    case optionSecondNumber_:
      if (!parseInteger(optarg, &task.secondNumber_)) {
        task.status_ = taskStatusParseError;
        return task;
      }

      task.secondNumberSet_ = true;
      break;

    default:
      task.status_ = taskStatusParseError;
      return task;
    }
  }

  if (task.operation_ == operationUnknown) {
    task.status_ = taskStatusUnknownOperation;
    return task;
  }

  if (!task.firstNumberSet_) {
    task.status_ = taskStatusMissingArgument;
    return task;
  }

  if (task.operation_ != operationFactorial && !task.secondNumberSet_) {
    task.status_ = taskStatusMissingArgument;
    return task;
  }

  return task;
}

bool makeCalculate(Task *task) {
  if (task == nullptr) {
    return false;
  }

  bool calculationStatus = false;

  switch (task->operation_) {
  case operationAdd:
    calculationStatus = calculator_math::add(
        task->firstNumber_, task->secondNumber_, &task->integerResult_);
    task->floatingResultUsed_ = false;
    break;

  case operationSubtract:
    calculationStatus = calculator_math::subtract(
        task->firstNumber_, task->secondNumber_, &task->integerResult_);
    task->floatingResultUsed_ = false;
    break;

  case operationMultiply:
    calculationStatus = calculator_math::multiply(
        task->firstNumber_, task->secondNumber_, &task->integerResult_);
    task->floatingResultUsed_ = false;
    break;

  case operationDivide:
    calculationStatus = calculator_math::divide(
        task->firstNumber_, task->secondNumber_, &task->floatingResult_);
    task->floatingResultUsed_ = true;
    break;

  case operationPower:
    calculationStatus = calculator_math::power(
        task->firstNumber_, task->secondNumber_, &task->integerResult_);
    task->floatingResultUsed_ = false;
    break;

  case operationFactorial:
    calculationStatus =
        calculator_math::factorial(task->firstNumber_, &task->integerResult_);
    task->floatingResultUsed_ = false;
    break;

  default:
    task->status_ = taskStatusUnknownOperation;
    return false;
  }

  if (!calculationStatus) {
    task->status_ = taskStatusCalculationError;
    return false;
  }

  task->status_ = taskStatusOk;
  return true;
}

void printResult(const Task *task) {
  if (task == nullptr) {
    return;
  }

  if (task->floatingResultUsed_) {

    printf("%g\n", task->floatingResult_);
    return;
  }

  printf("%d\n", task->integerResult_);
}

const char *getErrorMessage(TaskStatus status) {
  switch (status) {
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

void printError(TaskStatus status) {
  fputs(getErrorMessage(status), stderr);
  fputc('\n', stderr);
}

const char *getApplicationName(int argc, char **argv) {
  if (argc <= successCode_ || argv == nullptr) {
    return "calculator";
  }

  if (argv[0] == nullptr) {
    return "calculator";
  }

  return argv[0];
}

int applicationRun(int argc, char **argv) {
  const char *appName = getApplicationName(argc, argv);
  Task task = makeTask(argc, argv);

  if (task.status_ == taskStatusHelp) {
    printHelp(appName);
    return successCode_;
  }

  if (task.status_ != taskStatusOk) {
    printError(task.status_);
    printHelp(appName);
    return errorCode_;
  }

  if (!makeCalculate(&task)) {
    printError(task.status_);
    return errorCode_;
  }

  printResult(&task);
  return successCode_;
}

} // namespace calculator_app

int main(int argc, char **argv) {
  return calculator_app::applicationRun(argc, argv);
}