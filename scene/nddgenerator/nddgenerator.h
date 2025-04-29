#ifndef NDDGENERATOR_H
#define NDDGENERATOR_H

#include <iostream>
#include <chrono>
#include <chrono>

#include <QVector2D>
#include <QList>

class nddgenerator {

    int countnumbers;
    unsigned long long seed;
    double sigma = 10.0;
    double mu = 15;
    const unsigned long long a = 1664525;
    const unsigned long long c = 1013904223;
    const unsigned long long m = 4294967296;

    public:

    QList<QVector2D*>  normaldata;
    nddgenerator(int countnumbers);
    void generate();
};

#endif //    NDDGENERATOR_H
