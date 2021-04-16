#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int post_process_selection;
uniform float time;

const float offset = 1.0 / 300.0;

//Effects:
void normalDisplay();
void nightVision();
void inverseColor();
void grayscale();
void kernelEffects(int effect_id);

float random(vec2 st); //used for nightvision noise

void main()
{ 
    if (post_process_selection == 1) normalDisplay();
    else if (post_process_selection == 2) nightVision();
    else if (post_process_selection == 3) grayscale();
    else if (post_process_selection == 4) inverseColor();
    else if (post_process_selection == 5) kernelEffects(post_process_selection); //sharpen
    else if (post_process_selection == 6) kernelEffects(post_process_selection); //blur
    else if (post_process_selection == 7) kernelEffects(post_process_selection); //edge detection
}

void normalDisplay() {
    FragColor = texture(screenTexture, TexCoords);
}

//Generate random noise for nightvision
float random (vec2 st) {
    return fract(sin(dot(st.xy,vec2(12.9898,78.233)))*43758.5453123);
}
void nightVision() {
    grayscale();
    vec2 st = vec2(time*FragColor.g,time*TexCoords.x*TexCoords.y);
    float rnd = random(st);
    FragColor = vec4(rnd*vec3(0.0,FragColor.g,0.0),1.0);
}

void inverseColor() {
    FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
}

void grayscale() {
    FragColor = texture(screenTexture, TexCoords);
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    FragColor = vec4(average, average, average, 1.0);
}

void kernelEffects(int effect_id) {
    vec2 offsets[9] = vec2[](
    vec2(-offset,  offset), // top-left
    vec2( 0.0,    offset), // top-center
    vec2( offset,  offset), // top-right
    vec2(-offset,  0.0),   // center-left
    vec2( 0.0,    0.0),   // center-center
    vec2( offset,  0.0),   // center-right
    vec2(-offset, -offset), // bottom-left
    vec2( 0.0,   -offset), // bottom-center
    vec2( offset, -offset)  // bottom-right    
    );

    //Sharpen
    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
    //Blur
    if (effect_id == 6) {
        kernel = float[](
            1.0 / 16, 2.0 / 16, 1.0 / 16,
            2.0 / 16, 4.0 / 16, 2.0 / 16,
            1.0 / 16, 2.0 / 16, 1.0 / 16  
        );
    }
    //Edge Detection
    else if (effect_id == 7) {
        kernel = float[](
            1, 1, 1,
            1, -8, 1,
            1, 1, 1
        );
    }

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
    FragColor = vec4(col, 1.0);
}