#include "shell.h"

Shell::Shell()
    : should_run_in_background(false),
      previous_directory(get_current_directory()) {}

bool Shell::interpret_command(const std::string& raw_command) {
  // Clean up child processes
  for (size_t i = 0; i < background_pids.size(); ++i) {
    pid_t child_pid = background_pids.at(i);
    pid_t terminated_child_pid = waitpid(child_pid, 0, WNOHANG);
    if (terminated_child_pid == child_pid) {
      background_pids.erase(background_pids.begin() + i);
      --i;
    }
  }

  std::vector<std::string> command_tokens = parser.tokenize(raw_command);
  if (!parser.is_valid_input()) {
    return false;
  }
  if (command_tokens.empty()) {
    return true;
  }

  if (is_builtin_command(command_tokens)) {
    execute_builtin_command(command_tokens);
    return true;
  }

  should_run_in_background = parser.is_marked_for_background(command_tokens);
  if (should_run_in_background) {
    command_tokens.pop_back();
  }

  std::vector<std::vector<std::string>> pipe_slices =
      parser.split_by_pipe(command_tokens);
  execute_pipe_slices(pipe_slices);

  return true;
}

int Shell::execute_pipe_slices(
    const std::vector<std::vector<std::string>>& pipe_slices) {
  // Save a copy of the original stdin file descriptor
  int original_stdin_fd = dup(STDIN_FILENO);
  int original_stdout_fd = dup(STDOUT_FILENO);

  for (size_t i = 0; i < pipe_slices.size(); ++i) {
    std::vector<std::string> pipe_slice = pipe_slices.at(i);

    // Create the pipe
    int pipe_fds[2];
    pipe(pipe_fds);

    // Create the child process
    pid_t child_pid = fork();
    if (child_pid == 0) {  // If child
      if (parser.has_io_redirection(pipe_slice)) {
        std::vector<std::pair<std::string, std::string>> redirect_pairings =
            parser.get_io_redirection_pairings(pipe_slice);
        execute_io_redirection(redirect_pairings);
        pipe_slice = parser.get_io_redirection_command(pipe_slice);
      }

      // Make child processes write to pipe instead of stdout
      // except for last one. The last one should write to stdout.
      if (i < pipe_slices.size() - 1) {
        dup2(pipe_fds[1], STDOUT_FILENO);
        close(pipe_fds[1]);
      }
      int error_num = execute_command(pipe_slice);
      if (error_num != 0) {
        std::cout << "command not found: " << pipe_slice.at(0) << std::endl;
        return error_num;
      }
    } else {  // If parent
      if (!should_run_in_background && i == pipe_slices.size() - 1) {
        waitpid(child_pid, 0, 0);
      } else {
        background_pids.push_back(child_pid);
      }

      // Make parent process read from pipe instead of stdin
      dup2(pipe_fds[0], STDIN_FILENO);
      close(pipe_fds[1]);
    }
  }

  // Replace current stdin file descriptor with original value
  dup2(original_stdin_fd, STDIN_FILENO);
  dup2(original_stdout_fd, STDOUT_FILENO);
}

int Shell::execute_io_redirection(
    const std::vector<std::pair<std::string, std::string>>& redirect_pairings)
    const {
  for (const auto& redirect_pairing : redirect_pairings) {
    std::string io_redirect = redirect_pairing.first;
    std::string filename = redirect_pairing.second;

    if (io_redirect == parser.INPUT_REDIRECT) {  // If input redirect
      int filename_fd = open(filename.c_str(), O_RDONLY,
                             S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
      dup2(filename_fd, STDIN_FILENO);
      close(filename_fd);
    } else if (io_redirect == parser.OUTPUT_REDIRECT) {  // If output redirect
      int filename_fd = open(filename.c_str(), O_CREAT | O_WRONLY | O_TRUNC,
                             S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
      dup2(filename_fd, STDOUT_FILENO);
      close(filename_fd);
    }
  }

  return 0;
}

bool Shell::is_builtin_command(
    const std::vector<std::string>& command_tokens) const {
  std::string command = command_tokens.at(0);
  return command == CHANGE_DIRECTORY || command == EXIT_SHELL;
}

int Shell::execute_builtin_command(
    const std::vector<std::string>& command_tokens) {
  std::string command = command_tokens.at(0);
  if (command == CHANGE_DIRECTORY) {
    return change_current_directory(command_tokens);
  } else if (command == EXIT_SHELL) {
    return exit_shell();
  }
}

int Shell::execute_command(const std::vector<std::string>& command_tokens) {
  if (command_tokens.empty()) {
    return 0;
  }

  char** args = new char*[command_tokens.size() + 1];
  for (int i = 0; i < command_tokens.size(); ++i) {
    args[i] = (char*)command_tokens[i].c_str();
  }
  args[command_tokens.size()] = nullptr;

  return execvp(args[0], args);
}

int Shell::change_current_directory(
    const std::vector<std::string>& command_tokens) {
  if (command_tokens.size() > 2) {
    std::cout << CHANGE_DIRECTORY << ": too many arguments" << std::endl;
    return 0;
  }

  int error_num = 0;
  std::string command = command_tokens.at(0);
  std::string current_directory = get_current_directory();
  if (command != CHANGE_DIRECTORY) {
    std::stringstream error_message;
    error_message << "The first token must be <" << CHANGE_DIRECTORY << ">";
    throw std::runtime_error(error_message.str());
  }

  if (command_tokens.size() == 1) {
    const char* home_directory;
    if ((home_directory = getenv("HOME")) == nullptr) {
      home_directory = getpwuid(getuid())->pw_dir;
    }
    error_num = chdir(home_directory);
  } else {
    std::string destination = command_tokens.at(1);
    if (destination == "-") {
      std::cout << previous_directory << std::endl;
      error_num = chdir(previous_directory.c_str());
    } else if (destination == ".") {
      error_num = chdir(".");
    } else if (destination == "..") {
      error_num = chdir("..");
    } else {
      error_num = chdir(destination.c_str());
    }
  }

  if (error_num != 0) {
    std::cout << CHANGE_DIRECTORY << ": " << strerror(errno) << std::endl;
    return error_num;
  }

  previous_directory = current_directory;

  return 0;
}

std::string Shell::get_current_directory() {
  char current_directory_buffer[100];
  getcwd(current_directory_buffer, sizeof(current_directory_buffer));

  return {current_directory_buffer};
}

int Shell::exit_shell() {
  std::cout << "Shell exited" << std::endl;
  exit(0);
  return 0;
}