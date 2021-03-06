#ifndef MOVING_PLATE_HPP
#define MOVING_PLATE_HPP

#include <glad/glad.h> //GLAD must be BEFORE GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include "shape.hpp"

class MovingPlate: public Shape {
    protected:
        //rotation about the y-axis
        float rotation;
        //initial orientation (ensures we start at right point)
        float orientation;
        //scale vector
        glm::vec3 scale_vec;
        //position vector
        glm::vec3 position;
        //original position vector
        glm::vec3 original_position;
        //boolean: if plate is pressed
        bool is_pressed;
        //boolean: plate's static status (default is off)
        bool status_flag;
        //plate texture
        unsigned int texture;
        //shader program
        Shader* shader_program;
    public:
        MovingPlate(Shape_Struct s, glm::vec3 scale, glm::vec3 pos, float orient);
        void process_input(GLFWwindow *win, glm::vec3 camera_pos);
        void draw(Shader *optional_shader);
        bool get_plate_status();
        glm::vec3 get_position();
        void set_texture(unsigned int texture);
        void set_shader(Shader* shader_program);
        void set_scale(glm::vec3 scale_vec);
        double range = 1.2;
        Shader* original_shader;
};

#endif //MOVING_PLATE_HPP