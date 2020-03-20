#ifndef __DEFINE_H__
#define __DEFINE_H__

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stack>
#include <string>

#include "node.h"

void destroyParamv(std::vector<Node *> & pv);
void destroyNodes(Node * curr, const Node * func);
void destroyFunction(Node * expr);
Node * parseDefineRhs(const char ** strp,
                      Node * func,
                      std::stack<Node *> & stk,
                      const std::vector<Node *> & funcv);
void printStack(std::stack<Node *> stk);
void clearStack(std::stack<Node *> & stk, const Node * func);
Node * copyFunc(const Node * sou);
Node * copyFunc(const Node * sou, std::vector<Node *> & newParamv);

void skipSpace(const char ** strp) {
  //code cited from homework 081
  while (isspace(**strp)) {
    *strp = *strp + 1;
  }
}

bool isValidOp(char c) {
  //code cited from homework 081
  return strchr("+-*/%", c) != NULL;
}

//check if 'cmd' is a command (define, test, numint, mcint, ...)
bool checkCommand(const char ** strp, const char * cmd) {
  if (**strp == cmd[0]) {
    for (size_t i = 0; i < strlen(cmd); i++) {
      if (cmd[i] != **strp) {
        return false;
      }
      *strp = *strp + 1;
    }
  }
  else {
    return false;
  }
  if (!isspace(**strp) && **strp != '(') {
    return false;
  }
  return true;
}

//check if 'id' matches any function name in 'funcv'
//iterate 'funcv' to check if any elements matches 'id'
bool hasReapeatFunc(const std::vector<Node *> & funcv, const std::string id) {
  //inner functions
  if (funcv.size() > 0) {
    for (size_t i = 0; i < funcv.size(); i++) {
      if (funcv[i]->id == id) {
        return true;
      }
    }
  }
  return false;
}

//check if a parameter ('paramid') collide with the function name ('funcid')
//check if a parameter ('paramid') collide with any parameter name in 'pv'
bool hasRepeatParam(std::vector<Node *> & pv, std::string & funcid, std::string & paramid) {
  size_t sz = pv.size();
  if (paramid == funcid) {
    return true;
  }
  for (size_t i = 0; i < sz; i++) {
    if (paramid == pv[i]->id) {
      return true;
    }
  }
  return false;
}

//parse the first parameter's brackets (something like '(f x y)') in define
//create nodes of the function and its parameters
bool parseDefineLhsBracket(const char ** strp,
                           const std::vector<Node *> & funcv,
                           std::string & funcid,
                           std::vector<Node *> & pv,
                           std::string & paramid) {
  bool isFuncId = true;
  while (**strp != ')' && **strp != '\0' && **strp != '#') {
    paramid.clear();
    while (!isspace(**strp) && **strp != ')' && **strp != '\0' && **strp != '#') {
      //while (!isspace(**strp) && **strp != ')') {
      //std::cout << **strp << std::endl;
      if (!isalpha(**strp)) {
        std::cerr << "Illegal func def: '" << **strp << "' is not alphabet\n";
        destroyParamv(pv);
        return false;
      }
      if (isFuncId) {
        funcid.push_back(**strp);
      }
      else {
        paramid.push_back(**strp);
      }
      *strp = *strp + 1;
    }
    isFuncId = false;
    if (hasReapeatFunc(funcv, funcid)) {
      std::cerr << "Illegal func def: found collision function id '" << funcid << "'\n";
      destroyParamv(pv);
      return false;
    }
    if (paramid.length() > 0) {
      if (hasRepeatParam(pv, funcid, paramid)) {
        std::cerr << "Illegal func def: input repeated parameter(s)\n";
        destroyParamv(pv);
        return false;
      }
      pv.push_back(new ExpressionNode(paramid));
    }
    skipSpace(strp);
  }
  if (**strp == '\0' || **strp == '#') {
    std::cerr << "Incomplete func def: end of line before input ending bracket on lhs\n";
    destroyParamv(pv);
    return false;
  }
  return true;
}

