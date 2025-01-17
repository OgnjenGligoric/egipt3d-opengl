#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 lightColor;

void main()
{    
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec4 textureColor = texture(texture_diffuse1, TexCoords);

    FragColor = vec4(textureColor.rgb * ambient, textureColor.a);
}