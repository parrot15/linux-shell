#ifndef PA2_IMPLEMENTING_A_LINUX_SHELL_PARROT15_PARSER_H
#define PA2_IMPLEMENTING_A_LINUX_SHELL_PARROT15_PARSER_H

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class Parser {
 public:
  Parser() = default;

  std::vector<std::string> tokenize(const std::string& command);
  bool is_valid_input() const;

  bool is_marked_for_background(
      const std::vector<std::string>& command_tokens) const;

  bool has_io_redirection(const std::vector<std::string>& command_tokens) const;
  std::vector<std::string> get_io_redirection_command(
      const std::vector<std::string>& redirect_slice) const;
  std::vector<std::pair<std::string, std::string>> get_io_redirection_pairings(
      const std::vector<std::string>& redirect_slice) const;

  std::vector<std::vector<std::string>> split_by_pipe(
      const std::vector<std::string>& command_tokens) const;

  // Encapsulation characters
  const std::string DOUBLE_QUOTE = "\"";
  const std::string SINGLE_QUOTE = "\'";

  // Special characters
  const std::string INPUT_REDIRECT = "<";
  const std::string OUTPUT_REDIRECT = ">";
  const std::string PIPE = "|";
  const std::string BACKGROUND_PROCESS = "&";

  // Space character
  const std::string SPACE = " ";

 private:
  bool is_input_valid = true;
};

#endif  // PA2_IMPLEMENTING_A_LINUX_SHELL_PARROT15_PARSER_H
