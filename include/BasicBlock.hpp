
class BasicBlock {
public:
    vector<json> instrs;
    
    
    // methods
    void push_back(json inst) {
        instrs.push_back(inst);
    }
    
};