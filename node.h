#ifndef __NODE_H__
#define __NODE_H__

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

//typedef std::pair<std::string, double> value_t;

bool isNumber(const std::string & str) {
  char * endptr = NULL;
  strtod(str.c_str(), &endptr);
  if (*endptr == '\0') {
    return true;
  }
  return false;
}

bool isOperator(const std::string & str) {
  //code cited from homework 081
  if (str.length() != 1) {
    return false;
  }
  return strchr("+-*/%", str.at(0));
}

class Node
{
 public:
  std::string id;
  double value;
  std::vector<Node *> paramv;
  Node * left;
  Node * right;

  Node(std::string id_) : id(id_), value(1.0), paramv(), left(NULL), right(NULL) {}
  Node(std::string id_, double value_) :
      id(id_),
      value(value_),
      paramv(),
      left(NULL),
      right(NULL) {}
  Node(std::string id_, std::vector<Node *> paramv_) :
      id(id_),
      value(1.0),
      paramv(paramv_),
      left(NULL),
      right(NULL) {}
  Node(Node * left_, Node * right_) : id(), value(1.0), paramv(), left(left_), right(right_) {}
  Node(std::string id_, Node * left_, Node * right_) :
      id(id_),
      value(1.0),
      paramv(),
      left(left_),
      right(right_) {}
  Node(std::string id_, double value_, std::vector<Node *> paramv_, Node * left_, Node * right_) :
      id(id_),
      value(value_),
      paramv(paramv_),
      left(left_),
      right(right_) {}
  virtual ~Node() {}
  virtual std::string toString() = 0;
  virtual double evaluate() = 0;
};

class ExpressionNode : public Node
{
 public:
  ExpressionNode() : Node(NULL, NULL) {}
  ExpressionNode(std::string id_) : Node(id_) {}
  ExpressionNode(std::string id_, double value_) : Node(id_, value_) {}
  ExpressionNode(std::string id_, Node * left_, Node * right_) : Node(id_, left_, right_) {}
  virtual ~ExpressionNode() {}
  virtual std::string toString() {
    std::stringstream ss;
    if (isNumber(id)) {
      //number
      assert(left == NULL && right == NULL);
      ss << id;
    }
    else if (isOperator(id)) {
      //operator
      assert(left != NULL && right != NULL);
      ss << "(" << left->toString() << " " << id << " " << right->toString() << ")";
    }
    else {
      //parameter
      assert(left == NULL && right == NULL);
      ss << id << "(" << value << ")";
    }
    return ss.str();
  }
  virtual double evaluate() {
    if (isNumber(id)) {
      //number
      assert(left == NULL && right == NULL);
      return value;
    }
    else if (isOperator(id)) {
      //operator
      assert(left != NULL && right != NULL);
      value = evaluateOp(this);
      return value;
    }
    else {
      //parameter
      assert(left == NULL && right == NULL);
      return value;
    }
  }

  friend void checkDivOp(const Node * op, const char * msg);
  friend void checkModOps(const Node * op, const char * msg);
  friend double evaluateOp(const Node * op);
};

class FunctionNode : public Node
{
 public:
  FunctionNode() : Node(NULL, NULL) {}
  FunctionNode(std::string id_, std::vector<Node *> paramv_) : Node(id_, paramv_) {}
  FunctionNode(std::string id_, double value_) : Node(id_, value_) {}
  virtual ~FunctionNode() {}
  virtual std::string toString() {
    assert(left != NULL && right == NULL);
    std::stringstream ss;
    if (left != NULL) {
      ss << left->toString();
    }
    return ss.str();
  }
  virtual double evaluate() {
    assert(left != NULL && right == NULL);
    value = left->evaluate();
    return value;
  }
};

class SinFunctionNode : public FunctionNode
{
 public:
  SinFunctionNode() : FunctionNode("sin", std::sin(1)) {}
  virtual ~SinFunctionNode() {}
  virtual std::string toString() {
    assert(left != NULL && right == NULL);
    assert(paramv.size() == 1);
    std::stringstream ss;
    ss << id << "(" << left->toString() << ")";
    return ss.str();
  }
  virtual double evaluate() {
    assert(left != NULL && right == NULL);
    assert(paramv.size() == 1);
    value = std::sin(left->evaluate());
    return value;
  }
};

class CosFunctionNode : public FunctionNode
{
 public:
  CosFunctionNode() : FunctionNode("cos", std::cos(1)) {}
  virtual ~CosFunctionNode() {}
  virtual std::string toString() {
    assert(left != NULL && right == NULL);
    assert(paramv.size() == 1);
    std::stringstream ss;
    ss << id << "(" << left->toString() << ")";
    return ss.str();
  }
  virtual double evaluate() {
    assert(left != NULL && right == NULL);
    assert(paramv.size() == 1);
    value = std::cos(left->evaluate());
    return value;
  }
};

