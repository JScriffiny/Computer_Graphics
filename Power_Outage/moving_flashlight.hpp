#ifndef MOVING_FLASHLIGHT_HPP
#define MOVING_FLASHLIGHT_HPP

#include <glad/glad.h> //GLAD must be BEFORE GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include "shape.hpp"

class MovingFlashlight: public Shape {
    protected:
        //rotation about the y-axis
        float rotation;
        //initial orientation (ensures we start at right point)
        float orientation;
        //scale vector
        glm::vec3 scale_vec;
        //position vector
        glm::vec3 position;
        //plate texture
        unsigned int texture;
        //shader program
        Shader* shader_program;
    public:
        MovingFlashlight(Shape_Struct s, glm::vec3 scale, glm::vec3 pos, float orient,float yaw,float pitch);
        void process_input(GLFWwindow *win);
        void draw(glm::vec3 camPos, float yaw_change, float pitch_change);
        glm::vec3 get_position();
        void set_position(glm::vec3 position);
        void set_texture(unsigned int texture);
        void set_shader(Shader* shader_program);
        void set_scale(glm::vec3 scale_vec);
        float yaw = 0.0f;
        float pitch = 0.0f;
};

#endif //MOVING_FLASHLIGHT_HPP