
class BasicBlock {
public:
    vector<json> instrs;
    
    // constructor
    BasicBlock() {
    }
    
    // methods
    void push_back(json inst) {
        instrs.push_back(inst);
    }
    
    void print() {
        cout << "{\n";
        for(json &inst: instrs) {
            cout << "  " << inst << endl;
        }
        cout << "}\n";
    }
    
};