//parse the first parameter (something like 'define(f x y)') in define
//first, parse 'define'
//then, create nodes of the function and its parameters
Node * parseDefineLhs(const char ** strp, const std::vector<Node *> & funcv) {
  //find define
  if (!checkCommand(strp, "define")) {
    std::cerr << "Illegal define identifier\n";
    return NULL;
  }
  skipSpace(strp);
  if (**strp != '(') {
    std::cerr << "Illegal func def: missing starting bracket\n";
    return NULL;
  }
  *strp = *strp + 1;
  skipSpace(strp);
  std::string funcid;
  std::vector<Node *> pv;
  std::string paramid;
  if (parseDefineLhsBracket(strp, funcv, funcid, pv, paramid) == false) {
    return NULL;
  }
  if (funcid.length() == 0 || pv.size() == 0) {
    std::cerr << "Incomplete func def: missing func name or param\n";
    destroyParamv(pv);
    return NULL;
  }
  return new FunctionNode(funcid, pv);
}

//parse the equal sign in the define statement
bool parseDefineEq(const char ** strp, Node * func) {
  skipSpace(strp);
  if (**strp != '=') {
    std::cerr << "Illegal func def: missing equal sign\n";
    destroyFunction(func);
    return false;
  }
  return true;
}

//find and return the parameter node from a fuction ('func')
//iterate the parameter vector (func->paramv)
Node * findParam(const std::string & str, const Node * func) {
  for (size_t i = 0; i < func->paramv.size(); i++) {
    if (str == func->paramv[i]->id) {
      return func->paramv[i];
    }
  }
  return NULL;
}

//find and return the parameter index from a function ('func')
//iterate the parameter vector (func->paramv)
int findParamIndex(const std::string & str, const Node * func) {
  for (size_t i = 0; i < func->paramv.size(); i++) {
    if (str == func->paramv[i]->id) {
      return i;
    }
  }
  return -1;
}

//add a vector of parameter node ('sou') to the back of another parameter node ('des')
//will not be used in the program
std::vector<Node *> catFuncv(std::vector<Node *> des, const std::vector<Node *> & sou) {
  for (size_t i = 0; i < sou.size(); i++) {
    des.push_back(sou[i]);
  }
  return des;
}

//find and return the function node from a vector of function node ('funcv')
Node * findFunc(const std::string & str, const std::vector<Node *> & funcv) {
  for (size_t i = 0; i < funcv.size(); i++) {
    if (str == funcv[i]->id) {
      return funcv[i];
    }
  }
  return NULL;
}

//push all build-in function names into a string vector (biidv)
//biidv: build-in function id vector
void createBuildInFuncIdv(std::vector<std::string> & biidv) {
  assert(biidv.size() == 0);
  biidv.push_back("sin");
  biidv.push_back("cos");
  biidv.push_back("sqrt");
  biidv.push_back("ln");
  biidv.push_back("pow");
}

//find and return the node of build-in function from a vector of function node ('funcv')
Node * findBuildInFunc(const std::string & str, const std::vector<Node *> & funcv) {
  std::vector<std::string> biidv;
  createBuildInFuncIdv(biidv);
  for (size_t i = 0; i < biidv.size(); i++) {
    if (str == biidv[i]) {
      return findFunc(str, funcv);
    }
  }
  return NULL;
}

//create the node of build-in function whose id matches 'str'
Node * createBuildInFuncNode(const std::string & str) {
  std::vector<std::string> biidv;
  createBuildInFuncIdv(biidv);
  for (size_t i = 0; i < biidv.size(); i++) {
    if (str == biidv[i]) {
      switch (i) {
        case 0:
          return new SinFunctionNode();
        case 1:
          return new CosFunctionNode();
        case 2:
          return new SqrtFunctionNode();
        case 3:
          return new LnFunctionNode();
        case 4:
          return new PowFunctionNode();
        default:
          std::cerr << "cannot create '" << str << "' func\n";
          return NULL;
      }
    }
  }
  return NULL;
}

