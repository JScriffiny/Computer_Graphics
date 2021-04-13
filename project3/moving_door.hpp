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
        //original  position vector
        glm::vec3 original_position;
        //door status
        bool is_open;

    public:
        //Constructor
        MovingDoor(Shape_Struct s, glm::vec3 scale, glm::vec3 pos, float orient);
        //process input
        void process_input(GLFWwindow *win, glm::vec3 camera_pos);
        //render object
        void draw(Shader *shader_program, unsigned int texture);
        //get door status
        bool get_door_status();
};

#endif //MOVING_DOOR_HPP