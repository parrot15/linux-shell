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
  //   bool has_input_redirect(const std::vector<std::string>& command_tokens)
  //   const; bool has_output_redirect(
  //       const std::vector<std::string>& command_tokens) const;
  bool has_io_redirection(const std::vector<std::string>& command_tokens) const;
  //   bool is_io_redirection_valid(const std::vector<std::string>&
  //   command_tokens) const;
  // void split_on_special_tokens(std::vector<std::string>& tokens) const;
  std::vector<std::string> get_io_redirection_command(
      const std::vector<std::string>& redirect_slice) const;
  std::vector<std::pair<std::string, std::string>> get_io_redirection_pairings(
      const std::vector<std::string>& redirect_slice) const;
  std::vector<std::vector<std::string>> split_by_pipe(
      const std::vector<std::string>& command_tokens) const;
  //   std::vector<std::vector<std::string>> split_by_redirect(
  //       const std::vector<std::string>& command_tokens) const;

 private:
  bool is_input_valid = true;

  // Encapsulation characters
  const std::string DOUBLE_QUOTE = "\"";
  const std::string SINGLE_QUOTE = "\'";
  const std::string OPEN_PAREN = "(";
  const std::string CLOSE_PAREN = ")";

  // Special characters
  const std::string INPUT_REDIRECT = "<";
  const std::string OUTPUT_REDIRECT = ">";
  const std::string PIPE = "|";
  const std::string BACKGROUND_PROCESS = "&";
  const std::string EXPANSION = "$";

  // Space character
  const std::string SPACE = " ";
};

#endif  // PA2_IMPLEMENTING_A_LINUX_SHELL_PARROT15_PARSER_H