//copy the head node, which is in the process of copy the graph of a function
//will not be used in the program
Node * copyHeadNode(const Node * sou, const Node * const desFunc) {
  if (sou->paramv.size() > 0) {
    return copyFunc(sou);
  }
  else {
    //number operator parameter
    if (sou->left == NULL && sou->right == NULL) {
      if (!isNumber(sou->id)) {
        Node * param = findParam(sou->id, desFunc);
        assert(param != NULL);
        return param;
      }
    }
    return new ExpressionNode(sou->id, sou->value);
  }
}

//a recursive helper for the copy of the graph of a function
//will not be used in the program
void copyFuncHelper(Node ** des, const Node * sou, const Node * const desFunc) {
  if (sou != NULL) {
    if (*des == NULL) {
      *des = copyHeadNode(sou, desFunc);
    }
    copyFuncHelper(&(*des)->left, sou->left, desFunc);
    copyFuncHelper(&(*des)->right, sou->right, desFunc);
  }
}

//copy the graph of a function ('sou') and return a new graph of the same function
//will not be used in the program
Node * copyFunc(const Node * sou) {
  assert(sou->paramv.size() > 0);
  Node * des = new FunctionNode(sou->id, sou->value);
  for (size_t i = 0; i < sou->paramv.size(); i++) {
    assert(sou->paramv[i]->left == NULL && sou->paramv[i]->right == NULL);
    des->paramv.push_back(new ExpressionNode(sou->paramv[i]->id, sou->paramv[i]->value));
  }
  copyFuncHelper(&des, sou, des);
  return des;
}

//another version of copying the head node, which is in the process of copy the graph of a function
//will be used in the program
Node * copyHeadNode(const Node * sou, const Node * const souFunc, std::vector<Node *> & newParamv) {
  if (sou->paramv.size() > 0) {
    std::cerr << "Illegal func def: attempt to call a function which was "
                 "defined by calling another function\n";
    return NULL;
  }
  else {
    //number operator parameter
    if (sou->left == NULL && sou->right == NULL) {
      //bottom layer
      if (!isNumber(sou->id)) {
        int idx = findParamIndex(sou->id, souFunc);
        assert(idx != -1);
        Node * nd = newParamv[idx];
        //newParamv.erase(newParamv.begin() + idx);
        return nd;
      }
    }
    return new ExpressionNode(sou->id, sou->value);
  }
}

//another version of a recursive helper for the copy of the graph of a function
//will be used in the program
bool copyFuncHelper(Node ** des,
                    const Node * sou,
                    const Node * const souFunc,
                    std::vector<Node *> & newParamv) {
  if (sou != NULL) {
    if (*des == NULL) {
      *des = copyHeadNode(sou, souFunc, newParamv);
      if (*des == NULL) {
        return false;
      }
    }
    if (!copyFuncHelper(&(*des)->left, sou->left, souFunc, newParamv)) {
      return false;
    }
    if (!copyFuncHelper(&(*des)->right, sou->right, souFunc, newParamv)) {
      return false;
    }
  }
  return true;
}

//another version of copyint the graph of a function ('sou') and
//return a new graph of the same function
//will be used in the program
Node * copyFunc(const Node * sou, std::vector<Node *> & newParamv) {
  assert(sou->paramv.size() > 0);
  //std::cout << sou->id << " " << sou->paramv.size() << " " << newParamv.size() << "\n";
  assert(sou->paramv.size() == newParamv.size());
  Node * des = NULL;
  if ((des = createBuildInFuncNode(sou->id)) == NULL) {
    des = new FunctionNode(sou->id, sou->value);
  }
  for (size_t i = 0; i < newParamv.size(); i++) {
    des->paramv.push_back(newParamv[i]);
  }
  if (!copyFuncHelper(&des, sou, sou, newParamv)) {
    if (des->left != NULL) {
      destroyNodes(des->left, des);
    }
    delete des;
    des = NULL;
    return NULL;
  }
  return des;
}

