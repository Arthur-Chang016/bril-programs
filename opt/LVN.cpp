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
    // bb.print();
    
    vector<LvnEntry> table;
    map<Value, int> value2num;
    unordered_map<string, long> var2num;
    
    for(json &inst: bb.instrs) {
        if(inst.count("label")) continue;
        if(inst.count("op") == false) continue;
        
        // if(inst["op"] == "const") continue;
        if(inst.count("dest") == false) continue;
        
        if(inst["op"] == "call") {
            // TODO, change some args to the new var
            cout << "it's call !!!!!!!!!!!!!!!" << endl;
            continue;
        }
        
        vector<string> args = inst.count("args") ? ConvertToVa(inst["args"]) : vector<string>();
        // build value
        // Value value((string) inst["op"], args);
        Value value = inst["op"] == "const" ? Value(inst["op"], (long)inst["value"]) : Value(inst["op"], args);
        
        int num = 0;
        if(value2num.count(value)) {
            num = value2num[value];
            string var = table[num].canVar;
            
            // TODO update the args with the var, if it's not const
            
            
        } else {
            // int newNum = table.size();
            num = table.size();
            string dest = inst["dest"];
            
            // TODO check if inst being overwritten
            
            if(inst["op"] == "id") {  // directly point to the target
                string target = inst["args"][0];
                var2num[dest] = var2num[target];
                
                // value.print();
                // cout << endl;
                
                // for(auto [s, l]: var2num) {
                //     cout << s << ' ' << l << endl;
                // }
                // cout << endl;
                
            } else {
                table.push_back(LvnEntry(value, dest));  // at the index "num"
            }
            
            
            // replace args
            if(inst.count("args")) {
                for(json &arg: inst["args"]) {
                    if(var2num.count(arg))
                        arg = table[var2num[arg]].canVar;
                }
            }
        }
        var2num[inst["dest"]] = num;
        
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
            bb = LVN(bb);
            
            bb.print();
        }
        func["instrs"] = convertToInstrs(BBs);
    }
    
    // cout << program << endl;
    
    
    return 0;
}
