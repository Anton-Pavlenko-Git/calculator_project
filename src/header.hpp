#pragma once

struct Number {
    int first{0};
    int second{0};
    std::string operation;
    long long result{0};
};

void runCalculator(int argc, char* argv[]);
int argsCheck(int argc, char* argv[]);
void print_help();
int parse(int argc, char* argv[], Number& num);
int calc(Number& num);
void printResult(Number& num);