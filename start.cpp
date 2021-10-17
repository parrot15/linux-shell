// #include <fcntl.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/stat.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// #include <unistd.h>

// #include <termios.h>
// #include <unistd.h>

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
      cout << "Skipping..." << endl;
      continue;
    }
  }
}

// int main() {
//   Shell shell;

//   int ch;
//   for (;;) {
//     display_prompt();

//     // 2nd way
//     struct termios old_settings;
//     struct termios new_settings;

//     // get current terminal attirbutes descriptor for stdin
//     tcgetattr(STDIN_FILENO, &old_settings);
//     new_settings = old_settings;

//     // disable canonical mode
//     new_settings.c_lflag &= ~ICANON;
//     // wait until at least one keystroke available
//     new_settings.c_cc[VMIN] = 1;
//     // no timeout
//     new_settings.c_cc[VTIME] = 0;
//     // set immediately
//     tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);

//     char ch;
//     for (;;) {
//       cin >> ch;
//       // if (ch == 10) {  // If enter was pressed
//       //   cout << "enter pressed" << endl;
//       //   break;
//       // }
//       if (ch == 65) {  // If up arrow was pressed
//         cout << "up arrow pressed" << endl;
//         break;
//       } else if (ch == 66) {  // If down arrow was pressed
//         cout << "down arrow pressed" << endl;
//         break;
//       }
//     }

//     // for (;;) {
//     //   int ch = getchar();
//     //   if (ch < 0) {
//     //     if (ferror(stdin)) { /* there was an error... */
//     //       cout << "An error occurred" << endl;
//     //     }
//     //     clearerr(stdin);
//     //     /* do other stuff */
//     //   } else if (ch == 65) {
//     //     /* some key OTHER than ESC was hit, do something about it? */
//     //     cout << "up arrow" << endl;
//     //     break;
//     //   } else if (ch = 66) {
//     //     cout << "down arrow" << endl;
//     //     break;
//     //   } else {
//     //     cout << "not up/down arrow. Ignoring..." << endl;
//     //     break;
//     //   }
//     // }

//     // tcgetattr(STDIN_FILENO, &info);
//     // info.c_lflag |= ICANON;
//     // tcsetattr(STDIN_FILENO, TCSANOW, &info);
//     tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);

//     string input_command;
//     getline(cin, input_command);  // Get a line from standard input

//     bool is_valid_input = shell.interpret_command(input_command);
//     if (!is_valid_input) {
//       cout << "Skipping..." << endl;
//       continue;
//     }
//   }

//   // // 3rd way
//   // struct termios old_settings;
//   // struct termios new_settings;

//   // tcgetattr(STDIN_FILENO, &old_settings);
//   // new_settings = old_settings;
//   // new_settings.c_lflag &= (~ICANON & ~ECHO);
//   // tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);

//   // for (;;) {
//   //   fd_set set;
//   //   struct timeval tv;

//   //   tv.tv_sec = 10;
//   //   tv.tv_usec = 0;

//   //   FD_ZERO(&set);
//   //   FD_SET(STDIN_FILENO, &set);

//   //   int res = select(STDIN_FILENO + 1, &set, nullptr, nullptr, &tv);

//   //   if (res > 0) {
//   //     char c;
//   //     printf("Input available\n");
//   //     read(STDIN_FILENO, &c, 1);
//   //   } else if (res < 0) {
//   //     perror("select error");
//   //     break;
//   //   } else {
//   //     printf("Select timeout\n");
//   //   }
//   // }

//   // tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);

//   // Shell shell;

//   // for (;;) {
//   //   display_prompt();

//   //   string input_command;
//   //   getline(cin, input_command);  // Get a line from standard input
//   //   bool is_valid_input = shell.interpret_command(input_command);
//   //   if (!is_valid_input) {
//   //     cout << "Skipping..." << endl;
//   //     continue;
//   //   }
//   // }
// }
