#ifndef POINT_HPP
#define POINT_HPP

#include <bits/stdc++.h>

struct Point {
    double x, y, z;
    void set(double _x, double _y, double _z) {
        x = _x;  y = _y; z = _z;
    }
    void set(Point* p) {
        x = p->x; y = p->y; z = p->z;
    }
    void print() {
        std::cout << x << " " << y << " " << z << std::endl;
    }
};

#endif