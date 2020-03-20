#ifndef __MAXMIN_H__
#define __MAXMIN_H__

#include "define.h"
#include "mcint.h"
#include "node.h"
#include "numint.h"

enum MaxMinCommand { none, max, min };
typedef enum MaxMinCommand mm_command_t;

mm_command_t parseMaxMinCommand(const char ** strp) {
  skipSpace(strp);
  if (**strp == 'm' && *(*strp + 1) == 'a') {
    if (checkCommand(strp, "max")) {
      return max;
    }
  }
  else if (**strp == 'm' && *(*strp + 1) == 'i') {
    if (checkCommand(strp, "min")) {
      return min;
    }
  }
  return none;
}

double parseMaxMinArg(const char ** strp) {
  skipSpace(strp);
  double num = parseDoubleNumber(strp);
  return num;
}

long parseMaxMinMaxStep(const char ** strp) {
  return parseMcIntTrial(strp);
}

std::vector<double> parseMaxMinStartPoint(const char ** strp, const Node * func) {
  skipSpace(strp);
  assert(func->paramv.size() > 0);
  std::vector<double> startPoint;
  for (size_t i = 0; i < func->paramv.size(); i++) {
    checkArgAsmMidEnd(strp);
    startPoint.push_back(parseDoubleNumber(strp));
    skipSpace(strp);
  }
  checkArgAsmMidEnd(strp);
  return startPoint;
}

std::vector<double> calcGradient(Node * func, const std::vector<double> & point, double d) {
  assert(func->paramv.size() > 0 && func->paramv.size() == point.size());
  for (size_t i = 0; i < func->paramv.size(); i++) {
    func->paramv[i]->id = point[i];
  }
  std::vector<double> grad;
  for (size_t i = 0; i < func->paramv.size(); i++) {
    double f = func->evaluate();
    func->paramv[i]->id += d;
    double fPrime = func->evaluate();
    grad.push_back((fPrime - f) / d);
    func->paramv[i]->id = point[i];
  }
  return grad;
}

double calcDistance(const std::vector<double> & pointA, const std::vector<double> & pointB) {
  assert(pointA.size() > 0 && pointA.size() == pointB.size());
  double distance = 0;
  for (size_t i = 0; i < pointA.size(); i++) {
    distance += std::pow(pointB[i] - pointA[i], 2);
  }
  return std::sqrt(distance);
}

double calcMaxMin(Node * func,
                  const std::vector<double> & startPoint,
                  const double gamma,
                  const double convergeDisgance,
                  const long maxStep,
                  const mm_command_t command) {
  assert(command != none);
  assert(func->paramv.size() > 0 && func->paramv.size() == startPoint.size());
  std::vector<double> currPoint = startPoint;
  std::vector<double> newPoint;
  bool isConverge = false;
  for (long k = 0; k < maxStep; k++) {
    std::vector<double> grad = calcGradient(func, currPoint, 1e-10);
    for (size_t i = 0; i < startPoint.size(); i++) {
      if (command == max) {
        newPoint.push_back(currPoint[i] + gamma * grad[i]);
      }
      else {
        newPoint.push_back(currPoint[i] - gamma * grad[i]);
      }
    }
    if (calcDistance(currPoint, newPoint) < convergeDisgance) {
      isConverge = true;
      break;
    }
    else {
      currPoint = newPoint;
      newPoint.clear();
    }
  }
  if (!isConverge) {
    std::stringstream ss("Not exist");
    throw std::domain_error(ss.str());
  }
  assert(newPoint.size() == func->paramv.size());
  for (size_t i = 0; i < func->paramv.size(); i++) {
    func->paramv[i]->id = newPoint[i];
  }
  return func->evaluate();
}

double parseMaxMin(const char ** strp,
                   const std::vector<Node *> & funcv,
                   const mm_command_t maxMinCommand) {
  skipSpace(strp);
  Node * func = parseFunc(strp, funcv);
  checkArgAsmMidEnd(strp);
  double gamma = parseMaxMinArg(strp);
  checkArgAsmMidEnd(strp);
  double convergedDistance = parseMaxMinArg(strp);
  checkArgAsmMidEnd(strp);
  std::vector<double> startPoint = parseMaxMinStartPoint(strp, func);
  checkArgAsmMidEnd(strp);
  double maxStep = parseMaxMinMaxStep(strp);
  skipSpace(strp);
  if (**strp != '\0' && **strp != '#') {
    std::stringstream ss;
    ss << "except to end line but found " << *strp;
    throw std::invalid_argument(ss.str());
  }
  return calcMaxMin(func, startPoint, gamma, convergedDistance, maxStep, maxMinCommand);
}

double parseMaxMin(const char ** strp, const std::vector<Node *> & funcv) {
  mm_command_t cmd = none;
  cmd = parseMaxMinCommand(strp);
  if (cmd == none || !isspace(**strp)) {
    std::stringstream ss("invalid max/min identifier");
    throw std::invalid_argument(ss.str());
  }
  if (cmd == max) {
    return parseMaxMin(strp, funcv, max);
  }
  else {
    return parseMaxMin(strp, funcv, min);
  }
}

#endif