//parse operand and return the node of that operand
Node * parseOperand(const char ** strp, const Node * func) {
  //code cite form homework 081
  //check if *strp starts from a number
  char * endp;
  double num = strtod(*strp, &endp);
  //if *strp does not starts form a number
  if (endp == *strp) {
    std::string str;
    while (!isspace(**strp) && **strp != ')' && **strp != '\0' && **strp != '#') {
      str.push_back(**strp);
      *strp = *strp + 1;
    }
    //check if *strp starts form a parameter
    Node * param = NULL;
    //if *strp does not starts form a parameter
    if ((param = findParam(str, func)) == NULL) {
      std::cerr << "Illegal func def error: unknown operand '" << str << "'\n";
      return NULL;
    }
    return param;
  }
  *strp = endp;
  std::stringstream ss;
  ss << num;
  return new ExpressionNode(ss.str(), num);
}

//parse the expression (something like '(op lhs rhs)')
//mutual recursion with parseDefineRhs()
Node * parseDefineRhsOp(const char ** strp,
                        Node * func,
                        std::stack<Node *> & stk,
                        const std::vector<Node *> & funcv) {
  //code cited from homework 081
  skipSpace(strp);
  std::string opstr(1, **strp);
  if (!isOperator(opstr)) {
    std::cerr << "Illegal func def: invalid op '" << opstr << "'\n";
    return NULL;
  }
  *strp = *strp + 1;
  Node * lhs = parseDefineRhs(strp, func, stk, funcv);
  if (lhs == NULL) {
    return NULL;
  }
  stk.push(lhs);
  Node * rhs = parseDefineRhs(strp, func, stk, funcv);
  if (rhs == NULL) {
    return NULL;
  }
  stk.push(rhs);
  skipSpace(strp);
  if (**strp == ')') {
    *strp = *strp + 1;
    stk.pop();
    stk.pop();
    Node * expr = new ExpressionNode(opstr, lhs, rhs);
    return expr;
  }
  std::cerr << "Illegal func def: on rhs expected ) but found " << *strp << "\n";
  return NULL;
}

//helper function of parseFunc()
//help to create a vector of parameter id ('pv') from the graph of source function ('sou')
bool parseFuncHelper(const char ** strp,
                     Node * func,
                     const Node * sou,
                     std::vector<std::string> & pv) {
  for (size_t i = 0; i < sou->paramv.size(); i++) {
    skipSpace(strp);
    std::string param;
    while (!isspace(**strp) && **strp != ')' && **strp != '\0' && **strp != '#') {
      param.push_back(**strp);
      *strp = *strp + 1;
    }
    if (!isspace(**strp) && **strp != ')') {
      std::cerr << "Illegal func def: invalid func call on rhs (end when calling a func)\n";
      return false;
    }
    if (!isNumber(param) && findParamIndex(param, func) == -1) {
      std::cerr << "Illegal func def: invalid parameter '" << param << "' on rhs func call\n";
      return false;
    }
    pv.push_back(param);
  }
  for (size_t i = 0; i < pv.size(); i++) {
    if (pv[i].length() == 0) {
      std::cerr << "Illegal func def: unmatched argument num when calling '" << sou->id << "'\n";
      return false;
    }
  }
  skipSpace(strp);
  if (**strp != ')') {
    std::cerr << "Illegal func def: invalid func call on rhs. expected ) bur found " << *strp
              << "\n";
    return false;
  }
  *strp = *strp + 1;
  return true;
}

