#ifndef __NUMINT_H__
#define __NUMINT_H__

#include "define.h"
#include "node.h"

typedef std::vector<std::pair<double, double> > param_range_t;

//check if the statement end in the middle
void checkArgAsmMidEnd(const char ** strp) {
  if (**strp == '\0' || **strp == '#') {
    std::stringstream ss("end or invalid input in the mid of argument assignment");
    throw std::invalid_argument(ss.str());
  }
}

double parseDoubleNumber(const char ** strp) {
  char * endp;
  double num = strtod(*strp, &endp);
  if (endp == *strp) {
    std::stringstream ss;
    ss << "attempt to assign a non-number to an argument";
    throw std::invalid_argument(ss.str());
  }
  *strp = endp;
  return num;
}

//parse the function name and return the graph of that function if found in
//the function vector 'funcv'
Node * parseFunc(const char ** strp, const std::vector<Node *> & funcv) {
  skipSpace(strp);
  std::string funcid;
  while (!isspace(**strp) && **strp != '\0' && **strp != '#') {
    funcid.push_back(**strp);
    *strp = *strp + 1;
  }
  if (!isspace(**strp)) {
    std::stringstream ss;
    ss << "end after the first argument";
    throw std::invalid_argument(ss.str());
  }
  Node * func = NULL;
  if ((func = findFunc(funcid, funcv)) == NULL) {
    std::stringstream ss;
    ss << "attempt to call a undfined func '" << funcid << "'";
    throw std::invalid_argument(ss.str());
  }
  return func;
}

//parse integrate width from numint's arguments
double parseNumIntWidth(const char ** strp) {
  skipSpace(strp);
  double num = parseDoubleNumber(strp);
  if (num < 1e-12) {
    std::stringstream ss;
    ss << "width must be a positive number";
    throw std::invalid_argument(ss.str());
  }
  return num;
}

//parse integrate range from numint's arguments
param_range_t parseNumIntParamv(const char ** strp, Node * func) {
  assert(func->paramv.size() > 0);
  skipSpace(strp);
  param_range_t paramRangev;
  for (size_t i = 0; i < func->paramv.size(); i++) {
    std::pair<double, double> paramRange;
    for (size_t j = 0; j < 2; j++) {
      checkArgAsmMidEnd(strp);
      double num = parseDoubleNumber(strp);
      if (j == 0) {
        paramRange.first = num;
      }
      else if (j == 1) {
        paramRange.second = num;
      }
      skipSpace(strp);
    }
    if (paramRange.first >= paramRange.second) {
      std::stringstream ss("invalid range (start >= end)");
      throw std::invalid_argument(ss.str());
    }
    paramRangev.push_back(paramRange);
  }
  if (**strp != '\0' && **strp != '#') {
    std::stringstream ss;
    ss << "expect to end the line but found " << *strp;
    throw std::invalid_argument(ss.str());
  }
  return paramRangev;
}

//recursive helper of calculating 'trapezoid volumn'
double calcTrapezoidHelper(Node * func,
                           const std::vector<double> & orgParamv,
                           const double width,
                           int sz,
                           double ans) {
  if (sz == 0) {
    return ans + func->evaluate();
  }
  for (int i = 0; i < 2; i++) {
    ans = calcTrapezoidHelper(func, orgParamv, width, sz - 1, ans);
    func->paramv[sz - 1]->value += width;
  }
  func->paramv[sz - 1]->value = orgParamv[sz - 1];
  return ans;
}

//calculate 'trapezoid volumn'
double calcTrapezoid(Node * func, const double width) {
  int sz = func->paramv.size();
  assert(sz > 0);
  std::vector<double> orgParamv;
  for (int i = 0; i < sz; i++) {
    orgParamv.push_back(func->paramv[i]->value);
  }
  return calcTrapezoidHelper(func, orgParamv, width, sz, 0) / std::pow(2, sz) * std::pow(width, sz);
}

//recursive helper of calculating integration
double calcNumIntHelper(Node * func,
                        const param_range_t & paramRangev,
                        const double width,
                        int sz,
                        double ans) {
  if (sz == 0) {
    return ans + calcTrapezoid(func, width);
  }
  while (func->paramv[sz - 1]->value < paramRangev[sz - 1].second) {
    ans = calcNumIntHelper(func, paramRangev, width, sz - 1, ans);
    func->paramv[sz - 1]->value += width;
  }
  func->paramv[sz - 1]->value = paramRangev[sz - 1].first;
  return ans;
}

//calculate integration
double calcNumInt(Node * func, const param_range_t & paramRangev, const double width) {
  int sz = func->paramv.size();
  assert(sz > 0 && sz == int(paramRangev.size()));
  for (int i = 0; i < sz; i++) {
    func->paramv[i]->value = paramRangev[i].first;
  }
  return calcNumIntHelper(func, paramRangev, width, sz, 0);
}

//main function of parsint numint statement
//returning a double nunber as the result of integration
double parseNumInt(const char ** strp, const std::vector<Node *> & funcv) {
  bool hasNumInt = false;
  hasNumInt = checkCommand(strp, "numint");
  if (!hasNumInt || !isspace(**strp)) {
    std::stringstream ss;
    ss << "invalid numint identifier";
    throw std::invalid_argument(ss.str());
  }
  Node * func = parseFunc(strp, funcv);
  double width = parseNumIntWidth(strp);
  param_range_t paramRangev = parseNumIntParamv(strp, func);
  return calcNumInt(func, paramRangev, width);
}

#endif
