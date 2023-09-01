

import json
import sys

def isControlInst(inst):
    return "op" in inst and (inst["op"] == "jmp" or inst["op"] == "br")

def isLabel(inst):
    return "label" in inst

# BB will start with a label or end with a control instruction
def CountNumberOfBasicBlocks(func):
    instList = func["instrs"]
    n = len(instList)
    cnt = 1
    
    for i in range(0, n - 1):
        cur = instList[i]; next = instList[i + 1]
        isBound = isControlInst(cur) or isLabel(next)
        if isBound: cnt += 1
    
    return cnt
    

def main():
    # read file
    brilJson = json.loads(sys.stdin.read())
    
    functions = brilJson["functions"]
    for func in functions:
        BBs = CountNumberOfBasicBlocks(func)
        funcName = func["name"]
        print(f"{funcName}:{BBs}")

if __name__ == "__main__":
    main()