#include "parser.h"

std::vector<std::string> Parser::tokenize(const std::string& command) {
  is_input_valid = true;
  std::vector<std::string> tokens;

  bool is_double_quote_closed = false;
  bool is_single_quote_closed = false;
  int token_length = 0;
  for (int i = 0; i < command.length(); ++i) {
    int start_idx = i;
    if (std::string(1, command[i]) == DOUBLE_QUOTE) {
      is_double_quote_closed = true;
    } else if (std::string(1, command[i]) == SINGLE_QUOTE) {
      is_single_quote_closed = true;
    }

    if (is_double_quote_closed) {
      ++i;
      ++start_idx;
      while (i < command.length() &&
             std::string(1, command[i]) != DOUBLE_QUOTE) {
        ++i;
      }
      if (i < command.length()) {
        is_double_quote_closed = false;
      }
      token_length = i - start_idx;
      ++i;
    } else if (is_single_quote_closed) {
      ++i;
      ++start_idx;
      while (i < command.length() &&
             std::string(1, command[i]) != SINGLE_QUOTE) {
        ++i;
      }
      if (i < command.length()) {
        is_single_quote_closed = false;
      }
      token_length = i - start_idx;
      ++i;
    } else {
      while (i < command.length() && std::string(1, command[i]) != SPACE) {
        ++i;
      }
      token_length = i - start_idx;
    }
    if (token_length > 0) {  // If the token is not empty
      tokens.push_back(command.substr(start_idx, token_length));
    }
  }
  if (is_double_quote_closed) {
    std::cout << "Missing closing double quote" << std::endl;
    is_input_valid = false;
    return {};
  }
  if (is_single_quote_closed) {
    std::cout << "Missing closing single quote" << std::endl;
    is_input_valid = false;
    return {};
  }

  return tokens;
}

bool Parser::is_valid_input() const { return is_input_valid; }

bool Parser::is_marked_for_background(
    const std::vector<std::string>& command_tokens) const {
  return command_tokens.back() == BACKGROUND_PROCESS;
}

bool Parser::has_io_redirection(
    const std::vector<std::string>& command_tokens) const {
  bool has_input_redirect = find(command_tokens.begin(), command_tokens.end(),
                                 INPUT_REDIRECT) != command_tokens.end();
  bool has_output_redirect = find(command_tokens.begin(), command_tokens.end(),
                                  OUTPUT_REDIRECT) != command_tokens.end();
  return has_input_redirect || has_output_redirect;
}

std::vector<std::string> Parser::get_io_redirection_command(
    const std::vector<std::string>& redirect_slice) const {
  std::vector<std::string> command_tokens;
  for (const auto& token : redirect_slice) {
    if (token == INPUT_REDIRECT || token == OUTPUT_REDIRECT) {
      break;
    }
    command_tokens.push_back(token);
  }
  return command_tokens;
}

std::vector<std::pair<std::string, std::string>>
Parser::get_io_redirection_pairings(
    const std::vector<std::string>& redirect_slice) const {
  std::vector<std::string> slice_without_command = redirect_slice;
  for (size_t i = 0; i < slice_without_command.size(); ++i) {
    std::string current_token = slice_without_command.at(i);
    if (current_token == INPUT_REDIRECT || current_token == OUTPUT_REDIRECT) {
      break;
    }
    slice_without_command.erase(slice_without_command.begin() + i);
    --i;
  }

  std::vector<std::pair<std::string, std::string>> redirect_pairings;
  for (size_t i = 0; i < slice_without_command.size() - 1; i += 2) {
    std::string first_token = slice_without_command.at(i);
    std::string second_token = slice_without_command.at(i + 1);

    std::pair<std::string, std::string> current_pairing;
    if (first_token == INPUT_REDIRECT || first_token == OUTPUT_REDIRECT) {
      redirect_pairings.emplace_back(first_token, second_token);
    } else {
      throw std::runtime_error("Received an invalid redirection token");
    }
  }

  return redirect_pairings;
}

std::vector<std::vector<std::string>> Parser::split_by_pipe(
    const std::vector<std::string>& command_tokens) const {
  // If there are no pipe tokens, then do nothing
  if (find(command_tokens.begin(), command_tokens.end(), PIPE) ==
      command_tokens.end()) {
    return std::vector<std::vector<std::string>>{command_tokens};
  }

  std::vector<std::vector<std::string>> pipe_slices;
  std::vector<std::string> current_slice;
  for (size_t i = 0; i < command_tokens.size(); ++i) {
    std::string current_token = command_tokens.at(i);
    current_slice.push_back(current_token);

    // Append to the list of lists if we've hit a pipe or we reached the end
    if (current_token == PIPE || i == command_tokens.size() - 1) {
      if (current_token == PIPE) {
        current_slice.pop_back();
      }
      pipe_slices.push_back(current_slice);
      current_slice.clear();
    }
  }

  return pipe_slices;
}