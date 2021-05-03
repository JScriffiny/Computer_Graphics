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
    void initialize();
    float get_alpha_value();
    void process_input(GLFWwindow* win);
    void render_player_coordinates(glm::vec3 camPos);
    void render_effects_list(int effect_id);
    void render_key_status(bool key_collected);

    Shape rect_player_coordinates;
    Shape rect_effects_list;
    Shape rect_selected_1;
    Shape rect_selected_2;
    Shape rect_selected_3;
    Shape rect_selected_4;
    Shape rect_selected_5;
    Shape rect_selected_6;
    Shape rect_selected_7;
    std::vector<Shape*> rect_selects;
    std::vector<std::string> effects;

    Shape rect_key_status;
    bool effects_list_flag = true;
    bool effects_list_activated = false;
};

#endif //TEXT_DISPLAY_HPP
