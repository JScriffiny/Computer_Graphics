#ifndef WORLD_STATE_HPP
#define WORLD_STATE_HPP

#include "camera.hpp"
#include "shape.hpp"
#include "Shader.hpp"
#include "moving_door.hpp"
#include "moving_plate.hpp"
#include "post_processor.hpp"

struct Draw_Data {
  Shape* shape;
  Shader* shader;
  unsigned int texture = -1;
};

class World {
  public:
    //Create the world state using provided window dimensions.
    World(int width, int height);
    void process_input(GLFWwindow* win);
    void render_scene (std::map<std::string, Draw_Data> objects,Shader *optional_shader = NULL);
    void check_collision(glm::vec3 previous_pos);
    void render_headsUp_display(Shader * fill_program, Shader font_program, Shape heads_up,
                                glm::mat4 view,glm::mat4 projection,float alpha_value,Font font);
    void render_skybox(Shader * shader, Shape shape, unsigned int texture);
    
    //These are left public for convenience (but depending upon the complexity of your world state, it would 
    //make sense to keep some or all of the data members protected)
    glm::vec4 clear_color = glm::vec4(0.0f,0.0f,0.0f,1.0f);
    bool rgba_array[4] = {false,false,false,false};
    unsigned int floor_texture = 0;

    //Point Light
    bool point_light_on = true;
    glm::vec4 point_light_position = glm::vec4(5.15f,-1.3f,3.75f,1.0f);
    glm::vec3 point_light_color = glm::vec3(1.0f,1.0f,1.0f);

    //Spot Light
    bool spot_light_on_flag = false;
    bool spot_light_redLens_flag = false;
    bool spot_light_redLens = false;
    bool spot_light_on = true;
    glm::vec4 spot_light_position = glm::vec4(0.1f, 10.0f, 0.1f, 1.0f); //prevent divide by 0
    glm::vec3 spot_light_ambient = glm::vec3(0.1f,0.1f,0.1f);
    glm::vec3 spot_light_diffuse = glm::vec3(0.8f,0.8f,0.8f);
    glm::vec3 spot_light_specular = glm::vec3(1.0f,1.0f,1.0f);

    //Directional Light
    bool dir_light_key_pressed = false;
    bool dir_light_on = false;
    glm::vec3 dir_light_direction = glm::vec3(0.0,-1.0,0.0);
    glm::vec3 dir_light_color = glm::vec3(0.4f,0.4f,0.4f);

    //Singular key press booleans
    bool print_flag = true;
    bool spawn_pressed = false;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    float rotation_y = 0.0f;
    float rotation_z = 0.0f;

    int height = 0;
    int width = 0;

    //Mouse settings
    bool first_mouse = true;
    float lastX = height/2.0f;
    float lastY = width/2.0f;

    //Camera
    bool cameraView_key_pressed = false;
    bool bird_cam_on = false;
    glm::vec3 bird_cam_pos = glm::vec3(-0.5f,18.0f,1.0f);
    glm::vec3 saved_player_pos = glm::vec3(10.0f,-3.0f,-3.0f);
    Camera* camera;

    //Post Processor
    Post_Processor* post_processor;

    //Pressure Plate
    MovingPlate* pressure_plate;

    //Door
    MovingDoor* door;
};

#endif //WORLD_STATE_HPP
