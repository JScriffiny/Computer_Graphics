#ifndef VERTEX_ATTR_HPP
#define VERTEX_ATTR_HPP

//A structure to manage parameters of vertex attributes.
struct Vertex_Attribute {
    int num_per_vertex;
    int data_type; //GL_FLOAT
    int stride_bytes;
    int offset_bytes;
};

//Builds and returns a vertex attribute structure.
Vertex_Attribute build_vertex_attr (int num_per, int data_type, int stride, int offset);

#endif //VERTEX_ATTR_HPP
