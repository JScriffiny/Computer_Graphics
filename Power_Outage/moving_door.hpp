#ifndef MOVING_DOOR_HPP
#define MOVING_DOOR_HPP

#include <glad/glad.h> //GLAD must be BEFORE GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include "shape.hpp"

class MovingDoor: public Shape {
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
        //door status
        bool is_open;
        //door texture
        unsigned int texture;
        //shader program
        Shader* shader_program;
    public:
        MovingDoor(Shape_Struct s, glm::vec3 scale, glm::vec3 pos, float orient);
        void process_input(GLFWwindow *win, glm::vec3 camera_pos);
        void draw();
        bool get_door_status();
        glm::vec3 get_position();
        void set_texture(unsigned int texture);
        void set_shader(Shader* shader_program);
        void set_scale(glm::vec3 scale_vec);
};

#endif //MOVING_DOOR_HPP