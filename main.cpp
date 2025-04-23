#include "regex.hpp"

int main(int argc, char** argv){

    if (argc < 2){
        std::cout << "Usage: <./appname> <regexpression>\n";
        return 1;
    }
    std::string regex = argv[1];
    //std::string match_in = argv[2];

    std::string postfix_expr = toPostfix(regex);

    std::cout << postfix_expr << "\n";

    return 0;
}