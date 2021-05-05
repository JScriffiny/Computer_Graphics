#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 6.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera {
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 right;
  glm::vec3 world_up;
  glm::vec3 up;
  float yaw;
  float pitch;
  float move_speed;
  float sensitivity;
  float zoom;
    
  protected:
  void update_camera_vectors();

  public: 
  Camera(glm::vec3 position, glm::vec3 up, float yaw = YAW,float pitch=PITCH);

  glm::vec3 get_position();
  glm::vec3 get_front();
  float get_pitch();
  float get_yaw();
  void set_position(glm::vec3 newPos);

  glm::mat4 get_view_matrix ();

  void process_keyboard (Camera_Movement direction, float deltaTime);

  void process_mouse_movement (float xoffset, float yoffset, bool constrainPitch = true);
};



#endif //CAMERA_HPP