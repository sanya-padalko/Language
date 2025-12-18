#include "calc.h"
#include "tree.h"

double Sum(Node_t* left, Node_t* right) {
    return GetValue(left) + GetValue(right);
}

double Sub(Node_t* left, Node_t* right) {
    return GetValue(left) - GetValue(right);
}

double Mul(Node_t* left, Node_t* right) {
    return GetValue(left) * GetValue(right);
}

double Div(Node_t* left, Node_t* right) {
    if (GetValue(right) == double(0))
        return NAN;

    return GetValue(left) / GetValue(right);
}

double Cos(Node_t* left, Node_t* right) {
    return cos(GetValue(right));
}

double Sin(Node_t* left, Node_t* right) {
    return sin(GetValue(right));
}

double Pow(Node_t* left, Node_t* right) {
    return pow(GetValue(left), GetValue(right));
}

double Sqrt(Node_t* left, Node_t* right) {
    double value = GetValue(right);
    if (value < 0)
        return NAN;

    return sqrt(value);
}

double Ln(Node_t* left, Node_t* right) {
    double a = GetValue(right);

    if (a <= 0) 
        return NAN;

    return log(a);
}

double Log(Node_t* left, Node_t* right) {
    double b = GetValue(right);
    double a = GetValue(left);

    if (a <= 0 || a == 1)
        return NAN;

    return log(b) / log(a);
}

double Exp(Node_t* left, Node_t* right) {
    double power = GetValue(right);

    return exp(power);
}

double Tan(Node_t* left, Node_t* right) {
    double a = GetValue(right);

    return tan(a);
}

double Cot(Node_t* left, Node_t* right) {
    double a = GetValue(right);

    if (sin(a) == 0)
        return NAN;

    return cos(a) / sin(a);
}

double Acos(Node_t* left, Node_t* right) {
    double a = GetValue(right);

    if (a < -1 || a > 1)
        return NAN;

    return acos(a);
}

double Asin(Node_t* left, Node_t* right) {
    double a = GetValue(right);

    if (a < -1 || a > 1)
        return NAN;

    return asin(a);
}

double Atan(Node_t* left, Node_t* right) {
    double a = GetValue(right);

    return atan(a);
}

double Acot(Node_t* left, Node_t* right) {
    double a = GetValue(right);

    return M_PI_2 - atan(a);
}