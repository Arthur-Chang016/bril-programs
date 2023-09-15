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

/**
 * For liveness analysis, Each basic block contains only 1 instructions
*/
pair<vector<BasicBlock>, unordered_map<int, vector<int>>> BuildCFG(json instrs) {
    // lambda utilities
    auto isControl = [](json inst)->bool {
        return inst.is_object() && inst.count("op") && (inst["op"] == "br" || inst["op"] == "jmp" || inst["op"] == "ret");
    };
    
    auto isLabel = [](json inst)->bool {
        return inst.is_object() && inst.count("label");
    };
    
    // BB partition
    vector<BasicBlock> ret;
    unordered_map<string, int> label2ind;
    BasicBlock tmpBB;
    int i = 0, n;
    for(i = 0, n = instrs.size(); i < n - 1; ++i) {
        json cur = instrs[i], next = instrs[i + 1];
        // cout << cur << endl;
        if(isLabel(cur)) {
            tmpBB.push_back(cur);  // label
            label2ind[cur["label"]] = ret.size();  // the new bb index
            tmpBB.push_back(next);
            ++i;
        } else {  // normal inst
            tmpBB.push_back(cur);
        }
        ret.push_back(tmpBB);
        tmpBB = BasicBlock();  // clear
    }
    if(i < instrs.size()) {  // process the last one
        tmpBB.push_back(instrs.back());
        ret.push_back(tmpBB);
    }
    
    unordered_map<int, vector<int>> graph;
    // init graph
    for(int i = 0; i < ret.size(); ++i)
        graph[i] = {};
    
    // build CFG
    for(int i = 0; i < ret.size(); ++i) {
        BasicBlock &bb = ret[i];
        // the first might be label
        json inst = bb.instrs.size() > 1 ? bb.instrs[1] : bb.instrs[0];
        if(isControl(inst)) {
            // (inst["op"] == "br" || inst["op"] == "jmp" || inst["op"] == "ret")
            
            if(inst.count("labels")) {
                for(json str: inst["labels"]) {
                    graph[i].push_back(label2ind[str]);
                }
            } else {  // ret
                // no next block
            }
        } else {
            if(i != ret.size() - 1)
                graph[i].push_back(i + 1);
        }
    }
    
    for(auto [i, v]: graph) {
        cout << i << ": ";
        for(int j: v) cout << j << ' ';
        cout << endl;
    }
    
    
    
    return {ret, graph};
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


int main(int argc, char **argv) {
    json program = ReadStdin();
    
    // cout << program << endl;
    
    for(auto &func: program["functions"]) {
        auto [BBs, graph] = BuildCFG(func["instrs"]);
        
        
        
        for(BasicBlock &bb: BBs) {
            // bb = LVN(bb);
            
            bb.print();
        }
        func["instrs"] = convertToInstrs(BBs);
    }
    // pretty printer
    // cout << program.dump(2) << endl;
    
    
    return 0;
}
