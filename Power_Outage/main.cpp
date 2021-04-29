#include "environment_setup.hpp"
#include "shape.hpp"
#include "Shader.hpp"
#include "build_shapes.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"
#include "Font.hpp"
#include "import_object.hpp"
#include <map>
#include "world_state.hpp"
#include "post_processor.hpp"
#include "text_display.hpp"
#include "skybox.hpp"
#include "moving_door.hpp"
#include "moving_plate.hpp"
#include "moving_key.hpp"

//Constants
#define WIN_WIDTH 960
#define WIN_HEIGHT 720
#define FPS 60.0

//Create the world state object
World world(WIN_WIDTH,WIN_HEIGHT);

//Create post processor object
Post_Processor post_processor(1,true,false);

//Create font object
Font arialFont("fonts/ArialBlackLarge.bmp","fonts/ArialBlack.csv",0.3,0.4);

//Materials
Material pearl{glm::vec3(0.25,0.20725,0.20725),
                 glm::vec3(1, 0.829, 0.829),
                 glm::vec3(0.296648,0.296648,0.296648),
                 0.088*128};

Material silver{glm::vec3(0.19225,0.19225,0.19225),
                 glm::vec3(0.50754, 0.50754, 0.50754),
                 glm::vec3(0.508273,0.508273,0.508273),
                 0.4*128};

//Create camera object
Camera camera(glm::vec3(10.0f,-3.0f,-3.0f),glm::vec3(0.0f,1.0f,0.0f),115.0f, 0.0f);

//Function Prototypes
void mouse_callback (GLFWwindow* win, double xpos, double ypos);
void enforceFrameRate(double last_frame_time, double frame_rate); //fights rendering lag

