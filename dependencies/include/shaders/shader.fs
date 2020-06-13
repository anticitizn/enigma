#version 460 core
out vec4 FragColor;
in vec3 normal;
in vec3 fragPos;
in vec3 lightPos;

void main()
{
	vec3 objectColor = vec3(1.0f, 0.3f, 0.3f);
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	// ambient
	float ambientStrength = 0.2f;
	vec3 ambient = lightColor * ambientStrength;

	// diffuse
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = diff * lightColor;

	// specular
	float specularStrength = 0.5f;
	vec3 viewDir = normalize(-fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diff + specular) * objectColor;
	FragColor = vec4(result, 1.0f);
}