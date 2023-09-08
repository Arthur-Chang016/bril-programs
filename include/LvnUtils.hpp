
#include <string>
#include <vector>
using namespace std;

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
   return dynamic_cast<const Base*>(ptr) != nullptr;
}

class arg {
public:
    virtual ~arg() = default;
};

class argInt: arg {
public:
    long val;
};

class argVar: arg {
public:
    string var;
};

class Value {
public:
    string op;
    // vector<arg> args;
    vector<string> args;
    long constVal = 0;
    bool isConst = false;
    
    // constructor
    Value(string o, vector<string> a): op(o), args(a), isConst(false) {}
    Value(string o, long c): op(o), constVal(c), isConst(true) {}
    
    // comparator (for map)
    bool operator<(const Value& rhs) const {
        return op < rhs.op ? true : (args < rhs.args);
        
        // return pair<string, vector<string>>(op, args) < pair<string, vector<string>>(rhs.op, rhs.args);
    }
    
    // methods
    void print() {
        cout << "{ " << op << " ";
        if(isConst) {
            cout << constVal << ' ';
        } else {
            for(string &s: args) {
                cout << s << ' ';
            }
        }
        cout << "}";
    }
    
    
    /**
     * INT would be put at the front
     * Int... , strings...
    */
    // void sortArg() {
    //     sort(begin(args), end(args), [](arg &a, arg &b) {
    //         bool AisInt = instanceof<argInt>(&a), BisInt = instanceof<argInt>(&b);
            
    //         if(AisInt && BisInt) {
    //             return dynamic_cast<argInt*>(&a)->val < dynamic_cast<argInt*>(&b)->val;
    //         } else if(AisInt && !BisInt) {
    //             return true;
    //         } else if(!AisInt && BisInt) {
    //             return false;
    //         } else {  // string and string
    //             return dynamic_cast<argVar*>(&a)->var < dynamic_cast<argVar*>(&b)->var;
    //         }
    //     });
    // }
    
};

class LvnEntry {
public:
    Value value;
    string canVar;
    
    // constructor
    LvnEntry(Value v, string c): value(v), canVar(c) {
        
    }
    
    // methods
    void print() {
        value.print();
        cout << ' ' << canVar;
        // cout << value << ' ' << canVar;
    }
    
};