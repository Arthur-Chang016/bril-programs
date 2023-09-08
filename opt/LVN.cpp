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

vector<long> ConvertToVa(const vector<json> &vj, unordered_map<string, int> &var2num) {
    vector<long> ret;
    for(const json &j: vj) {
        if(j.is_string() == false) {
            cout << j << " is not a string";
            exit(1);
        }
        ret.push_back(var2num.count(j) ? var2num[j] : -1);
        // ret.push_back(var2num[(string) j]);
    }
    return ret;
}

bool ContainsN1(const vector<long> &arr) {
    for(long l: arr)
        if(l == -1) return true;
    return false;
}

void ReplaceArgs(json &inst, const vector<LvnEntry> &table, unordered_map<string, int> &var2num) {
    if(inst.count("args")) {
        for(json &arg: inst["args"]) {
            if(var2num.count(arg))
                arg = table[var2num[arg]].canVar;
        }
    }
}

BasicBlock LVN(BasicBlock &bb) {
    // bb.print();
    
    vector<LvnEntry> table;
    map<Value, int> value2num;
    unordered_map<string, int> var2num;
    
    for(json &inst: bb.instrs) {
        if(inst.count("label")) continue;
        if(inst.count("op") == false) continue;
        
        // if(inst["op"] == "const") continue;
        if(inst.count("dest") == false) continue;
        
        if(inst["op"] == "call") {
            // replace args
            ReplaceArgs(inst, table, var2num);
            // cout << "it's call !!!!!!!!!!!!!!!" << endl;
            continue;
        }
        
        // if(inst["op"] == "id") {
            
        // }
        
        vector<long> args = inst.count("args") ? ConvertToVa(inst["args"], var2num) : vector<long>();
        
        if(ContainsN1(args)) continue;  // to be conservative. If -1, don't modify this inst
        // build value
        // Value value((string) inst["op"], args);
        Value value = inst["op"] == "const" ? Value(inst["op"], (long)inst["value"]) : Value(inst["op"], args);
        
        
        
        int num = 0;
        if(value2num.count(value)) {
            num = value2num[value];
            string var = table[num].canVar;
            
            // TODO update the args with the var, if it's not const
            
            // cout << "here ~!!~!!!!!!!!!!!" << endl;
            // cout << inst << endl;
            
            json orig = inst;
            
            inst = json::object();
            inst["args"] = json::array({var});
            inst["dest"] = orig["dest"];
            inst["op"] = "id";
            inst["type"] = orig["type"];
            
            // cout << inst << endl;
            
            
        } else {  // the value is not found
            num = table.size();
            string dest = inst["dest"];
            
            // TODO check if inst being overwritten
            
            if(inst["op"] == "id" && var2num.count(inst["args"][0])) {  // directly point to the target
                string target = inst["args"][0];
                var2num[dest] = var2num[target];
                
                // value.print();
                // cout << endl;
                
                // for(auto [s, l]: var2num) {
                //     cout << s << ' ' << l << endl;
                // }
                // cout << endl;
                
            } else {
                // value.print();
                
                table.push_back(LvnEntry(value, dest));  // at the index "num"
                value2num[value] = num;
            }
            // replace args
            ReplaceArgs(inst, table, var2num);
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
            
            // bb.print();
        }
        func["instrs"] = convertToInstrs(BBs);
    }
    // pretty printer
    cout << program.dump(2) << endl;
    
    
    return 0;
}