class SqrtFunctionNode : public FunctionNode
{
 public:
  SqrtFunctionNode() : FunctionNode("sqrt", std::sqrt(1)) {}
  virtual ~SqrtFunctionNode() {}
  virtual std::string toString() {
    assert(left != NULL && right == NULL);
    assert(paramv.size() == 1);
    std::stringstream ss;
    ss << id << "(" << left->toString() << ")";
    return ss.str();
  }
  virtual double evaluate() {
    assert(left != NULL && right == NULL);
    assert(paramv.size() == 1);
    double leftVal = left->evaluate();
    if (leftVal < 0) {
      std::stringstream ss;
      ss << "Invalid sqrt: attempt to sqrt a negative value";
      throw std::invalid_argument(ss.str());
    }
    value = std::sqrt(leftVal);
    return value;
  }
};

class LnFunctionNode : public FunctionNode
{
 public:
  LnFunctionNode() : FunctionNode("ln", std::log(1)) {}
  virtual ~LnFunctionNode() {}
  virtual std::string toString() {
    assert(left != NULL && right == NULL);
    assert(paramv.size() == 1);
    std::stringstream ss;
    ss << id << "(" << left->toString() << ")";
    return ss.str();
  }
  virtual double evaluate() {
    assert(left != NULL && right == NULL);
    assert(paramv.size() == 1);
    double leftVal = left->evaluate();
    if (std::abs(leftVal) < 1e-12 || leftVal < 0) {
      std::stringstream ss;
      ss << "Invalid sqrt: attempt to ln a zero or negative value";
      throw std::invalid_argument(ss.str());
    }
    value = std::log(leftVal);
    return value;
  }
};

class PowFunctionNode : public FunctionNode
{
 public:
  PowFunctionNode() : FunctionNode("pow", std::pow(1, 1)) {}
  virtual ~PowFunctionNode() {}
  virtual std::string toString() {
    assert(left != NULL && right == NULL);
    assert(paramv.size() == 2);
    std::stringstream ss;
    ss << id << "(" << left->toString() << ", " << paramv[1]->toString() << ")";
    return ss.str();
  }
  virtual double evaluate() {
    assert(left != NULL && right == NULL);
    assert(paramv.size() == 2);
    value = std::pow(left->evaluate(), paramv[1]->value);
    return value;
  }
};

void checkDivOp(const Node * op, const char * msg) {
  //friend of ExpressionNode
  //check validation of an division operation
  assert(isOperator(op->id));
  if (std::abs(op->right->evaluate() - 0) < 1e-12) {
    std::stringstream ss;
    ss << msg;
    throw std::invalid_argument(ss.str());
  }
}

void checkModOps(const Node * op, const char * msg) {
  //friend of ExpressionNode
  //check validation of modulo operation
  assert(isOperator(op->id));
  if (std::abs(op->left->evaluate() - int(op->left->evaluate())) >= 1e-12 ||
      std::abs(op->right->evaluate() - int(op->right->evaluate())) >= 1e-12) {
    std::stringstream ss;
    ss << msg;
    throw std::invalid_argument(ss.str());
  }
}

double evaluateOp(const Node * op) {
  //friend of ExpressionNode
  assert(isOperator(op->id));
  double ans;
  switch (op->id.at(0)) {
    case '+':
      ans = op->left->evaluate() + op->right->evaluate();
      break;
    case '-':
      ans = op->left->evaluate() - op->right->evaluate();
      break;
    case '*':
      ans = op->left->evaluate() * op->right->evaluate();
      break;
    case '/':
      checkDivOp(op, "Invalid division: denominator is 0");
      ans = op->left->evaluate() / op->right->evaluate();
      break;
    case '%':
      checkModOps(op, "Invalid modulo: invalid double operands found");
      checkDivOp(op, "Invalid modulo: denominator is 0");
      ans = int(op->left->evaluate()) % int(op->right->evaluate());
      break;
    default:
      std::stringstream ss;
      ss << "impossible op char: " << op->id;
      throw std::invalid_argument(ss.str());
  }
  return ans;
}

Node * createSinFunc() {
  Node * func = new SinFunctionNode();
  func->paramv.push_back(new ExpressionNode("x", 1));
  func->left = func->paramv[0];
  return func;
}

Node * createCosFunc() {
  Node * func = new CosFunctionNode();
  func->paramv.push_back(new ExpressionNode("x", 1));
  func->left = func->paramv[0];
  return func;
}

Node * createSqrtFunc() {
  Node * func = new SqrtFunctionNode();
  func->paramv.push_back(new ExpressionNode("x", 1));
  func->left = func->paramv[0];
  return func;
}

Node * createLnFunc() {
  Node * func = new LnFunctionNode();
  func->paramv.push_back(new ExpressionNode("x", 1));
  func->left = func->paramv[0];
  return func;
}

Node * createPowFunc() {
  Node * func = new PowFunctionNode();
  func->paramv.push_back(new ExpressionNode("x", 1));
  func->paramv.push_back(new ExpressionNode("1", 1));
  func->left = func->paramv[0];
  return func;
}

#endif
