#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <glad/glad.h> //GLAD must be BEFORE GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include "vertex_attr.hpp"
#include "Shader.hpp"

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

struct Shape_Struct {
   unsigned int VBO;
   unsigned int VAO;
   bool clear_objs;
  unsigned int EBO;
  int num_indices;
  int num_of_vertices;
  GLuint primitive;
  Material material;
};

//A class containing VBO, VAO, and EBO information 
//for drawing shapes.
class Shape {
    protected:
        //VBO id
        unsigned int VBO;
           //VAO id
        unsigned int VAO;
       
        //EBO id (if used)
        unsigned int EBO;
        //Number of vertices in the VBO
        int num_of_vertices;
        //Number of indices in the EBO
        int num_indices;
        //A boolean variable that is true if the 
        //buffers must be cleared when the destructor is invoked.
        bool clear_objs;
        //The primitive used to draw the shape (GL_TRIANGLES is the default)
        GLuint primitive;

        //Material for the shape
        Material material;
    
    public:
  
        //A Shape constructor that sets all values to their default.
        Shape();
        //A copy constructor that creates a new shape from an old one, but -importantly-
        //does NOT create a new VBO, VAO, or EBO.
        Shape(Shape &obj);

        //A constructor that creates a new shape from a structure.
        Shape(Shape_Struct);

        //Initializes the VBO and VAO with provided data, attribute pointers, and primitive.
        void initialize (float* data, int data_bytes, int num_vertices, 
                         std::vector<Vertex_Attribute> vao, 
                         GLuint prim = GL_TRIANGLES);

        //Optionally sets up an EBO (assumes that the EBO is used for creating an outline).
        void set_EBO (unsigned int* data, int num_indices);

        //Draws the shape using a given shader program.  Optionally draws an outline if the 
        //EBO has been set up.
        void draw (unsigned int shader_program,unsigned int outline_program=0);

        //Given a material structure (with ambient, diffuse, specular, and shininess values), set the 
        // material data member for the class
        void set_material(Material m);

        //Given a shader program, sets the values of the material uniform in that shader to match the materials
        //in the given shape.
        void use_material (Shader* s);

        //Destructor (deletes the buffers and vertex array object if this shape created them).
        ~Shape();
};


#endif //SHAPE_HPP