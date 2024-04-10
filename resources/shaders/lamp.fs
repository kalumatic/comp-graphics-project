#version 330 core
out vec4 FragColor;

struct DirLight {
    vec3 direction;

    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
};

struct Material {
    sampler2D texture_diffuse1;

    float shininess;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform DirLight dirLight;
uniform Material material;

uniform vec3 viewPos;


void main()
{
        // ambient
        vec3 ambient = dirLight.ambient * texture(material.texture_diffuse1, TexCoords).rgb;

        // diffuse
        vec3 norm = normalize(Normal);
        // vec3 lightDir = normalize(light.position - FragPos);
        vec3 lightDir = normalize(-dirLight.direction);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = dirLight.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;

        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
//         vec3 reflectDir = reflect(-lightDir, norm);
        // Blinn-Phong
        vec3 halfwayDir = normalize(lightDir + viewDir);
//         float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        float spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
        vec3 specular = dirLight.specular * spec * vec3(0.0);

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
}
