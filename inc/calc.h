#ifndef _CALC_H_
#define _CALC_H_

#include <math.h>

struct Node_t;

double Sum(Node_t* left, Node_t* right);
double Sub(Node_t* left, Node_t* right);
double Mul(Node_t* left, Node_t* right);
double Div(Node_t* left, Node_t* right);
double Cos(Node_t* left, Node_t* right);
double Sin(Node_t* left, Node_t* right);
double Ln(Node_t* left, Node_t* right);
double Log(Node_t* left, Node_t* right);
double Exp(Node_t* left, Node_t* right);
double Pow(Node_t* left, Node_t* right);
double Tan(Node_t* left, Node_t* right);
double Cot(Node_t* left, Node_t* right);
double Acos(Node_t* left, Node_t* right);
double Asin(Node_t* left, Node_t* right);
double Atan(Node_t* left, Node_t* right);
double Acot(Node_t* left, Node_t* right);

#endif