#include <fstream>
#include <iostream>
#include "json.hpp"
using json = nlohmann::json;
using namespace std;

json ReadStdin() {
    return json::parse(cin);
}

int main(int argc, char **argv) {
    json bril = ReadStdin();
    
    cout << bril << endl;
    
    
    
    
    return 0;
}

