#include "nddgenerator.h"

nddgenerator::nddgenerator(int countnumbers) {
    seed = std::chrono::system_clock::now().time_since_epoch().count();
    this->countnumbers = countnumbers;
}


void nddgenerator::generate() {
    for (int i = 0; i < countnumbers; i++) {
        seed = (a*seed + c) % m;
        long double u = static_cast<long double>(seed) / m;
        seed = (a*seed + c) % m;
        long double v = static_cast<long double>(seed) / m;
        long double r = std::sqrt(-2.0 * std::log(u));
        float x = mu + r * std::cos(2.0 * M_PI * v)*sigma;
        float y = mu + r * std::sin(2.0 * M_PI * v)*sigma;
        normaldata  << new QVector2D{x, y};
    }
}