//parse the function called in the expression side of define (something like '(func a b)')
//steps:
//first, parse and find the function's name, know as source function
//second, continue parsing and create a vector of parameter id of that source function
//next, create the vector of new parameter node from the sourse function
//then, copy the graph of function from the sourse function according to the new parameter vector
//in the end, return the copied graph
Node * parseFunc(const char ** strp, Node * func, const std::vector<Node *> & funcv) {
  std::string id;
  while (!isspace(**strp) && **strp != ')' && **strp != '\0' && **strp != '#') {
    id.push_back(**strp);
    *strp = *strp + 1;
  }
  if (!isspace(**strp)) {
    std::cerr << "Illegal func def: invalid func call on rhs. "
              << "expected a space but found " << *strp << "\n";
    return NULL;
  }
  Node * sou = NULL;
  if ((sou = findFunc(id, funcv)) == NULL) {
    std::cerr << "Illegal func def: undefined operator or function '" << id << "' on rhs\n";
    return NULL;
  }
  assert(sou->paramv.size() > 0);
  std::vector<Node *> oldParamv;
  std::vector<Node *> addedParamv;
  std::vector<std::string> pv;
  if (!parseFuncHelper(strp, func, sou, pv)) {
    return NULL;
  }
  for (size_t i = 0; i < pv.size(); i++) {
    if (!isNumber(pv[i])) {
      oldParamv.push_back(findParam(pv[i], func));
    }
    else {
      char * endp;
      double num = strtod(pv[i].c_str(), &endp);
      addedParamv.push_back(new ExpressionNode(pv[i], num));
    }
  }
  std::vector<Node *> newParamv(oldParamv);
  for (size_t i = 0; i < addedParamv.size(); i++) {
    newParamv.push_back(addedParamv[i]);
  }
  Node * des = copyFunc(sou, newParamv);
  if (des == NULL) {
    destroyParamv(addedParamv);
    return NULL;
  }
  assert(des != NULL);
  return des;
}

//parse the expression side of 'define' (something like '(op lhs rhs)' or '(func paramA paramB)')
//mutual resursion with parseDefineRhsOp
Node * parseDefineRhs(const char ** strp,
                      Node * func,
                      std::stack<Node *> & stk,
                      const std::vector<Node *> & funcv) {
  //parse expression on the rhs of the func definition
  //code cite from homework 081
  skipSpace(strp);
  if (**strp == '\0' || **strp == '#') {
    std::cerr << "Illegal func def: end of line found mid rhs expression\n";
    return NULL;
  }
  else if (**strp == '(') {
    *strp = *strp + 1;
    skipSpace(strp);
    std::string opstr(1, **strp);
    //(func paramA paramB)
    if (!isOperator(opstr)) {
      return parseFunc(strp, func, funcv);
    }
    //(op E E)
    return parseDefineRhsOp(strp, func, stk, funcv);
  }
  else {
    //number, parameter, or function
    return parseOperand(strp, func);
  }
}

//main function of parsing the define statement
//create and return the head node of the graph of a function
Node * parseDefine(const char ** strp, const std::vector<Node *> & funcv) {
  skipSpace(strp);
  //if (**strp == '\0') {
  //  std::cerr << "blank line found\n";
  //  return NULL;
  //}
  //parsing define lhs
  Node * func = NULL;
  if ((func = parseDefineLhs(strp, funcv)) == NULL) {
    return NULL;
  }
  *strp = *strp + 1;

  //parsing equal sign
  if (!parseDefineEq(strp, func)) {
    return NULL;
  }
  *strp = *strp + 1;

  //parsing define rhs
  assert(func != NULL);
  assert(func->id.length() > 0 && func->paramv.size() > 0);
  std::stack<Node *> stk;
  if ((func->left = parseDefineRhs(strp, func, stk, funcv)) == NULL) {
    clearStack(stk, func);
    destroyFunction(func);
    return NULL;
  }
  assert(stk.size() == 0);
  //*strp has been increamented in parseDefineRhsOp()
  return func;
}

//only used for testing
void printNodes(Node * curr) {
  if (curr != NULL) {
    printNodes(curr->left);
    std::cout << curr->id << " ";
    printNodes(curr->right);
  }
}

//only used for testing
//print stack status, which is prepared for memory freeing
void printStack(std::stack<Node *> stk) {
  while (stk.size() > 0) {
    std::cout << stk.top()->id << "\n";
    stk.pop();
  }
}

//print a defined function as 'defined(func param...)'
std::string getAFuncString(Node * expr, bool showContent) {
  assert(expr != NULL);
  assert((expr->paramv).size() > 0);
  std::stringstream ss;
  ss << "defined ";
  ss << expr->id << "(";
  size_t sz = expr->paramv.size();
  for (size_t i = 0; i < sz; i++) {
    ss << expr->paramv[i]->id;
    if (i < sz - 1) {
      ss << " ";
    }
    else {
      ss << ")";
    }
  }
  if (showContent) {
    ss << " = " << expr->toString();
  }
  return ss.str();
}

