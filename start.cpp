#include <chrono>
#include <ctime>
#include <iostream>

#include "shell.h"

/**
 * Displays the shell prompt to the user. The prompt contains the
 * name of the user and the current time.
 */
void display_prompt() {
  char current_user_buffer[30];
  getlogin_r(current_user_buffer, sizeof(char) * 30);

  auto current_time =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  auto current_date = strtok(ctime(&current_time), "\n");

  std::cout << std::string(current_user_buffer) << ": " << current_date << "$ " << std::flush;
}

int main() {
  Shell shell;

  for (;;) {
    display_prompt();

    std::string input_command;
    getline(std::cin, input_command);  // Get a line from standard input
    bool is_valid_input = shell.interpret_command(input_command);
    if (!is_valid_input) {
      continue;
    }
  }
}
