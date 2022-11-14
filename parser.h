#ifndef LINUX_SHELL_PARSER_H
#define LINUX_SHELL_PARSER_H

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class Parser {
 public:
  Parser() = default;

  /**
   * Tokenizes (splits) a command into its constituent words and special
   * characters. For example, the command "cat names.txt | sort" would be
   * tokenized as: "<cat>, <names.txt>, <|>, <sort>".
   * @param command The command to tokenize.
   * @return The list of tokens.
   */
  std::vector<std::string> tokenize(const std::string& command);

  /**
   * Determines if the user input to the shell is valid.
   * @return True if the input is valid, false otherwise.
   */
  bool is_valid_input() const;

  /**
   * Checks if a list of command tokens should run in the background by
   * checking if the last token is a "&" character.
   * @param command_tokens The list of tokens to check.
   * @return True if the tokenized command should run as a background
   * process, false otherwise.
   */
  bool is_marked_for_background(
      const std::vector<std::string>& command_tokens) const;

  /**
   * Checks if a list of command tokens involves I/O redirection (if any
   * of the tokens are either a "<" or ">" character).
   * @param command_tokens The list of tokens to check.
   * @return True if the tokenized command involves I/O redirection,
   * false if otherwise.
   */
  bool has_io_redirection(const std::vector<std::string>& command_tokens) const;

  /**
   * Extracts the command to execute from a given set of tokens containing
   * an I/O redirection character. For example, if the given set of tokens
   * is "<echo>, <"hello">, <>>, <test.txt>", then it will extract "<echo>,
   * <"hello">".
   * @param redirect_slice The set of tokens containing an I/O redirection
   * character.
   * @return The extracted command to execute.
   */
  std::vector<std::string> get_io_redirection_command(
      const std::vector<std::string>& redirect_slice) const;

  /**
   * Pairs up each I/O redirection with its associated file to redirect
   * output towards.
   * @param redirect_slice The set of tokens containing an I/O redirection
   * character.
   * @return A list of all pairs of I/O redirections with ther associated
   * files.
   * @throw std::runtime_error if an invalid redirection token (a token
   * that isn't "<" or ">") was received.
   */
  std::vector<std::pair<std::string, std::string>> get_io_redirection_pairings(
      const std::vector<std::string>& redirect_slice) const;

  /**
   * Splits a list of tokens along each pipe character encountered. For
   * example, if the command tokens are:
   *   [<ps>, <-ef>, <|>, <head>, <|>, <grep>, <-i>, <rcu>]
   * Then the output will be:
   *   [[<ps>, <-ef>], [<head>], [<grep>, <-i>, <rcu>]]
   * @param command_tokens The list of tokens to split up by pipe.
   * @return A list of each slice of tokens splitted.
   */
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

#endif
