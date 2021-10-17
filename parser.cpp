#include "parser.h"

using namespace std;

// Parser::Parser() : is_input_invalid(false) {
//
//}

vector<string> Parser::tokenize(const string& command) {
  is_input_valid = true;
  // cout << "tokenizing command: " << command << endl;
  vector<string> tokens;

  bool is_double_quote_closed = false;
  bool is_single_quote_closed = false;
  int token_length = 0;
  for (int i = 0; i < command.length(); ++i) {
    int start_idx = i;
    // string current_char = string(1, command[i]);
    if (string(1, command[i]) == DOUBLE_QUOTE) {
      is_double_quote_closed = true;
    } else if (string(1, command[i]) == SINGLE_QUOTE) {
      is_single_quote_closed = true;
    }

    if (is_double_quote_closed) {
      ++i;
      ++start_idx;
      while (i < command.length() && string(1, command[i]) != DOUBLE_QUOTE) {
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
      while (i < command.length() && string(1, command[i]) != SINGLE_QUOTE) {
        ++i;
      }
      if (i < command.length()) {
        is_single_quote_closed = false;
      }
      token_length = i - start_idx;
      ++i;
    } else {
      while (i < command.length() && string(1, command[i]) != SPACE) {
        ++i;
      }
      token_length = i - start_idx;
    }
    //        tokens.push_back(command.substr(start_idx, token_length));
    if (token_length > 0) {  // If the token is a string
      // string token = command.substr(start_idx, token_length);
      // stringstream token_buffer;
      // bool is_string_token = token.find(SPACE) != string::npos;
      // if (is_string_token) {
      //   token_buffer << DOUBLE_QUOTE << command.substr(start_idx, token_length)
      //                << DOUBLE_QUOTE;
      // } else {
      //   token_buffer << command.substr(start_idx, token_length);
      // }
      // tokens.push_back(token_buffer.str());
      // original:
      tokens.push_back(command.substr(start_idx, token_length));
    }
  }
  //    if (is_double_quote_closed || is_single_quote_closed) {
  //        cout << "One of the quotes is open\n";
  //    }
  if (is_double_quote_closed) {
    cout << "Missing closing double quote" << endl;
    is_input_valid = false;
    return vector<string>();
  }
  if (is_single_quote_closed) {
    cout << "Missing closing single quote" << endl;
    is_input_valid = false;
    return vector<string>();
  }

  // cout << "tokens size: " << tokens.size() << endl;
  cout << "tokens: ";
  for (const auto& i : tokens) {
    cout << "<" << i << "> ";
  }
  cout << endl;

  return tokens;
}

bool Parser::is_valid_input() const { return is_input_valid; }

bool Parser::is_marked_for_background(
    const vector<string>& command_tokens) const {
  return command_tokens.back() == BACKGROUND_PROCESS;
}

bool Parser::has_io_redirection(const vector<string>& command_tokens) const {
  // cout << ">>> command_tokens >>>" << endl;
  // for (const auto& token : command_tokens) {
  //   cout << "<" << token << "> ";
  // }
  // cout << endl << "<<<" << endl;
  bool has_input_redirect = find(command_tokens.begin(), command_tokens.end(),
                                 INPUT_REDIRECT) != command_tokens.end();
  bool has_output_redirect = find(command_tokens.begin(), command_tokens.end(),
                                  OUTPUT_REDIRECT) != command_tokens.end();
  // cout << "contains input redirect: " << boolalpha << has_input_redirect
  //      << endl;
  // cout << "contains output redirect: " << boolalpha << has_output_redirect
  //      << endl;
  return has_input_redirect || has_output_redirect;
}

// bool Parser::is_io_redirection_valid(const vector<string>& command_tokens)
// const {

// }

// void Parser::split_on_special_tokens(vector<string>& tokens) const {}

vector<vector<string>> Parser::split_by_pipe(
    const vector<string>& command_tokens) const {
  cout << "splitting by pipe" << endl;

  // If there are no pipe tokens, then do nothing
  if (find(command_tokens.begin(), command_tokens.end(), PIPE) ==
      command_tokens.end()) {
    cout << "No pipe tokens found" << endl;
    return vector<vector<string>>{command_tokens};
  }

  vector<vector<string>> pipe_slices;
  vector<string> current_slice;
  for (size_t i = 0; i < command_tokens.size(); ++i) {
    string current_token = command_tokens.at(i);
    current_slice.push_back(current_token);

    // Append to the list of lists if we've hit a pipe or we reached the end
    if (current_token == PIPE or i == command_tokens.size() - 1) {
      if (current_token == PIPE) {
        current_slice.pop_back();
      }
      pipe_slices.push_back(current_slice);
      current_slice.clear();
    }

    // // print pipe_slices
    // cout << ">>>>> pipe slices >>>>>" << endl;
    // for (const auto& slice : pipe_slices) {
    //   // cout << "[" << slice.at(0) << "]" << endl;
    //   for (const auto& token : slice) {
    //     cout << "<" << token << "> ";
    //   }
    //   cout << endl;
    // }
    // cout << "<<<<<" << endl;
  }

  return pipe_slices;
  // for (const auto& token : command_tokens) {
  //   if (token == PIPE) {
  //     current_slice.push_back(token);
  //   }
  // }
}

vector<string> Parser::get_io_redirection_command(
    const vector<string>& redirect_slice) const {
  cout << "[get_io_redirection_command] redirect_slice: ";
  for (const auto& token : redirect_slice) {
    cout << "<" << token << "> ";
  }
  cout << endl;
  vector<string> command_tokens;
  for (const auto& token : redirect_slice) {
    if (token == INPUT_REDIRECT or token == OUTPUT_REDIRECT) {
      break;
    }
    command_tokens.push_back(token);
  }
  return command_tokens;
}

vector<pair<string, string>> Parser::get_io_redirection_pairings(
    const vector<string>& redirect_slice) const {
  cout << "getting IO redirection pairings" << endl;
  // cout << ">>>>> redirect_slice >>>>>" << endl;
  // for (const auto& token : redirect_slice) {
  //   cout << "<" << token << "> ";
  // }
  // cout << endl << "<<<<<" << endl;

  vector<string> slice_without_command = redirect_slice;
  for (size_t i = 0; i < slice_without_command.size(); ++i) {
    string current_token = slice_without_command.at(i);
    if (current_token == INPUT_REDIRECT or current_token == OUTPUT_REDIRECT) {
      break;
    }
    slice_without_command.erase(slice_without_command.begin() + i);
    --i;
  }
  cout << ">>>>> slice_without_command >>>>>" << endl;
  for (const auto& token : slice_without_command) {
    cout << "<" << token << "> ";
  }
  cout << endl << "<<<<<" << endl;

  vector<pair<string, string>> redirect_pairings;
  for (size_t i = 0; i < slice_without_command.size() - 1; i += 2) {
    string first_token = slice_without_command.at(i);
    string second_token = slice_without_command.at(i + 1);

    pair<string, string> current_pairing;
    if (first_token == INPUT_REDIRECT || first_token == OUTPUT_REDIRECT) {
      // current_pairing.first = first_token;
      // current_pairing.second = second_token;
      redirect_pairings.emplace_back(first_token, second_token);
    } else {
      throw runtime_error("Received an invalid redirection token");
    }

    // redirect_pairings.push_back(current_pairing);
  }

  return redirect_pairings;
}