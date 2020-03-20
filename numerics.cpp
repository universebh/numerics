#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stack>
#include <string>

#include "define.h"
//#include "maxmin.h"
#include "mcint.h"
#include "node.h"
#include "numint.h"
#include "test.h"

//numerics miniproject
//some codes are cited and used from homework 081 and c++ library documentation

//this is the file containing the main function
//node.h contains the abstraction of function definition as well as the creationg of build-in functions

//a function is abstracted as a node liked to a graph (describe as a function graph or the graph of a function)
//operaters, numbers and parameters are abstracted as single nodes

//define.h contains functions used for parsing 'define' statement
//test.h contains functions used for paring 'test' statement
//numint.h contains functions used for 'numint' statement
//mcint.h contains functions used for 'mcint' statement

//some functions dealing with the gradient ascend problem were written in maxmin.h, but
//it's incomplete because there are still some bugs that I have not figured out,
//therefore, maxmin.h is not included

void createBuiltInFuncs(std::vector<Node *> & bifv) {
  assert(bifv.size() == 0);
  bifv.push_back(createSinFunc());
  bifv.push_back(createCosFunc());
  bifv.push_back(createSqrtFunc());
  bifv.push_back(createLnFunc());
  bifv.push_back(createPowFunc());
}

bool doDefine(const char ** strp, std::vector<Node *> & sdfv, std::vector<Node *> & funcv) {
  Node * expr = NULL;
  if ((expr = parseDefine(strp, funcv)) != NULL) {
    sdfv.push_back(expr);
    funcv.push_back(expr);
    std::cout << getAFuncString(expr, false) << "\n";  //funcString does not have a '\n' at the end
    return true;
  }
  else {
    return false;
  }
}

bool doTest(const char ** strp, std::vector<Node *> & funcv, std::stringstream & testInfo) {
  try {
    parseTest(strp, funcv, testInfo);
    if (testInfo.str().size() > 0) {
      std::cout << testInfo.str();  //testInfo has a '\n' at the end
    }
  }
  catch (std::invalid_argument & e) {
    std::cerr << "Illegal test statement: " << e.what() << "\n";
    testInfo.str("");
    testInfo.clear();
    return false;
  }
  return true;
}

bool doNumInt(const char ** strp, const std::vector<Node *> & funcv) {
  try {
    double numInt = parseNumInt(strp, funcv);
    std::cout << numInt << "\n";
  }
  catch (std::invalid_argument & e) {
    std::cerr << "Illegal numint statement: " << e.what() << "\n";
    return false;
  }
  return true;
}

bool doMcInt(const char ** strp, const std::vector<Node *> & funcv) {
  try {
    double mcInt = parseMcInt(strp, funcv);
    std::cout << mcInt << "\n";
  }
  catch (std::invalid_argument & e) {
    std::cerr << "Illegal mcint statement: " << e.what() << "\n";
    return false;
  }
  return true;
}

//resereved for the gradient ascend problem

//bool doMaxMin(const char ** strp, const std::vector<Node *> & funcv) {
//  try {
//    double maxMin = parseMaxMin(strp, funcv);
//    std::cout << maxMin << "\n";
//  }
//  catch (std::domain_error & e) {
//    std::cout << e.what() << "\n";
//  }
//  catch (std::invalid_argument & e) {
//    std::cerr << "Illegal max/min statment: " << e.what() << "\n";
//    return false;
//  }
//  return true;
//}

//only used for testing
void printInfo(std::vector<Node *> & sdfv, std::stringstream & testInfo) {
  if (sdfv.size() > 0) {
    std::cout << genFuncInfo(sdfv, true) << "\n";
  }
  if (testInfo.str().length() > 0) {
    std::cout << testInfo.str();
  }
}

//decide which command (define, test, numint, mcint) to execute
bool decideAction(const char ** strp,
                  std::vector<Node *> & sdfv,
                  std::vector<Node *> & funcv,
                  std::stringstream & testInfo) {
  bool canParse = true;
  testInfo.str("");
  testInfo.clear();
  if (!isEocLine(strp)) {
    skipSpace(strp);
    if (**strp == 'd') {
      if (!doDefine(strp, sdfv, funcv)) {
        canParse = false;
      }
    }
    else if (**strp == 't') {
      if (!doTest(strp, funcv, testInfo)) {
        canParse = false;
      }
    }
    else if (**strp == 'n') {
      if (!doNumInt(strp, funcv)) {
        canParse = false;
      }
    }
    else if (**strp == 'm') {
      if (*(*strp + 1) == 'c') {
        if (!doMcInt(strp, funcv)) {
          canParse = false;
        }
      }
      //else {
      //  if (!doMaxMin(strp, funcv)) {
      //    canParse = false;
      //  }
      //}
    }
    else {
      canParse = false;
    }
  }
  return canParse;
}

bool readInput(std::vector<Node *> & sdfv, std::vector<Node *> & funcv, FILE * in) {
  assert(in != NULL);
  char * line = NULL;
  size_t sz;
  std::stringstream testInfo;
  while (getline(&line, &sz, in) != -1) {
    const char * temp = line;
    if (in != stdin) {
      std::cout << line;
    }
    if (!decideAction(&temp, sdfv, funcv, testInfo)) {
      std::cerr << "Could not parse expression.\n";
      free(line);
      return false;
    }
  }
  free(line);
  return true;
}

bool closeFile(FILE * f, const char * fileNm) {
  assert(f != stdin);
  if (fclose(f) != 0) {
    std::cerr << "An exception happened when closing '" << fileNm << "'\n";
    perror("Close failure");
    return false;
  }
  return true;
}

int main(int argc, char ** argv) {
  std::vector<Node *> bifv;  //built-in func vector
  createBuiltInFuncs(bifv);
  if (argc == 1) {
    std::vector<Node *> sdfv;  //self-defined func vector
    std::vector<Node *> funcv(bifv);
    if (!readInput(sdfv, funcv, stdin)) {
      clearFuncv(funcv);
      return EXIT_FAILURE;
    }
    clearFuncv(funcv);
  }
  else {
    for (int i = 1; i < argc; i++) {
      std::vector<Node *> sdfv;  //self-defined func vector
      std::vector<Node *> funcv(bifv);
      FILE * f = fopen(argv[i], "r");
      if (f == NULL) {
        std::cerr << "An exception happened when opening '" << argv[i] << "'\n";
        perror("Open failure");
        clearFuncv(funcv);
        return EXIT_FAILURE;
      }
      if (!readInput(sdfv, funcv, f)) {
        clearFuncv(funcv);
        if (!closeFile(f, argv[i])) {
          return EXIT_FAILURE;
        }
        return EXIT_FAILURE;
      }
      if (!closeFile(f, argv[i])) {
        clearFuncv(funcv);
        return EXIT_FAILURE;
      }
      clearFuncv(sdfv);
      if (i < argc - 1) {
        std::cout << "\n";
      }
    }
    clearFuncv(bifv);
  }
  return EXIT_SUCCESS;
}
