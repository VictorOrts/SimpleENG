#version 330 core
layout (location = 0) in vec3 vertex_position; 
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 vNormal;

uniform mat4 projectionview;
uniform mat4 model;
out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
void main(){
	vec4 worldPos = model * vec4(vertex_position, 1.0);
    FragPos = worldPos.xyz; 
	mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * vNormal;
	TexCoords = uv;
	gl_Position =  projectionview  * worldPos;
}