// STL
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
using namespace std;

// json lib: https://github.com/nlohmann/json
#include "json.hpp"
using json = nlohmann::json;

// self create
#include "BasicBlock.hpp"
#include "LvnUtils.hpp"

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

vector<string> ConvertToVa(const vector<json> &vj) {
    vector<string> ret;
    for(const json &j: vj) {
        if(j.is_string() == false) {
            cout << j << " is not a string";
            exit(1);
        }
        ret.push_back(j);
    }
    return ret;
    // return {};
}

BasicBlock LVN(BasicBlock &bb) {
    bb.print();
    
    vector<LvnEntry> table;
    map<Value, int> value2num;
    unordered_map<string, long> var2num;
    
    for(json &inst: bb.instrs) {
        if(inst.count("label")) continue;
        if(inst.count("op") == false) continue;
        
        if(inst["op"] == "call") {
            // TODO, change some args to the new var
            cout << "it's call !!!!!!!!!!!!!!!" << endl;
            continue;
        }
        
        vector<string> args = inst.count("args") ? ConvertToVa(inst["args"]) : vector<string>();
        // build value
        Value value((string) inst["op"], args);
        
        if(value2num.count(value)) {
            int num = value2num[value];
            string var = table[num].canVar;
            
            
            
        }
        
        
        
        // if(inst["op"] == "const") {
            
            
            
            
        // } else if(inst.count("args")) {
        //     // cout << "here : " << inst["args"].is_array() << endl;
        //     // cout << "here : " << inst["args"] << endl;
            
        //     vector<string> args = inst.count("args") ? ConvertToVa(inst["args"]) : {};
            
            
        //     // build value
        //     Value value((string) inst["op"], args);
        // }
        
        
        
        
        
    }
    
    
    
    
    
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
