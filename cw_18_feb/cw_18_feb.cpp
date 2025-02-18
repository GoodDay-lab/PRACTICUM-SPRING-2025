/*
    Линейный классификатор, метод опорных векторов
*/

#include <bits/c++io.h>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/assignment.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace std;
namespace ublas = boost::numeric::ublas;

typedef double f64;
typedef long unsigned u64;
typedef ublas::vector<f64> vf64;
typedef ublas::matrix<f64> mf64;

class LinearClassifier
{
private:
    f64 lambda = 0.03;
    f64 learning = 0.01;
    vf64 weights;
public:
    LinearClassifier();
    ~LinearClassifier();

    void fit(mf64 &X_tr, vf64 &y_tr, u64 epoch = 100);
    vf64 predict(mf64 &X_ts);
    vf64 svm_grad(mf64 &X_tr, vf64 &y_tr);
    f64 svm_loss(mf64 &X_tr, vf64 &y_tr);

    void print_weights()
    {
        std::cout << "Weights:" << std::endl;
        for (u64 i = 0; i != weights.size(); i++)
            std::cout << weights[i] << ' ';
        std::cout << std::endl;
    }
};

LinearClassifier::LinearClassifier()
{
    ;
}

LinearClassifier::~LinearClassifier()
{
    ;
}

vf64 LinearClassifier::svm_grad(mf64 &X_tr, vf64 &y_tr)
{
    u64 i, j;
    vf64 grad = 2*lambda*weights, pred = ublas::prod(X_tr, weights);
    
    for (i = 0; i != X_tr.size2(); i++)
    {
        for (j = 0; j != X_tr.size1(); j++)
        {
            if (1 - pred[j] > 0) {
                grad[i] += -y_tr[j] * X_tr.at_element(j, i);
            }
        }
    }

    return grad;
}

void LinearClassifier::fit(mf64 &X_tr, vf64 &y_tr, u64 epoch)
{
    weights = vf64(X_tr.size2());
    u64 i = 0;
    vf64 grad;

    for (i = 0; i < epoch; i++)
    {
        grad = svm_grad(X_tr, y_tr);
        weights += -learning * grad;
    }
}

vf64 LinearClassifier::predict(mf64 &X_ts)
{
    return ublas::prod(X_ts, weights);
}

f64 LinearClassifier::svm_loss(mf64 &X_tr, vf64 &y_tr)
{
    u64 i;
    f64 loss = 0.0; vf64 pred = ublas::prod(X_tr, weights);

    for (i = 0; i < X_tr.size1(); i++) {
        f64 p = y_tr[i] * pred[i];
        if (1 - p > 0)
            loss += 1 - p;
    }

    return loss;
}

int main()
{
    vf64 y_train(5);
    mf64 X_train(5,3);

    X_train <<= 0.0,5.0,1,
                1.5,12.6,1,
                4.0,0.3,1,
                6.0,1.5,1,
                5.4,-1.2,1;
    
    y_train <<= 1,1,-1,-1,-1;
    
    LinearClassifier model;

    model.fit(X_train, y_train, 1);
    std::cout << "loss: " << model.svm_loss(X_train, y_train) << std::endl;

    model.fit(X_train, y_train, 100);
    vf64 pred = model.predict(X_train);

    for (u64 i = 0; i != pred.size(); i++) {
        std::cout << pred[i] << std::endl;
    }

    std::cout << "loss: " << model.svm_loss(X_train, y_train) << std::endl;
    model.print_weights();

    return 0;
}