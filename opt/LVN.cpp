// STL
#include <fstream>
#include <iostream>
using namespace std;

// json lib: https://github.com/nlohmann/json
#include "json.hpp"
using json = nlohmann::json;

// self create
#include "BasicBlock.hpp"

json ReadStdin() {
    return json::parse(cin);
}

vector<BasicBlock> BuildBasicBlock() {
    
    return {};
}

int main(int argc, char **argv) {
    json program = ReadStdin();
    
    // cout << program << endl;
    
    for(auto func: program["functions"]) {
        cout << func << endl;
    }
    
    
    
    return 0;
}

