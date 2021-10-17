#include "shell.h"

using namespace std;

// get initial value for cd - in constructor?
// Shell::Shell() : should_run_in_background(false) {}

// destructor must clean up child background processes

bool Shell::interpret_command(const string& raw_command) {
  // order of precedence (lowest to highest):
  //     ""/'' --> & --> >/< --> | --> $
  // steps:
  //     1. tokenize command by whitespace and ""/''
  //     2. if last token is &, remove it and mark entire command
  //        as background process
  //     3. split tokens by |
  //     4. split tokens by >/<

  // Clean up child processes
  for (size_t i = 0; i < background_pids.size(); ++i) {
    pid_t child_pid = background_pids.at(i);
    pid_t terminated_child_pid = waitpid(child_pid, 0, WNOHANG);
    if (terminated_child_pid == child_pid) {
      background_pids.erase(background_pids.begin() + i);
      --i;
    }
  }

  // 1.
  // cout << "interpreting command: <" << raw_command << ">" << endl;
  vector<string> command_tokens = parser.tokenize(raw_command);
  if (!parser.is_valid_input()) {
    return false;
  }
  if (command_tokens.size() == 0) {
    return true;
  }

  // pid_t child_pid = fork();
  // if (child_pid == 0) {
  //   execute_command(command_tokens);
  // } else {
  //   background_pids.push_back(child_pid);
  //   waitpid(child_pid, 0, 0);
  // }

  // 2
  should_run_in_background = parser.is_marked_for_background(command_tokens);
  if (should_run_in_background) {
    command_tokens.pop_back();
  }
  // TODO: make it run in background

  // 3
  // take the tokens and slice them up by pipes
  vector<vector<string>> pipe_slices = parser.split_by_pipe(command_tokens);
  // pipe each slice into each other
  int error_num = execute_pipe_slices(pipe_slices);
  // if (error_num != 0) {
  //   return false;
  // }

  // for each pipe split, split by IO redirect

  // if "$(" is encountered, we have an expansion. Keep reading
  // in characters until ")" is encountered

  // execute_command(command_tokens);

  return true;
}

int Shell::execute_pipe_slices(const vector<vector<string>>& pipe_slices) {
  // cout << ">>>>> pipe slices >>>>>" << endl;
  // for (const auto& slice : pipe_slices) {
  //   for (const auto& token : slice) {
  //     cout << "<" << token << "> ";
  //   }
  //   cout << endl;
  // }
  // cout << "<<<<<" << endl;

  // cout << "--- pipe_fds ---" << endl;
  // for (size_t i = 0; i < 2; ++i) {
  //   cout << "<" << pipe_fds[i] << "> ";
  // }
  // cout << endl;
  // for (const auto& command_slice : pipe_slices) {
  // cout << "INSIDE EXECUTE PIPE SLICES" << endl;
  // Save a copy of the original stdin file descriptor
  int original_stdin_fd = dup(0);
  int original_stdout_fd = dup(1);

  // vector<vector<string>> test_slices;
  // test_slices.push_back(vector<string>{"ls", "-la"});
  // test_slices.push_back(vector<string>{"grep", "-i", "parse"});
  // cout << ">>> test_slices >>>" << endl;
  // for (const auto& slice: test_slices) {
  //   for (const auto& token: slice) {
  //     cout << "<" << token << "> ";
  //   }
  //   cout << endl;
  // }
  // cout << "<<<" << endl;
  for (size_t i = 0; i < pipe_slices.size(); ++i) {
    vector<string> pipe_slice = pipe_slices.at(i);
    // Print current command slice
    cout << "current slice: ";
    for (const auto& token : pipe_slice) {
      cout << "<" << token << "> ";
    }
    cout << endl;

    // Create the pipe
    int pipe_fds[2];
    pipe(pipe_fds);

    // int error_num = 0;
    // Create the child process
    pid_t child_pid = fork();
    if (child_pid == 0) {  // If child
      // string command = pipe_slice.at(0);
      // Do IO redirection here
      if (parser.has_io_redirection(pipe_slice)) {
        // pipe_slice.erase(pipe_slice.begin());
        // if (!parser.is_io_redirection_valid()) {
        //   return -1;
        // }
        // pipe_slice = parser.get_io_redirection_command(pipe_slice);
        
        // cout << ">>> [io redirection] pipe_slice >>>" << endl;
        // for (const auto& token : pipe_slice) {
        //   cout << "<" << token << "> ";
        // }
        // cout << endl << "<<<" << endl;

        vector<pair<string, string>> redirect_pairings =
            parser.get_io_redirection_pairings(pipe_slice);
        execute_io_redirection(redirect_pairings);
        pipe_slice = parser.get_io_redirection_command(pipe_slice);
      } 
      // else {
      //   cout << "No IO redirect tokens found" << endl;
      // }

      // error_num = handle_io_redirect(pipe_slice);
      // if (error_num != 0) {
      //   return error_num;
      // }

      // cout << ">>> modified pipe_slice >>>" << endl;
      // for (const auto& token : pipe_slice) {
      //   cout << "<" << token << "> ";
      // }
      // cout << endl << "<<<" << endl;

      // Make child processes write to pipe instead of stdout
      // except for last one. The last one should write to stdout.
      if (i < pipe_slices.size() - 1) {
        dup2(pipe_fds[1], 1);
        close(pipe_fds[1]);
      }
      int error_num = execute_command(pipe_slice);
      if (error_num != 0) {
        cout << "command not found: " << pipe_slice.at(0) << endl;
        // cout << strerror(errno) << endl;
        return error_num;
      }
    } else {  // If parent
      if (!should_run_in_background && i == pipe_slices.size() - 1) {
        waitpid(child_pid, 0, 0);
      } else {
        background_pids.push_back(child_pid);
      }
      // Wait for child process to finish
      // if (i == pipe_slices.size() - 1) {
      //   waitpid(child_pid, 0, 0);
      // }
      // background_pids.push_back(child_pid);

      // Make parent process read from pipe instead of stdin
      dup2(pipe_fds[0], 0);
      close(pipe_fds[1]);
    }
  }
  // execute_command(pipe_slices.at(0));

  // Replace current stdin file descriptor with original value
  dup2(original_stdin_fd, 0);
  dup2(original_stdout_fd, 1);
}

