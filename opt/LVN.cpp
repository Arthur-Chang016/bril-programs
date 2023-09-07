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
    // lambda utilities
    auto isControl = [](json inst)->bool {
        return inst.is_object() && inst.count("op") && (inst["op"] == "br" || inst["op"] == "jmp" || inst["op"] == "ret");
    };
    
    auto isLabel = [](json inst)->bool {
        return inst.is_object() && inst.count("label");
    };
    
    // BB partition
    vector<BasicBlock> ret;
    BasicBlock tmpBB;
    for(int i = 0, n = instrs.size(); i < n - 1; ++i) {
        
        json cur = instrs[i], next = instrs[i + 1];
        bool endOfBlock = isControl(cur) || isLabel(next);
        
        if(endOfBlock) {
            tmpBB.push_back(cur);
            ret.push_back(tmpBB);
            tmpBB = BasicBlock();  // clear
        } else {
            tmpBB.push_back(cur);
        }
    }
    // the last instruction
    tmpBB.push_back(instrs.back());
    ret.push_back(tmpBB);
    
    return ret;
}

json convertToInstrs(vector<BasicBlock> &BBs) {
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
            // bb.print();
            
            bb = LVN(bb);
        }
        func["instrs"] = convertToInstrs(BBs);
    }
    
    // cout << program << endl;
    
    
    return 0;
}
