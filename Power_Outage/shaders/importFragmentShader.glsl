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
in vec4 FragPosLightSpace;

Material material;
uniform PointLight point_light;
uniform SpotLight spot_light;
uniform DirLight dir_light;
uniform vec4 view_position;
uniform bool use_texture;
uniform sampler2D texture_image;
uniform sampler2D depth_image;

vec3 calc_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calc_spot_light(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calc_dir_light(DirLight light, vec3 normal, vec3 viewDir);
float calc_shadow(vec4 fragPosLightSpace,float bias);

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

  //Shadow calculation
  float shadow = calc_shadow(FragPosLightSpace,0.005);
  vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));

  return lighting;
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

float calc_shadow(vec4 fragPosLightSpace,float bias) {
  // perform perspective divide
  vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
  // transform to [0,1] range
  projCoords = projCoords * 0.5 + 0.5;
  // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
  float closestDepth = texture(depth_image, projCoords.xy).r; 
  // get depth of current fragment from light's perspective
  float currentDepth = projCoords.z;
  // check whether current frag pos is in shadow
  //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

  // PCF
  float shadow = 0.0;
  vec2 texelSize = 1.0 / textureSize(depth_image, 0);
  for(int x = -1; x <= 1; ++x)
  {
      for(int y = -1; y <= 1; ++y)
      {
          float pcfDepth = texture(depth_image, projCoords.xy + vec2(x, y) * texelSize).r; 
          shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
      }    
  }
  shadow /= 9.0;
  
  // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
  if(projCoords.z > 1.0)
      shadow = 0.0;

  return shadow;
}