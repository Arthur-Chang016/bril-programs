// STL
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

// json lib: https://github.com/nlohmann/json
#include "json.hpp"
using json = nlohmann::json;

// self create
#include "BasicBlock.hpp"

json ReadStdin() {
    return json::parse(cin);
}

vector<BasicBlock> BuildBasicBlock(json instrs) {
    // TODO
    
    BasicBlock bb;
    for (json &inst: instrs) {
        bb.push_back(inst);
    }
    
    return {bb};
}

json convertToInst(vector<BasicBlock> &BBs) {
    json retInstrs = json::array();
    for(BasicBlock &bb: BBs) {
        for(json &inst: bb.instrs) {
            retInstrs.push_back(inst);
        }
    }
    return retInstrs;
}

BasicBlock LVN(BasicBlock &bb) {
    // TODO
    return bb;
}

int main(int argc, char **argv) {
    json program = ReadStdin();
    
    // cout << program << endl;
    
    for(auto &func: program["functions"]) {
        vector<BasicBlock> BBs = BuildBasicBlock(func["instrs"]);
        
        for(BasicBlock &bb: BBs) {
            bb = LVN(bb);
        }
        func["instrs"] = convertToInst(BBs);
    }
    
    cout << program << endl;
    
    
    return 0;
}

