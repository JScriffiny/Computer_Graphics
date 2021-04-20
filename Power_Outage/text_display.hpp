#ifndef TEXT_DISPLAY_HPP
#define TEXT_DISPLAY_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "world_state.hpp"
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
    Text_Display(float alpha_value,Display_Data data);
    float get_alpha_value();
    void render_player_coordinates(glm::vec3 camPos);
    Shape rect_player_coordinates;
};

#endif //TEXT_DISPLAY_HPP
