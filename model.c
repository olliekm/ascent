#include "model.h"
#include <math.h>

static float sigmoid(float z) {
    return 1 / (1 + exp(-z));
}

static float dot(const float *a, const float *b, int n) {
    float result = 0.0f;
    for (int i = 0; i < n; i++) {
        result += a[i] * b[i];
    }
    return result;
}

static float logistic_predict(const float *w, const float *x, int n) {
    return sigmoid(dot(w, x, n));
}

static float logistic_loss(const float *w, const float *X, const int *y, int m, int n) {
    float sum = 0.0f;
    for (int i = 0; i < m; i++){
        float p_i = logistic_predict(w, X + i * n, n);
        sum += y[i] * log(p_i) + (1-y[i]) * log(1-p_i);
    }
    return -1.0f/m * sum;
}

static void logistic_gradient(float *grad, const float *w, const float *X, const int *y, int m, int n) {
    for (int j = 0; j < n; j++) {
        grad[j] = 0.0f;
    }

    for (int i = 0; i < m; i++) {
        float p_i = logistic_predict(w, X + i*n, n);
        float error = p_i - y[i];
        for (int j = 0; j < n; j++) {
            grad[j] += error * X[i * n + j];
        }
    }

    for (int j = 0; j < n; j++) {
        grad[j] /= m;
    }
}

Model logistic_regression_model(void) {
    return (Model){
        .predict = logistic_predict,
        .loss = logistic_loss,
        .gradient = logistic_gradient
    };
}