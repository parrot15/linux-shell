#ifndef LINUX_SHELL_SHELL_H
#define LINUX_SHELL_SHELL_H

#include <fcntl.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "parser.h"

class Shell {
 public:
  Shell();

  /**
   * The main control flow of the shell. Determines if the input is valid
   * and subsequently parses the input by invoking the parser. Executes
   * the parsed input. Cleans up any background processes that have finished.
   * @param raw_command The raw user input to interpret, without any 
   * pre-processing.
   * @return True if the interpretation of the command was successful,
   * false if otherwise.
   */
  bool interpret_command(const std::string& raw_command);

 private:
  Parser parser;

  bool should_run_in_background;
  std::vector<pid_t> background_pids;

  std::string previous_directory;

  const std::string CHANGE_DIRECTORY = "cd";
  const std::string EXIT_SHELL = "exit";

  /**
   * A pipe slice is a section of a command containing pipes. For example,
   * the command:
   *   "ps -ef | head | grep -i rcu"
   * will be split into the following pipe slices:
   *   "ps -ef", "head", "grep -i rcu"
   * Each of these pipe slices will be executed, and their outputs will be
   * fed into the next slice. The output of the last slice will be fed to
   * stdout.
   * @param pipe_slices The list of pipe splices to execute.
   * @return -1 if a pipe slice failed to execute.
   */
  int execute_pipe_slices(
      const std::vector<std::vector<std::string>>& pipe_slices);

  /**
   * Establishes I/O redirections for a command by redirecting the command's
   * input or output to a file. For example, for an input redirect ("<"), it
   * will redirect the command to read from a file instead of stdin. For an
   * output redirect (">"), it will redirect the command to write to a file
   * instead of stdout.
   * @param redirect_pairings A list of all pairs of I/O redirections with
   * ther associated files.
   * @return 0 if successful.
   */
  int execute_io_redirection(
      const std::vector<std::pair<std::string, std::string>>& redirect_pairings)
      const;

  /**
   * Checks if a list of command tokens is a built-in command (a command
   * that the shell interprets).
   * @param command_tokens The list of tokens to check.
   * @return True if the tokenized command is a built-in command, false
   * otherwise.
   */
  bool is_builtin_command(const std::vector<std::string>& command_tokens) const;

  /**
   * Executes a built-in command (e.g. cd commands, exit).
   * @param command_tokens The tokenized command to execute.
   * @return -1 if the command failed to execute, 0 if interpretation of
   * the command was successful.
   */
  int execute_builtin_command(const std::vector<std::string>& command_tokens);

  /**
   * Executes a command (not a built-in command).
   * @param command_tokens The tokenized command to execute.
   * @return -1 if the command failed to execute, 0 if interpretation of
   * the command was successful.
   */
  static int execute_command(const std::vector<std::string>& command_tokens);

  /**
   * Changes the current working directory of the shell, either to the
   * current directory (e.g. "cd ."), the directory above (e.g. "cd .."),
   * or the most recent directory (e.g. "cd -").
   * @param command_tokens The change directory command to execute.
   * @return -1 if the command failed to execute, 0 if interpretation of
   * the command was sucessful.
   * @throw std::runtime_error if the first token is not "cd".
   */
  int change_current_directory(const std::vector<std::string>& command_tokens);

  /**
   * Gets the current working directory of the shell.
   * @return The current working directory.
   */
  static std::string get_current_directory();

  /**
   * Exits the shell
   * @return 0 if successful.
   */
  static int exit_shell();
};

#endif