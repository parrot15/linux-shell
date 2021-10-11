#ifndef PA2_IMPLEMENTING_A_LINUX_SHELL_PARROT15_PARSER_H
#define PA2_IMPLEMENTING_A_LINUX_SHELL_PARROT15_PARSER_H

#include <vector>
#include <string>
#include <iostream>

class Parser {
public:
    Parser() = default;

    std::vector<std::string> tokenize(const std::string& str_to_tokenize);
};


#endif //PA2_IMPLEMENTING_A_LINUX_SHELL_PARROT15_PARSER_H
