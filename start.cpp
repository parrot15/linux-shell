#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>

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
  Shell shell;

  for (;;) {
    display_prompt();

    string input_command;
    getline(cin, input_command);  // Get a line from standard input
    bool is_valid_input = shell.interpret_command(input_command);
    if (!is_valid_input) {
      continue;
    }
  }
}
