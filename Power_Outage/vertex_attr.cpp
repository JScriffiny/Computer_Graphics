#include "vertex_attr.hpp"

Vertex_Attribute build_vertex_attr (int num_per, int data_type, int stride, int offset) {
    Vertex_Attribute va;
    va.num_per_vertex = num_per;
    va.data_type = data_type;
    va.stride_bytes = stride;
    va.offset_bytes = offset;
    return va;
}