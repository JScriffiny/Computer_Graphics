#version 330 core
struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct PointLight {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float constant;
  float linear;
  float quadratic;
  bool on;
};

struct SpotLight {
  vec3 position;  
  vec3 direction;
  float cutOff;
  float outerCutOff;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float constant;
  float linear;
  float quadratic;
  bool on;
};

struct DirLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  bool on;
};

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec3 fColor;
in vec3 sColor;
in vec2 TexCoord;

Material material;
uniform PointLight point_light;
uniform SpotLight spot_light;
uniform DirLight dir_light;
uniform vec4 view_position;
uniform bool use_texture;
uniform sampler2D texture_image;

vec3 calc_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calc_spot_light(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calc_dir_light(DirLight light, vec3 normal, vec3 viewDir);

void main()
{
  material.ambient = fColor;
  material.diffuse = fColor;
  material.specular = sColor;
  vec3 norm = normalize(Normal);

  vec3 viewDir = normalize(view_position.xyz - FragPos);
  vec3 result = vec3(0,0,0);
  result += calc_point_light(point_light,norm,FragPos,viewDir);
  result += calc_spot_light(spot_light,norm,FragPos,viewDir);
  result += calc_dir_light(dir_light,norm,viewDir);

  FragColor = vec4(result,1.0);
}


vec3 calc_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  if (!light.on) {
    return vec3(0.0,0.0,0.0);
  }
  vec3 lightDir = normalize(light.position-fragPos);

  //diffuse shading
  float diff = max(dot(normal,lightDir),0.0);

  //specular shading
  vec3 reflectDir = reflect(-lightDir,normal);
  float spec = pow(max(dot(viewDir,reflectDir),0.0),256);

  //attenuation
  float distance    = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			                    light.quadratic * (distance * distance)); 

  vec3 ambient, diffuse, specular;
  
  if (use_texture) {
    ambient = light.ambient*vec3(texture(texture_image,TexCoord));
    diffuse = diff*light.diffuse*vec3(texture(texture_image,TexCoord));
    specular = spec*light.specular*vec3(texture(texture_image,TexCoord));
  }
  else {
    ambient = material.ambient.xyz*light.ambient;
    diffuse = (diff*material.diffuse.xyz)*light.diffuse;
    specular = (spec*material.specular.xyz)*light.specular;
  }

  ambient  *= attenuation;
  diffuse  *= attenuation;
  specular *= attenuation;

  return (ambient + diffuse + specular);
}

vec3 calc_spot_light(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  if (!light.on) {
    return vec3(0.0,0.0,0.0);
  }

  vec3 lightDir = normalize(light.position-fragPos);
  
  // diffuse
  float diff = max(dot(normal, lightDir), 0.0);

  // specular
  vec3 reflectDir = reflect(-lightDir, normal);  
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  
  //attenuation
  float distance    = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			                    light.quadratic * (distance * distance)); 
  vec3 ambient, diffuse, specular;
  
  if (use_texture) {
    ambient = light.ambient*vec3(texture(texture_image,TexCoord));
    diffuse = diff*light.diffuse*vec3(texture(texture_image,TexCoord));
    specular = spec*light.specular*vec3(texture(texture_image,TexCoord));
  }
  else {
    ambient = material.ambient.xyz*light.ambient;
    diffuse = (diff*material.diffuse.xyz)*light.diffuse;
    specular = (spec*material.specular.xyz)*light.specular;
  }
  ambient  *= attenuation;
  diffuse  *= attenuation;
  specular *= attenuation;

  // spotlight (soft edges)
  float theta = dot(lightDir, normalize(-light.direction)); 
  float epsilon = (light.cutOff - light.outerCutOff);
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
  diffuse  *= intensity;
  specular *= intensity;

  return (ambient + diffuse + specular);
}

vec3 calc_dir_light(DirLight light, vec3 normal, vec3 viewDir) {
  if (!light.on) {
    return vec3(0.0,0.0,0.0);
  }

  vec3 lightDir = normalize(-light.direction);

  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);

  // specular shading
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  // combine results
  vec3 ambient, diffuse, specular;
  if (use_texture) {
    ambient = light.ambient*vec3(texture(texture_image,TexCoord));
    diffuse = diff*light.diffuse*vec3(texture(texture_image,TexCoord));
    specular = spec*light.specular*vec3(texture(texture_image,TexCoord));
  }
  else {
    ambient = material.ambient.xyz*light.ambient;
    diffuse = (diff*material.diffuse.xyz)*light.diffuse;
    specular = (spec*material.specular.xyz)*light.specular;
  }

  return (ambient + diffuse + specular);
}