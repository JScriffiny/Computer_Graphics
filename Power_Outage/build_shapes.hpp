#ifndef BUILD_SHAPES_HPP
#define BUILD_SHAPES_HPP
#include "shape.hpp"
#include <stdlib.h>


//Given a file path and file name, loads a texture into memory and returns an identifier for that 
// texture.
unsigned int get_texture (std::string path);

//Creates a generic shape given vertex data
void set_up_shape(Shape* shape, void* data, int num_values,int num_vertex_vals, int data_size);

//Creates a basic triangle (xy plane) and defines the data members of the Shape object referenced by the first 
// argument. 
void set_basic_triangle(Shape* tri, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, bool flip_normal = false, bool outline=false);

//Creates a basic triangle (xy plane) and defines the data members of the Shape object referenced by the first 
// argument. 
void set_texture_triangle (Shape* tri, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, bool flip_normal = false, bool outline=false, float text_size = 1.0f);

//Creates a basic circle (xy plane) and defines data members of the Shape object referenced by the first argument.
void set_basic_circle (Shape* circle, glm::vec3 location, float radius, bool flip_normal = false, bool outline=false, int points=10);

//Creates a basic circle (xy plane) and defines data members of the Shape object referenced by the first argument.
void set_texture_circle (Shape* circle, glm::vec3 location, float radius, bool flip_normal = false, bool outline=false, int points=10);

//Creates a basic rectangle (xy plane) and defines the members of the Shape object referenced by the first argument.
void set_basic_rectangle (Shape* rect, glm::vec3 lower_left, float width, float height, bool flip_normal = false, bool outline=false);

//Creates a basic rectangle (xy plane) and defines the members of the Shape object referenced by the first argument.
void set_texture_rectangle (Shape* rect, glm::vec3 lower_left, float width, float height, bool flip_normal = false, bool outline=false, float text_size=1.0f);

//Creates a basic hull (xy plane) for a ship and defines the members of the Shape object referenced by the first argument.
void set_basic_hull(Shape* hull,glm::vec3 lower_left, float width, float height, bool flip_normal = false, bool outline=false);

//Creates a texture hull (xy plane) for a ship and defines the members of the Shape object referenced by the first argument.
void set_texture_hull(Shape* hull,glm::vec3 lower_left, float width, float height, bool flip_normal = false, bool outline=false);

//Creates a random starfield in the xy plane (-1,-1) to (1,1)
void set_basic_starfield (Shape* stars, int num_stars);

//Creates a 1x1x1 cube
void set_basic_cube (Shape* cube, bool flip_normal = false, bool outline=false);

//Process skybox images
unsigned int get_cube_map(std::vector<std::string> faces, bool cube_map_flag);

#endif //BUILD_SHAPES_HPP