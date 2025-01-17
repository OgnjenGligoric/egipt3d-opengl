#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D sprite;
uniform vec3 spriteColor;
uniform float alpha = 1.0f;
uniform float threshold = 0.0f;
uniform vec3 highlightColor = vec3(1.0f, 0.0f, 0.0f);  
uniform vec3 lightColor;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    vec4 textureColor = texture(sprite, TexCoords);

    if (TexCoords.x < threshold) {  
        color = vec4(highlightColor, alpha) * vec4(textureColor.rgb * ambient, textureColor.a);
    } else {
        color = vec4(spriteColor, alpha) * vec4(textureColor.rgb * ambient, textureColor.a);
    }
}
