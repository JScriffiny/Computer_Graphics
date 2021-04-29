#include "camera.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch):front(glm::vec3(0.0f, 0.0f, -1.0f)), move_speed(SPEED), sensitivity(SENSITIVITY), zoom(ZOOM) {
    this->position = position;
    this->world_up = up;
    this->yaw = yaw;
    this->pitch = pitch;
    this->update_camera_vectors();
}

glm::mat4 Camera::get_view_matrix() {
    return glm::lookAt(this->position, this->position+this->front,this->world_up);
}

void Camera::process_keyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = this->move_speed * deltaTime;

        if (direction == FORWARD) {
            int cur_y = this->position.y;
            this->position += this->front * velocity;
            this->position.y = cur_y;
        }
        if (direction == BACKWARD) {
            int cur_y = this->position.y;
            this->position -= this->front * velocity;
            this->position.y = cur_y;
        }
        if (direction == LEFT)
            this->position -= this->right * velocity;
        if (direction == RIGHT)
            this->position += this->right * velocity;
    }

void Camera::process_mouse_movement (float xoffset, float yoffset,bool constrainPitch) {
    xoffset *= this->sensitivity;
    yoffset *= this->sensitivity;
        this->yaw  += xoffset;
        this->pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (this->pitch > 89.0f)
                this->pitch = 89.0f;
            if (this->pitch < -89.0f)
                this->pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        this->update_camera_vectors();
}

glm::vec3 Camera::get_position() {
    return this->position;
}
glm::vec3 Camera::get_front() {
    return this->front;
}
float Camera::get_pitch() {
    return this->pitch;
}
float Camera::get_yaw() {
    return this->yaw;
}

void Camera::set_position(glm::vec3 newPos) {
    this->position = newPos;
}

void Camera::update_camera_vectors() {
      //glm::vec3 front;
      this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
      this->front.y = sin(glm::radians(this->pitch));
      this->front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
      this->front = glm::normalize(this->front);
      // also re-calculate the Right and Up vector
      this->right = glm::normalize(glm::cross(this->front, this->world_up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
      this->up    = glm::normalize(glm::cross(this->right, this->front));
}
