#ifndef MODEL_H
#define MODEL_H

typedef struct model {
    float (*predict)(const float *w, const float *x, int n);
    float (*loss)(const float *w, const float *X, const int* y, int m, int n);
    void (*gradient)(float* grad, const float *w, const float *X, const int* y, int m, int n);
} Model;

Model logistic_regression_model(void);

#endif