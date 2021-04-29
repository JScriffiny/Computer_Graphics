#ifndef MOVING_KEY_HPP
#define MOVING_KEY_HPP

#include <glad/glad.h> //GLAD must be BEFORE GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include "shape.hpp"

class MovingKey: public Shape {
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
        MovingKey(Shape_Struct s, glm::vec3 scale, glm::vec3 pos, float orient);
        void process_input(GLFWwindow *win, glm::vec3 camera_pos);
        void draw(Shader *optional_shader);
        glm::vec3 get_position();
        void set_texture(unsigned int texture);
        void set_shader(Shader* shader_program);
        void set_scale(glm::vec3 scale_vec);
        float key_range = 2.5f;
        float keyhole_range = 4.0f;
        bool first_collect = false;
        bool collected = false;
        bool inserted = false;
        bool collect_flag = true;
        bool insert_flag = true;
        Shader* original_shader;
};

#endif //MOVING_KEY_HPP