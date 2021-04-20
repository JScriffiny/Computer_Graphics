#ifndef TEXT_DISPLAY_HPP
#define TEXT_DISPLAY_HPP

#include <glad/glad.h> //GLAD must be BEFORE GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.hpp"
#include "shape.hpp"
#include "Font.hpp"

struct Display_Data {
  Shader * fill_program;
  Shader * font_program;
  glm::mat4 view;
  glm::mat4 projection;
  Font * font;
};

class Text_Display {
  private:
    float alpha_value = 0.0f;
    Display_Data data;
  public:
    Text_Display(Display_Data data);
    float get_alpha_value();
    void process_input(GLFWwindow* win);
    void render_player_coordinates(glm::vec3 camPos);
    void render_fire();
    void render_effects_list(int effect_id);
    Shape rect_player_coordinates;
    Shape rect_fire;
    Shape rect_effects_list;
    bool fire_flag = true;
    bool effects_list_flag = true;
};

#endif //TEXT_DISPLAY_HPP
