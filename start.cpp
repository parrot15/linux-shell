// #include <fcntl.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/stat.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// #include <unistd.h>

#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>

// #include "parser.h"
#include "shell.h"

using namespace std;

void display_prompt() {
  char current_user_buffer[30];
  getlogin_r(current_user_buffer, sizeof(char) * 30);

  auto current_time =
      chrono::system_clock::to_time_t(chrono::system_clock::now());
  auto current_date = strtok(ctime(&current_time), "\n");

  cout << string(current_user_buffer) << ": " << current_date << "$ " << flush;
}

int main() {
  // Parser parser;
  Shell shell;
  // vector<pid_t> child_pids;

  for (;;) {
    display_prompt();

    // order of precedence (lowest to highest):
    //     ""/'' --> & --> >/< --> | --> $
    // steps:
    //     1. tokenize command by whitespace and ""/''
    //     2. if last token is &, remove it and mark entire command
    //        as background process
    //     3. split tokens by |
    //     4. split tokens by >/<

    string input_command;
    getline(cin, input_command);  // get a line from standard input
    bool is_valid_input = shell.interpret_command(input_command);
    if (!is_valid_input) {
      cout << "Skipping..." << endl;
      continue;
    }

    // // 1.
    // string input_command;
    // getline(cin, input_command);  // get a line from standard input
    // vector<string> command_tokens = parser.tokenize(input_command);
    // if (!parser.is_valid_input()) {
    //   cout << "Skipping..." << endl;
    //   continue;
    // }

    // // 2 - 3
    // shell.interpret_command(command_tokens);

    // // IO redirect (>/<) has higher precedence than pipe (|)

    // // take the tokens and split them up by pipes

    // // for each pipe split, split by IO redirect

    // // if "$(" is encountered, we have an expansion. Keep reading
    // // in characters until ")" is encountered

    // if (input_command == string("exit")) {
    //   cout << "Shell exited" << endl;
    //   break;
    // }

    // pid_t pid = fork();
    // if (pid == 0) {  // child process
    //   // preparing the input command for execution
    //   char* args[] = {(char*)input_command.c_str(), NULL};
    //   execvp(args[0], args);
    // } else {
    //   // child_pids.push_back(pid);
    //   waitpid(pid, 0, 0);  // wait for the child process
    //   // we will discuss why waitpid() is preferred over wait()
    // }
  }
}