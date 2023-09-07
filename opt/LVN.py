import json
import sys

    

def main():
    # read file
    brilJson = json.loads(sys.stdin.read())
    
    functions = brilJson["functions"]
    for func in functions:
        # BBs = CountNumberOfBasicBlocks(func)
        funcName = func["name"]
        # print(f"{funcName}:{BBs}")

if __name__ == "__main__":
    main()