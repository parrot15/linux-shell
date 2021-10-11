#include <stdio.h>
#include<iostream>
#include <vector>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <ctime>
#include <chrono>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "parser.h"

using namespace std;

void display_prompt() {
    char current_user_buffer[30];
    getlogin_r(current_user_buffer, sizeof(char) * 30);

    auto current_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
    auto current_date = strtok(ctime(&current_time), "\n");

    cout << string(current_user_buffer) << ": " << current_date << "$ " << flush;
}

int main() {
    Parser input_parser;
    while (true) {
        display_prompt();
        string input_line;
        getline(cin, input_line);  // get a line from standard input
        vector<string> input_tokens = input_parser.tokenize(input_line);
        if (input_line == string("exit")) {
            cout << "Shell exited" << endl;
            break;
        }
        int pid = fork();
        if (pid == 0) {  // child process
            // preparing the input command for execution
            char* args[] = {(char*) input_line.c_str(), NULL};
            execvp(args[0], args);
        } else {
            waitpid(pid, 0, 0);  // wait for the child process
            // we will discuss why waitpid() is preferred over wait()
        }
    }
}