// STL
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
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

/**
 * the return array's size corresponds to the BB
 * each BB has only 1 instructions
*/
pair<vector<vector<string>>, vector<vector<string>>> BuildLiveness(vector<BasicBlock> &BBs) {
    vector<vector<string>> defs(BBs.size()), uses(BBs.size());
    for(int i = 0; i < BBs.size(); ++i) {
        BasicBlock &bb = BBs[i];
        json inst = bb.instrs.size() > 1 ? bb.instrs[1] : bb.instrs[0];
        
        if(inst.count("dest")) {
            defs[i].push_back(inst["dest"]);
        }
        if(inst.count("args")) {
            uses[i].insert(uses[i].end(), inst["args"].begin(), inst["args"].end());
        }
    }
    return {defs, uses};
}

void print(vector<vector<string>> &defs, vector<vector<string>> &uses) {
    cout << "{" << endl;
    for(int i = 0; i < defs.size(); ++i) {  // assume they have the same size
        cout << "{ defs: ";
        for(string s: defs[i]) {
            cout << s << ' ';
        }
        cout << ", uses: ";
        for(string s: uses[i]) {
            cout << s << ' ';
        }
        cout << "}" << endl;
    }
    cout << "}" << endl;
}

void print(vector<set<string>> &in, vector<set<string>> &out) {
    cout << "{" << endl;
    for(int i = 0; i < in.size(); ++i) {  // assume they have the same size
        cout << "{ in: ";
        for(string s: in[i]) {
            cout << s << ' ';
        }
        cout << ", out: ";
        for(string s: out[i]) {
            cout << s << ' ';
        }
        cout << "}" << endl;
    }
    cout << "}" << endl;
}

unordered_map<int, vector<int>> BuildPred(unordered_map<int, vector<int>> &graph) {
    unordered_map<int, vector<int>> pred;
    for(int i = 0; i < graph.size(); ++i)
        pred[i] = {};
    
    for(int i = 0; i < graph.size(); ++i) {
        for(int next: graph[i]) {
            pred[next].push_back(i);  // reverse edges
        }
    }
    return pred;
}

pair<bool, set<string>> UnionSucc(set<string> &self, vector<int> &Succ, vector<vector<string>> &uses) {
    set<string> ret = self;
    for(int next: Succ) {
        vector<string> use = uses[next];
        ret.insert(begin(use), end(use));
    }
    bool isChange = self.size() != ret.size();
    return {isChange, ret};
}

pair<bool, set<string>> MakeIn(set<string> &out, vector<string> &def, vector<string> &use, set<string> &in) {
    set<string> ret = out;
    // out - def
    for(string &s : def) {
        ret.erase(s);
    }
    // use U (out - def)
    ret.insert(begin(use), end(use));
    
    return {in != ret, ret};
}

pair<vector<set<string>>, vector<set<string>>> WorkList(vector<vector<string>> &defs, vector<vector<string>> &uses, unordered_map<int, vector<int>> &graph, unordered_map<int, vector<int>> &pred) {
    vector<set<string>> in(defs.size()), out(defs.size());
    unordered_set<int> workList;
    // init workList
    for(int i = 0; i < defs.size(); ++i)
        workList.insert(i);
    
    while(workList.empty() == false) {
        int ind = *workList.begin();
        workList.erase(workList.begin());
        
        auto [isChangeOut, tmpOut] = UnionSucc(out[ind], graph[ind], uses);  // union all succ as out
        out[ind] = tmpOut;
        
        auto [isChangeIn, tmpIn] = MakeIn(out[ind], defs[ind], uses[ind], in[ind]);
        in[ind] = tmpIn;
        
        if(isChangeOut || isChangeIn)
            workList.insert(begin(pred[ind]), end(pred[ind]));
    }
    return {in, out};
}

int main(int argc, char **argv) {
    json program = ReadStdin();
    
    for(auto &func: program["functions"]) {
        auto [BBs, graph] = BuildCFG(func["instrs"]);
        auto [defs, uses] = BuildLiveness(BBs);
        auto pred = BuildPred(graph);
        
        auto [in, out] = WorkList(defs, uses, graph, pred);
        
        print(defs, uses);
        print(in, out);
        
        
        for(BasicBlock &bb: BBs) {
            
            bb.print();
        }
        func["instrs"] = convertToInstrs(BBs);
    }
    // pretty printer
    // cout << program.dump(2) << endl;
    
    
    return 0;
}