//free memory of a vector of parameter node
void destroyParamv(std::vector<Node *> & pv) {
  size_t sz = pv.size();
  for (size_t i = 0; i < sz; i++) {
    if (pv[i] != NULL) {
      delete pv[i];
      pv[i] = NULL;
    }
  }
}

//free memory of a graph starting from the node 'curr'
//which should not be used for destroying graph of a fuction
void destroyNodes(Node * curr, const Node * func) {
  if (curr != NULL) {
    destroyNodes(curr->left, func);
    destroyNodes(curr->right, func);
    if (curr != NULL) {
      if (findParam(curr->id, func) == NULL) {
        delete curr;
      }
      curr = NULL;
    }
  }
}

//free memory of a graph starting from the node 'curr'
//which is used for destroying the graph of a fuction
void destroyNodes(Node * curr, std::vector<Node *> & paramv) {
  if (curr != NULL) {
    destroyNodes(curr->left, paramv);
    destroyNodes(curr->right, paramv);
    if (curr != NULL) {
      bool delNode = true;
      for (size_t i = 0; i < paramv.size(); i++) {
        if (curr == paramv[i]) {
          delNode = false;
        }
      }
      if (delNode) {
        delete curr;
      }
      curr = NULL;
    }
  }
}

//recursive helper of destroying the graph of a function
void destroyFuncHelper(Node * curr, std::vector<Node *> & paramv) {
  if (curr != NULL) {
    if (curr->paramv.size() > 0) {
      for (size_t i = 0; i < curr->paramv.size(); i++) {
        bool addToParamv = true;
        for (size_t j = 0; j < paramv.size(); j++) {
          if (curr->paramv[i] == paramv[j]) {
            addToParamv = false;
          }
        }
        if (addToParamv) {
          paramv.push_back(curr->paramv[i]);
        }
      }
    }
    destroyFuncHelper(curr->left, paramv);
    destroyFuncHelper(curr->right, paramv);
  }
}

//destroy the graph of a function
void destroyFunction(Node * expr) {
  assert(expr != NULL);
  assert((expr->paramv).size() > 0);
  std::vector<Node *> paramv;
  destroyFuncHelper(expr, paramv);
  destroyNodes(expr, paramv);
  destroyParamv(paramv);
  //if (expr->left != NULL) {
  //  destroyNodes(expr->left, expr);
  //}
  //destroyParamv(expr->paramv);
  //delete expr;
  //expr = NULL;
}

void clearStack(std::stack<Node *> & stk, const Node * func) {
  while (stk.size() != 0) {
    destroyNodes(stk.top(), func);
    stk.pop();
  }
}

bool isEocLine(const char ** strp) {
  //parse empty or comment line
  skipSpace(strp);
  if (**strp != '\0' && **strp != '#') {
    return false;
  }
  return true;
}

//only used for testing
std::string genFuncInfo(const std::vector<Node *> funcv, bool showContent) {
  std::stringstream ss;
  size_t infosz = funcv.size();

  for (size_t i = 0; i < infosz; i++) {
    ss << "defined " << funcv[i]->id << "(";
    size_t paramsz = funcv[i]->paramv.size();
    for (size_t j = 0; j < paramsz; j++) {
      ss << funcv[i]->paramv[j]->id;
      if (j < paramsz - 1) {
        ss << " ";
      }
      else {
        ss << ")";
      }
    }
    if (showContent) {
      ss << " = ";
      ss << funcv[i]->toString();
    }
    if (i < infosz - 1) {
      ss << "\n";
    }
  }
  return ss.str();
}

//free the memory for the vector of function graph
void clearFuncv(std::vector<Node *> & funcv) {
  for (size_t i = 0; i < funcv.size(); i++) {
    if (funcv[i] != NULL) {
      destroyFunction(funcv[i]);
      funcv[i] = NULL;
    }
  }
}

#endif