int Shell::execute_io_redirection(
    const vector<pair<string, string>>& redirect_pairings) const {
  for (size_t i = 0; i < redirect_pairings.size(); ++i) {
    string io_redirect = redirect_pairings.at(i).first;
    string filename = redirect_pairings.at(i).second;
    cout << "io_redirect: " << io_redirect << " filename: " << filename << endl;

    if (io_redirect == "<") {  // If input redirect
      int filename_fd = open(filename.c_str(), O_RDONLY,
                             S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
      // only do the actual redirection if it's the last IO redirect
      // if (i == redirect_pairings.size() - 1) {
        dup2(filename_fd, 0);
        close(filename_fd);
        // return 0;
      // }
    } else if (io_redirect == ">") {  // If output redirect
      int filename_fd = open(filename.c_str(), O_CREAT | O_WRONLY | O_TRUNC,
                             S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
      // only do the actual redirection if it's the last IO redirect
      // if (i == redirect_pairings.size() - 1) {
        dup2(filename_fd, 1);
        close(filename_fd);
        // return 0;
      // }
    }
  }

  return 0;
}

// int Shell::execute_io_redirection(vector<string>& command_slice) {
//   cout << ">>>>> [IO REDIRECT] command to redirect >>>>>" << endl;
//   for (const auto& token : command_slice) {
//     cout << "<" << token << "> ";
//   }
//   cout << endl << "<<<<<" << endl << "---------------------" << endl;

//   vector<string> current_slice;
//   // vector<pair<string, string>> redirects_to_handle;
//   for (size_t i = 0; i < command_slice.size(); ++i) {
//     string current_token = command_slice.at(i);
//     if (current_token == "<" or current_token == ">") {
//       // If we hit a redirection token but it's the last token,
//       // that means the filename is missing
//       if (i == command_slice.size() - 1) {
//         cout << current_token << ": missing filename" << endl;
//         return -1;
//       }
//       // string filename = command_slice.at(i + 1);
//       string filename = command_slice.at(command_slice.size() - 1);
//       if (current_token == "<") {  // If input redirect
//         int filename_fd = open(filename.c_str(), O_RDONLY,
//                                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
//         // only do the actual redirection if it's the last IO redirect
//         // the last IO redirect will be at the second-last index, since
//         // the filename will be the last index
//         if (i == command_slice.size() - 2) {
//           dup2(filename_fd, 0);
//           close(filename_fd);

//           command_slice = current_slice;
//           return 0;
//         }
//       } else if (current_token == ">") {  // If output redirect
//         int filename_fd = open(filename.c_str(), O_CREAT | O_WRONLY |
//         O_TRUNC,
//                                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
//         // only do the actual redirection if it's the last IO redirect
//         // the last IO redirect will be at the second-last index, since
//         // the filename will be the last index
//         cout << "debug info: [" << current_token << " | " << i << "]" <<
//         endl; if (i == command_slice.size() - 2) {
//           dup2(filename_fd, 1);
//           close(filename_fd);

//           command_slice = current_slice;
//           return 0;
//         }
//       }
//       // redirects_to_handle.emplace_back(filename, current_token);
//       // current_slice.clear();
//       // command_slice = current_slice;
//       // return 0;
//     } else {
//       current_slice.push_back(current_token);
//     }

//     // cout << ">>> current_slice >>>" << endl;
//     // for (const auto& token : current_slice) {
//     //   cout << "<" << token << "> ";
//     // }
//     // cout << endl << "<<<" << endl;
//   }

//   // for (size_t i = 0; i < redirects_to_handle.size(); ++i) {

//   // }

//   return 0;
// }

int Shell::execute_command(const vector<string>& command_slice) {
  if (command_slice.size() == 0) {
    return 0;
  }

  // pid_t pid = fork();
  // if (child_pid == 0) {  // child process

  // cout << "<" << args[0] << ">" << endl;

  // cout << "<" << args[1] << ">" << endl;
  // char* args[] = {(char*)input_command.c_str(), NULL};
  string command = command_slice.at(0);
  // If it's a built-in command, execute it in the shell process itself
  if (command == "cd") {
    return execute_cd_builtin(command_slice);
  } else if (command == "exit") {
    return execute_exit_builtin(command_slice);
  }
  // If it's NOT a built-in command, execute it in a child process
  // pid_t child_pid = fork();
  // if (child_pid == 0) {  // If child
  char** args = new char*[command_slice.size() + 1];
  for (int i = 0; i < command_slice.size(); ++i) {
    args[i] = (char*)command_slice[i].c_str();
  }
  // args[command_slice.size()] = NULL;
  args[command_slice.size()] = nullptr;

  return execvp(args[0], args);
  // } else {  // If parent
  //   background_pids.push_back(child_pid);
  //   waitpid(child_pid, 0, 0);
  // }
  // return 0;
  // execvp(args[0], args);
  // } else {  // parent process
  // background_pids.push_back(child_pid);
  // waitpid(child_pid, 0, 0);  // wait for the child process
  // we will discuss why waitpid() is preferred over wait()
  // }
}

int Shell::execute_cd_builtin(const vector<string>& command_slice) const {
  if (command_slice.size() > 2) {
    cout << "cd: too many arguments" << endl;
    return 0;
  }

  // TODO: previous directory stuff
  // char current_directory_buffer[100];
  // getcwd(current_directory_buffer, sizeof(current_directory_buffer));
  // previous_directory = string(current_directory_buffer);
  // cout << previous_directory << endl;

  int error_num = 0;
  string command = command_slice.at(0);
  if (command != "cd") {
    throw runtime_error("The first token must be <cd>");
  }
  if (command_slice.size() == 1) {
    // cout << "going to home directory" << endl;
    const char* home_directory;
    // if ((home_directory = getenv("HOME")) == NULL) {
    if ((home_directory = getenv("HOME")) == nullptr) {
      home_directory = getpwuid(getuid())->pw_dir;
    }
    error_num = chdir(home_directory);
  } else {
    string destination = command_slice.at(1);
    // if (destination == ".") {
    //   // cout << "staying in current directory" << endl;
    //   error_num = chdir(".");
    // } else if (destination == "..") {
    //   // cout << "going up 1 directory" << endl;
    //   error_num = chdir("..");
    // } else {
    //   // cout << "going to directory \"" << destination << "\"" << endl;
    //   error_num = chdir(destination.c_str());
    // }
    error_num = chdir(destination.c_str());
  }

  if (error_num != 0) {
    cout << "cd: " << strerror(errno) << endl;
    return error_num;
  }
  return 0;
  // return chdir(destination)
}

int Shell::execute_exit_builtin(const vector<string>& command_slice) const {
  // clean up child background processes
  cout << "Shell exited" << endl;
  // int status;
  // wait(&status);
  // exit(0) should always run in the shell (parent) process
  exit(0);
}

// void Shell::execute_builtin_command(BuiltinCommand command) {}