int main() {
  //Initialize the environment
  GLFWwindow* window = initialize_environment(WIN_WIDTH,WIN_HEIGHT,"Power Outage");
  if (window == NULL) {
    return -1; //error msg already printed to screen.
  }

  //Initialize world camera
  world.camera = &camera;

  //Initialize post processor
  world.post_processor = &post_processor;

  //The font must be initialized -after- the environment.
  arialFont.initialize();

  //Import objects
  ImportOBJ new_importer;

  /** Office Scene setup **/
  //Office Floor
  Shape_Struct new_officeFloor = new_importer.loadFiles("models/floor");
  Shape officeFloor(new_officeFloor);
  unsigned int officeFloor_texture = new_importer.getTexture();
  //Office Walls
  Shape_Struct new_walls = new_importer.loadFiles("models/walls");
  Shape walls(new_walls);
  unsigned int walls_texture = new_importer.getTexture();
  //Office Furniture
  Shape_Struct new_furniture = new_importer.loadFiles("models/furniture");
  Shape furniture(new_furniture);
  unsigned int furniture_texture = new_importer.getTexture();

  //Keyhole
  Shape_Struct new_keyhole = new_importer.loadFiles("models/keyhole");
  Shape keyhole(new_keyhole);
  unsigned int keyhole_texture = new_importer.getTexture();

  //Material Cubes
  Shape cube1,cube2;
  set_basic_cube(&cube1);
  cube1.set_material(silver);
  set_basic_cube(&cube2);
  cube2.set_material(pearl);

  //Pressure Plate
  MovingPlate pressure_plate(new_importer.loadFiles("models/pressurePlate"),
                            glm::vec3(0.5,0.5,0.5),glm::vec3(1.2,-3.99,-0.8),0.0f);
  pressure_plate.set_texture(new_importer.getTexture());
  
  //Door
  MovingDoor door(new_importer.loadFiles("models/door"),
                  glm::vec3(0.5,0.5,0.5),glm::vec3(5.0,-3.99,3.75),0.0f);
  door.set_texture(new_importer.getTexture());

  //Key
  MovingKey office_key(new_importer.loadFiles("models/key"),
                  glm::vec3(0.25,0.25,0.25),glm::vec3(11.0,-3.99,1.0),0.0f);
  office_key.set_texture(new_importer.getTexture());
  
  //Brick floor
  Shape worldFloor;
  world.floor_texture = get_texture("images/bricks.jpg");
  set_texture_rectangle(&worldFloor,glm::vec3(-1.0,-1.0,0.0f),2.0f,2.0f,false,false,50.0f);
  
  //Initialize shader programs
  Shader fill_program("shaders/vertexShader.glsl","shaders/fragmentShader.glsl");
  Shader texture_program("shaders/textureVertexShader.glsl","shaders/textureFragmentShader.glsl");
  Shader outline_program("shaders/vertexShader.glsl","shaders/outlineFragmentShader.glsl");
  Shader font_program ("shaders/fontVertexShader.glsl","shaders/fontFragmentShader.glsl");
  Shader import_program("shaders/importVertexShader.glsl","shaders/importFragmentShader.glsl");
  Shader depth_program("shaders/depthVertexShader.glsl","shaders/depthFragmentShader.glsl");
  Shader skybox_program("shaders/skyboxVertexShader.glsl","shaders/skyboxFragmentShader.glsl");
  Shader post_process_program("shaders/postVertexShader.glsl","shaders/postFragmentShader.glsl");

  //Map structure setup to pass objects to render scene function
  std::map<std::string,Draw_Data> draw_map;
  //Add floor to map
  draw_map["worldFloor"].shape = &worldFloor;
  draw_map["worldFloor"].shader = &texture_program;
  //Add officeFloor to map
  draw_map["officeFloor"].shape = &officeFloor;
  draw_map["officeFloor"].shader = &import_program;
  draw_map["officeFloor"].texture = officeFloor_texture;
  //Add walls to map
  draw_map["walls"].shape = &walls;
  draw_map["walls"].shader = &import_program;
  draw_map["walls"].texture = walls_texture;
  //Add furniture to map
  draw_map["furniture"].shape = &furniture;
  draw_map["furniture"].shader = &import_program;
  draw_map["furniture"].texture = furniture_texture;
  //Add keyhole to map
  draw_map["keyhole"].shape = &keyhole;
  draw_map["keyhole"].shader = &import_program;
  draw_map["keyhole"].texture = keyhole_texture;
  //Add cubes to map
  draw_map["cube1"].shape = &cube1;
  draw_map["cube1"].shader = &fill_program;
  draw_map["cube2"].shape = &cube2;
  draw_map["cube2"].shader = &fill_program;
  //Add shaders for stencil program to reference
  draw_map["stencil_fill"].shader = &fill_program;
  draw_map["stencil_import"].shader = &import_program;

  //Set shaders for moving objects
  pressure_plate.set_shader(&import_program);
  door.set_shader(&import_program);
  office_key.set_shader(&import_program);
  //Initialize world plate and door
  world.pressure_plate = &pressure_plate;
  world.door = &door;
  world.office_key = &office_key;
  
  //Shader initialization
  std::vector<Shader*> shaders = {&fill_program,&outline_program,&texture_program,
                                  &import_program,&depth_program,&skybox_program,&post_process_program};
  glm::mat4 identity(1.0f);
  glm::mat4 model = identity;
  glm::mat4 view = identity;
  view = camera.get_view_matrix();
  glm::mat4 projection = glm::perspective(glm::radians(45.0f),(float)WIN_WIDTH/(float)WIN_HEIGHT,0.1f,100.0f);
  for (int i = 0; i < shaders.size(); i++) {
    shaders[i]->use();
    shaders[i]->setMat4("transform",identity);
    shaders[i]->setMat4("model",model);
    shaders[i]->setMat4("view",view);
    shaders[i]->setMat4("projection",projection);
    shaders[i]->setFloat("ambient_strength",0.1);
    shaders[i]->setFloat("specular_strength",1.0f);
    shaders[i]->setVec4("view_position",glm::vec4(world.camera->get_position(),0.0f));
    //Point Light
    glm::vec3 wp = world.point_light_position;
    shaders[i]->setVec3("point_light.position",glm::vec3(wp.x,wp.y,wp.z));
    shaders[i]->setVec3("point_light.ambient",0.2f*world.point_light_color);
    shaders[i]->setVec3("point_light.diffuse",world.point_light_color);
    shaders[i]->setVec3("point_light.specular",world.point_light_color);
    shaders[i]->setBool("point_light.on",world.point_light_on);
    shaders[i]->setFloat("point_light.constant",1.0);
    shaders[i]->setFloat("point_light.linear",0.14);
    shaders[i]->setFloat("point_light.quadratic",0.07);
    shaders[i]->setFloat("shininess",256);
    //Spot Light
    shaders[i]->setVec3("spot_light.position",world.camera->get_position());
    shaders[i]->setVec3("spot_light.direction",world.camera->get_front());
    shaders[i]->setFloat("spot_light.cutOff",glm::cos(glm::radians(12.5f)));
    shaders[i]->setFloat("spot_light.outerCutOff",glm::cos(glm::radians(17.5f)));
    shaders[i]->setVec3("spot_light.ambient",world.spot_light_ambient);
    shaders[i]->setVec3("spot_light.diffuse",world.spot_light_diffuse);
    shaders[i]->setVec3("spot_light.specular",world.spot_light_specular);
    shaders[i]->setFloat("spot_light.constant",1.0f);
    shaders[i]->setFloat("spot_light.linear",0.09f);
    shaders[i]->setFloat("spot_light.quadratic",0.032f);
    shaders[i]->setBool("spot_light.on",world.spot_light_on);
    //Directional Light
    shaders[i]->setVec3("dir_light.direction",world.dir_light_direction);
    shaders[i]->setVec3("dir_light.ambient",0.2f*world.dir_light_color);
    shaders[i]->setVec3("dir_light.diffuse",world.dir_light_color);
    shaders[i]->setVec3("dir_light.specular",world.dir_light_color);
    shaders[i]->setBool("dir_light.on",world.dir_light_on);
    shaders[i]->setFloat("time",glfwGetTime());
  }

  //Text Display setup
  Display_Data display_data;
  display_data.fill_program = &fill_program;
  display_data.font_program = &font_program;
  display_data.projection = projection;
  display_data.view = view;
  display_data.font = &arialFont;
  Text_Display text_display(display_data);

  //Skybox setup
  Shape skybox_cube;
  set_basic_cube(&skybox_cube);
  std::vector<std::string> faces {
    "skybox/right.jpg",
    "skybox/left.jpg",
    "skybox/top.jpg",
    "skybox/bottom.jpg",
    "skybox/front.jpg",
    "skybox/back.jpg"
  };
  unsigned int cubemapTexture = get_cube_map(faces,false);
  Skybox skybox(&skybox_program,skybox_cube,cubemapTexture);

  //font_program shader setup
  font_program.use();
  font_program.setMat4("view",glm::mat4(1.0));
  font_program.setMat4("projection", glm::ortho(-5.0, 5.0, -5.0, 5.0, -1.0, 1.0));
  font_program.setVec4("transparentColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
  font_program.setFloat("alpha", text_display.get_alpha_value());
  font_program.setInt("texture1", 0);

  //Cursor setup
  glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window,mouse_callback);

  //Enable depth testing to avoid managing ordering of 3D objects
  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_BACK,GL_LINE);

  //Blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  /** Framebuffer (Post Processing) **/
  //Make framebuffer object
  unsigned int post_framebuffer;
  glGenFramebuffers(1, &post_framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, post_framebuffer); 
  //Create and bind a texture
  unsigned int texColorBuffer;
  glGenTextures(1, &texColorBuffer);
  glBindTexture(GL_TEXTURE_2D, texColorBuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  //Attach texture to framebuffer object
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
  //Make renderbuffer object
  unsigned int rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  //Attach the renderbuffer to framebuffer depth and stencil attachments
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
  //Check if framebuffer is complete
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	  std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /** Framebuffer (Shadows) **/
  //Make framebuffer object
  unsigned int depthMapFBO;
  glGenFramebuffers(1, &depthMapFBO);
  const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
  //Generate 2D texture to hold depth information
  unsigned int depthMap;
  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT,
               0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  //Bind and attach the texture
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  //Reset to default
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //Stencil Testing
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
  glStencilFunc(GL_NOTEQUAL,1,0xFF);
  
  //glfwWindowShouldClose checks if GLFW has been instructed to close
  while(!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    world.deltaTime = currentFrame - world.lastFrame;
    world.lastFrame = currentFrame;

    //Prevent rendering lag
    enforceFrameRate(world.lastFrame,FPS);

    //Set the clear color
    glm::vec4 clr = world.clear_color;
    glClearColor(clr.r,clr.g,clr.b,clr.a);

    //1. Process Input
    world.process_input(window);
    text_display.process_input(window);
    post_processor.process_input(window);

    //2. Render Scene
    //First Pass (Shadows)
    glViewport(0,0,SHADOW_WIDTH,SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    world.render_scene(draw_map,&depth_program);

    //Second Pass
    glViewport(0,0,WIN_WIDTH,WIN_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER,post_framebuffer);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,depthMap);
    world.render_scene(draw_map);
    skybox.render(world.camera->get_view_matrix());
    text_display.render_player_coordinates(world.camera->get_position());
    text_display.render_effects_list(post_processor.get_selection());
    text_display.render_key_status(office_key.collected);
    
    //Third Pass (Render post processing effects last)
    post_processor.render_effect(&post_process_program,texColorBuffer);
    
    //3. Poll for events
    glfwPollEvents();
    
    //4. Swap Buffers
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}

void mouse_callback(GLFWwindow* win, double xpos, double ypos) {
  if (world.first_mouse) {
    world.lastX = xpos;
    world.lastY = ypos;
    world.first_mouse = false;
  }

  float offsetx = xpos-world.lastX;
  float offsety = world.lastY-ypos;

  world.x_offset = offsetx;
  world.y_offset = offsety;

  world.lastX = xpos;
  world.lastY = ypos;

  camera.process_mouse_movement(offsetx,offsety);
}

void enforceFrameRate(double last_frame_time, double frame_rate) {
  while (glfwGetTime() >= last_frame_time+(1.0/frame_rate));
}