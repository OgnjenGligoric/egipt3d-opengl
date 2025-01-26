#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 LightingColor;  // Pass lighting result to fragment shader
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D texture_normal1; // Normal map
uniform sampler2D texture_specular1; // Specular intensity map

void main()
{
    vec3 FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
    
    // Use normal map for normals (Gouraud-style approximation)
    vec3 TangentNormal = texture(texture_normal1, TexCoords).rgb * 2.0 - 1.0; // Convert from [0,1] to [-1,1]
    vec3 Normal = normalize(mat3(transpose(inverse(model))) * (TangentNormal));
    
    // Ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse lighting
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular lighting with texture
    float specularStrength = texture(texture_specular1, TexCoords).r; // Sample specular strength
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;
    
    // Combine lighting components
    LightingColor = ambient + diffuse + specular;

    // Final position
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
