#ifndef TETMESH_HPP
#define TETMESH_HPP

#include <bits/stdc++.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "point.hpp"
#include "tetra.hpp"

class TetMesh
{
private:
    Point* points;
    Tetra* tetras;
    long num_points;
    long num_tetras;
    Point pos; // position in world space
public:
    TetMesh(Point pos) { this->pos.set(&pos); }
    TetMesh(double x, double y, double z) { this->pos.set(x,y,z); }
    ~TetMesh() { delete points; };
    void read_vtk_file(std::string filename);

    // return all its points coordinates in world space
    // and in a double[] array instead of a Point[] array
    // note: this allocates memory on the heap which must be freed
    const double* get_points();
    inline const GLuint get_num_points() { return this->num_points; };

    // return a pointer to an array of indices in order to draw triangles
    // note: this allocates memory on the heap which must be freed
    const GLuint* get_indices();
    // for each tetrahedron, there are four triangles i.e. 12 indices
    inline const GLuint get_num_indices() { return 12 * this->num_tetras; }
};

#endif