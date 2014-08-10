#include "tetmesh.hpp"

const double* TetMesh::get_points() {
    double* result = new double[3*this->num_points];
    for (GLuint i = 0 ; i < this->num_points ; i++) {
        result[3*i] = this->pos.x + this->points[i].x;
        result[3*i+1] = this->pos.y + this->points[i].y;
        result[3*i+2] = this->pos.z + this->points[i].z;
    }
    return result;
}

const GLuint* TetMesh::get_indices() {
    GLuint* result = new GLuint[12*num_tetras];
    int i = 0;
    for (int j = 0 ; j < num_tetras ; j++) {
        Tetra* t = &(this->tetras[j]);
        result[i]   = t->points[0]; result[i+1]  = t->points[1]; result[i+2]  = t->points[2];
        result[i+3] = t->points[0]; result[i+4]  = t->points[1]; result[i+5]  = t->points[3];
        result[i+6] = t->points[1]; result[i+7]  = t->points[2]; result[i+8]  = t->points[3];
        result[i+9] = t->points[0]; result[i+10] = t->points[2]; result[i+11] = t->points[3];
        i += 12;
    }
    return result;
}

void TetMesh::read_vtk_file(std::string filename) {
    try {
        std::ifstream is;
        is.open(filename.c_str());
        if (is.is_open()) {
            std::string line;
            GLuint line_num = 0;
            while (is.good()) {
                line_num++;
                getline(is, line);
                if (line[0] == '#') {
                    // std::cout << line_num << ": ignoring comment " << std::endl;
                    continue;
                }
                if (line == "") {
                    // std::cout << line_num << ": empty line " << std::endl;
                    continue;
                }
                if (line == "ASCII") {
                    // std::cout << line_num << ": saw ASCII line " << std::endl;
                    continue;
                }
                if (line.length() >= 6 and line.substr(0, 6) == "POINTS") {
                    // std::cout << line_num << ": saw POINTS line " << std::endl;
                    std::stringstream ss(line);
                    { std::string _; ss >> _; } // we don't care about this sring
                    ss >> this->num_points; this->points = new Point[this->num_points];
                    for (GLuint i = 0 ; i < num_points ; i++) {
                        line_num++;
                        is >> points[i].x >> points[i].y >> points[i].z;
                        // std::cout << "\t" << line_num << ": read point " << i << ": "; points[i].print();
                    }
                }
                if (line.length() >= 5 and line.substr(0, 5) == "CELLS") {
                    // std::cout << line_num << ": saw CELLS line " << std::endl;
                    std::stringstream ss(line);
                    { std::string _; ss >> _; } // we don't care about this sring
                    ss >> this->num_tetras;
                    this->tetras = new Tetra[this->num_tetras];
                    for (GLuint i = 0 ; i < num_tetras ; i++) {
                        line_num++;
                        { int _; is >> _; } // num points is always 4 for a tet mesh
                        GLuint indices[4]; is >> indices[0] >> indices[1] >> indices[2] >> indices[3];
                        this->tetras[i].set(indices);
                        // std::cout << "\t" << line_num << ": read tetra " << i << ": " << indices[0] << " " << indices[1] << " " << indices[2] << " " << indices[3] << std::endl;
                    }
                }
            }
        }
    }
    catch (const char* err) {
        std::cerr << err << std::endl;
    }
}