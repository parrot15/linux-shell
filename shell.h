#ifndef PA2_IMPLEMENTING_A_LINUX_SHELL_PARROT15_SHELL_H
#define PA2_IMPLEMENTING_A_LINUX_SHELL_PARROT15_SHELL_H

#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "parser.h"

class Shell {
 public:
  Shell();

  bool interpret_command(const std::string& raw_command);

 private:
  Parser parser;
  bool should_run_in_background;
  std::vector<pid_t> background_pids;
  std::string previous_directory = ".";

  // enum BuiltinCommand { cd, exit };
  const std::string CHANGE_DIRECTORY = "cd";
  const std::string EXIT_SHELL = "exit";

  int execute_pipe_slices(
      const std::vector<std::vector<std::string>>& pipe_slices);
  int execute_io_redirection(
      const std::vector<std::pair<std::string, std::string>>& redirect_pairings)
      const;
  // int execute_io_redirection(std::vector<std::string>& command_slice);
  bool is_builtin_command(const std::vector<std::string>& command_tokens) const;
  int execute_builtin_command(const std::vector<std::string>& command_tokens);
  int execute_command(const std::vector<std::string>& command_slice);
  int change_current_directory(const std::vector<std::string>& command_tokens);
  int exit_shell(const std::vector<std::string>& command_tokens) const;
  // void execute_builtin_command(BuiltinCommand command);
};

#endif  // PA2_IMPLEMENTING_A_LINUX_SHELL_PARROT15_SHELL_H