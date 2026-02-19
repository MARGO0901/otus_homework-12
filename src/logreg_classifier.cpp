#include <kdd99/logreg_classifier.h>

#include <cassert>
#include <numeric>
#include <cmath>

using kdd99::LogregClassifier;

namespace  kdd99 {

template<typename T>
auto sigma(T x) {
    return 1/(1 + std::exp(-x));
}

LogregClassifier::LogregClassifier(const coef_t& coef)
    : coef_{coef}
{
    assert(!coef_.empty());
}

float LogregClassifier::predict_proba(const features_t& feat) const {
    auto z = std::inner_product(feat.begin(), feat.end(), ++coef_.begin(), coef_.front());
    return sigma(z);
}

}
