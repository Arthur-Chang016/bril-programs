// STL
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
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

unordered_set<string> Allvars(BasicBlock &bb) {
    unordered_set<string> ret;
    for(json &inst: bb.instrs) {
        if(inst.count("dest")) ret.insert(inst["dest"]);
        if(inst.count("args")) {
            for(json &j: inst["args"]) ret.insert(j);
        }
    }
    return ret;
}

BasicBlock SingleTDCE(BasicBlock &bb) {
    
    unordered_set<string> live = Allvars(bb);
    // for(json &inst: bb.instrs) {
        
    for(int i = bb.instrs.size() - 1; i >= 0; --i) {
        json &inst = bb.instrs[i];
        
        vector<string> defs, uses;
        if(inst.count("dest")) defs.push_back(inst["dest"]);
        if(inst.count("args")) {
            for(json &j: inst["args"]) uses.push_back(j);
        }
        
        // delete inst
        if(defs.empty() == false) {
            string def = defs.back();
            if(!live.count(def)) inst = json::object();  // remove the inst
        }
        
        // live -= defs
        for(string &s: defs) {
            live.erase(s);
        }
        
        // live += uses
        for(string &s: uses) {
            live.insert(s);
        }
    }
    
    // remove empty instructions
    bb.instrs.erase(remove_if(begin(bb.instrs), end(bb.instrs), [](const json& j) {
        return j == json::object();  // equals to empty object
    }), end(bb.instrs));
    
    return bb;
}

BasicBlock TDCE(BasicBlock &bb) {
    int origSize;
    do {
        origSize = bb.instrs.size();
        bb = SingleTDCE(bb);
    } while(origSize != bb.instrs.size());
    return bb;
}

int main(int argc, char **argv) {
    json program = ReadStdin();
    
    // cout << program << endl;
    
    for(auto &func: program["functions"]) {
        vector<BasicBlock> BBs = BuildBasicBlock(func["instrs"]);
        
        
        
        for(BasicBlock &bb: BBs) {
            bb = TDCE(bb);
        }
        func["instrs"] = convertToInstrs(BBs);
    }
    // pretty printer
    cout << program.dump(2) << endl;
    
    
    return 0;
}
