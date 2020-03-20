#ifndef __MCINT_H__
#define __MCINT_H__

#include <ctime>

#include "define.h"
#include "node.h"
#include "numint.h"

typedef std::pair<std::vector<double>, std::vector<double> > param_bound_t;

//parse the number of sample points from the arguments of 'mcint'
long parseMcIntTrial(const char ** strp) {
  skipSpace(strp);
  char * endp;
  long num = strtol(*strp, &endp, 10);
  if (endp == *strp) {
    std::stringstream ss("attempt to assign a non-integer to trial");
    throw std::invalid_argument(ss.str());
  }
  *strp = endp;
  if (num < 1) {
    std::stringstream ss("trial must be not less then 1");
    throw std::invalid_argument(ss.str());
  }
  return num;
}

//parse the integrate range from the arguments of 'mcint'
param_range_t parseMcIntParamv(const char ** strp, Node * func) {
  return parseNumIntParamv(strp, func);
}

//generate a random double number from min to max inclusively
double generateRandomDouble(const double min, const double max) {
  if (min >= max) {
    std::stringstream ss("invalid range for generating random double (min >= max)");
    throw std::invalid_argument(ss.str());
  }
  double random = min + (max - min) * double(rand() % RAND_MAX) / double(RAND_MAX - 1);
  if (random < min || random > max) {
    std::stringstream ss("random double is out of bound (random < min or random > max");
    throw std::invalid_argument(ss.str());
  }
  return random;
}

//update the minimum parameter in each iteration when calculating the integration
void updateMinParamv(const Node * func, param_bound_t & bound, const size_t idx) {
  assert(bound.first.size() == bound.second.size());
  if (func->paramv[idx]->value < bound.first[idx]) {
    bound.first[idx] = func->paramv[idx]->value;
  }
}

//update the maximum parameter in each iteration when calculating the integration
void updateMaxParamv(const Node * func, param_bound_t & bound, const size_t idx) {
  assert(bound.first.size() == bound.second.size());
  if (func->paramv[idx]->value > bound.second[idx]) {
    bound.second[idx] = func->paramv[idx]->value;
  }
}

//calculate the MC integration
double calcMcInt(Node * func, const param_range_t & paramRangev, const long trial) {
  param_bound_t bound;
  double ans = 0;
  srand(time(NULL));
  for (size_t i = 0; i < func->paramv.size(); i++) {
    func->paramv[i]->value = generateRandomDouble(paramRangev[i].first, paramRangev[i].second);
    bound.first.push_back(func->paramv[i]->value);
    bound.second.push_back(func->paramv[i]->value);
  }
  ans += func->evaluate();
  for (long k = 1; k < trial; k++) {
    for (size_t i = 0; i < func->paramv.size(); i++) {
      func->paramv[i]->value = generateRandomDouble(paramRangev[i].first, paramRangev[i].second);
      updateMinParamv(func, bound, i);
      updateMaxParamv(func, bound, i);
    }
    ans += func->evaluate();
  }
  ans /= double(trial);
  for (size_t i = 0; i < func->paramv.size(); i++) {
    double dif = bound.second[i] - bound.first[i];
    if (dif < 1e-12) {
      std::stringstream ss;
      ss << "upper bond is too close to the lower bound";
      throw std::invalid_argument(ss.str());
    }
    ans *= dif;
  }
  return ans;
}

//the main function of paring mcint statement
//returning a double number as the result of MC integration
double parseMcInt(const char ** strp, const std::vector<Node *> & funcv) {
  bool hasMcInt = false;
  hasMcInt = checkCommand(strp, "mcint");
  if (!hasMcInt || !isspace(**strp)) {
    std::stringstream ss("invalid mcint identifier");
    throw std::invalid_argument(ss.str());
  }
  Node * func = parseFunc(strp, funcv);
  long trial = parseMcIntTrial(strp);
  param_range_t paramRangev = parseMcIntParamv(strp, func);
  return calcMcInt(func, paramRangev, trial);
}

#endif
