#ifndef __TEST_H__
#define __TEST_H__

#include "define.h"
#include "node.h"

double parseTestLhsBracket(const char ** strp,
                           const std::vector<Node *> & funcv,
                           std::stringstream & testInfo);

//parse the parameters of the first argument from a test statement
//parse something like 'valA valB ...)'
//mutual recursion with parseTestLhsBracket()
double parseTestLhsParams(const char ** strp,
                          Node * func,
                          const std::vector<Node *> & funcv,
                          std::stringstream & testInfo) {
  assert(func->paramv.size() > 0);
  skipSpace(strp);
  if (**strp == '\0' || **strp == '#') {
    std::stringstream ss;
    ss << "end of line found in the first arument (lhs) after a func name";
    throw std::invalid_argument(ss.str());
  }
  for (size_t i = 0; i < func->paramv.size(); i++) {
    double num = 1;
    if (**strp == '(') {
      num = parseTestLhsBracket(strp, funcv, testInfo);
    }
    else {
      char * endp;
      num = strtod(*strp, &endp);
      if (endp == *strp) {
        std::stringstream ss;
        ss << "found non-number symbal or the param number does not match "
           << "in the first argument (lhs)";
        throw std::invalid_argument(ss.str());
      }
      *strp = endp;
    }
    func->paramv[i]->value = num;
    size_t infosz = testInfo.str().length();
    if (infosz > 1) {
      std::string snm(1, testInfo.str()[infosz - 2]);
      if (testInfo.str()[infosz - 1] != ')' ||
          (!isNumber(snm) && testInfo.str()[infosz - 2] != ')')) {
        testInfo << func->paramv[i]->value;
      }
    }
    if (i < func->paramv.size() - 1) {
      testInfo << " ";
    }
    skipSpace(strp);
  }
  if (**strp != ')') {
    std::stringstream ss;
    ss << "in the first argument (lhs) expected ) but found " << *strp;
    throw std::invalid_argument(ss.str());
  }
  testInfo << ")";
  *strp = *strp + 1;
  return func->evaluate();
}

//parse the the first argument from a test statement
//parse something like '(func valA valB ...)'
//mutual recursion with parseTestLhsParams()
double parseTestLhsBracket(const char ** strp,
                           const std::vector<Node *> & funcv,
                           std::stringstream & testInfo) {
  //start at '('
  *strp = *strp + 1;
  skipSpace(strp);
  std::string funcid;
  while (!isspace(**strp) && **strp != ')' && **strp != '\0' && **strp != '#') {
    funcid.push_back(**strp);
    *strp = *strp + 1;
  }
  if (!isspace(**strp)) {
    std::stringstream ss;
    ss << "expect a space after func name but found " << *strp;
    throw std::invalid_argument(ss.str());
  }
  Node * func = NULL;
  if ((func = findFunc(funcid, funcv)) == NULL) {
    std::stringstream ss;
    ss << "attempt to call a undefined func '" << funcid << "'";
    throw std::invalid_argument(ss.str());
  }
  testInfo << func->id << "(";
  return parseTestLhsParams(strp, func, funcv, testInfo);
}

//parse the second argument from a test statement, which should be a number
double parseTestRhs(const char ** strp) {
  //only accpets a number
  skipSpace(strp);
  char * endp;
  double num = strtod(*strp, &endp);
  if (endp == *strp) {
    std::stringstream ss;
    ss << "in the second argument (rhs) expeced a number but found " << *strp;
    throw std::invalid_argument(ss.str());
  }
  *strp = endp;
  return num;
}

//the main function of paring a test statement
//first, parse the test statement
//next, calculate the expected value
//then, compare the second argument (guessed value) with the expected value to
//see if they are match
double parseTest(const char ** strp,
                 const std::vector<Node *> & funcv,
                 std::stringstream & testInfo) {
  skipSpace(strp);
  if (!checkCommand(strp, "test")) {
    std::stringstream ss;
    ss << "invalid test identifier";
    throw std::invalid_argument(ss.str());
  }
  skipSpace(strp);
  if (**strp != '(') {
    std::stringstream ss;
    ss << "expected ( but found" << *strp;
    throw std::invalid_argument(ss.str());
  }
  double trueVal = parseTestLhsBracket(strp, funcv, testInfo);
  double testVal = parseTestRhs(strp);
  testInfo << " = " << testVal;
  if (std::abs(trueVal - testVal) < 1e-12) {
    testInfo << " [correct]\n";
  }
  else {
    testInfo << " [INCORRECT: expected " << trueVal << "]\n";
  }
  return trueVal;
}

#endif
