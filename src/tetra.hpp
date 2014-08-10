#ifndef TETRA_HPP
#define TETRA_HPP

#include "point.hpp"
#include <bits/stdc++.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct Tetra
{
    GLuint points[4]; // points referred to by their tetmesh indices
    void set(GLuint pts[4]) {
        for (int i = 0 ; i < 4 ;i++) points[i] = pts[i];
    }
};

#endif