#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
    float Radius;
};
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;
vec3 position;
vec3 normal;
vec3 diffuse;
vec3 PointLightCalc(Light nlight);
vec3 DiffuseLightCalc(vec3 lightPos, vec3 lightInt, float attenuation);
void main()
{             
    // retrieve data from gbuffer
    position = texture(gPosition, TexCoord).rgb;
    normal = texture(gNormal, TexCoord).rgb;
    diffuse = texture(gAlbedoSpec, TexCoord).rgb;
    float Specular = texture(gAlbedoSpec, TexCoord).a;

    vec3 ambientL = vec3(0.1f, 0.1f, 0.1f)*diffuse;
	vec3 colourAmbient = ambientL;
    normal = normalize(normal);
    vec3 colour;

    for(int i = 0;i<NR_LIGHTS;i++){
        colour += PointLightCalc(lights[i]);
    }
    colour = colour *diffuse;
    FragColor = vec4(colour,1);
}

vec3 PointLightCalc(Light nlight)
{
    float dist = distance(nlight.Position, position);
    float attenuation = 1 - smoothstep(1.f, nlight.Radius, dist);

    vec3 colour = DiffuseLightCalc(nlight.Position, nlight.Color, attenuation);

    return colour;
}

vec3 DiffuseLightCalc(vec3 lightPos, vec3 lightInt, float attenuation)
{
    vec3 L = normalize(lightPos - position);
    float scaler = max(0, dot(L, normal))*attenuation;

    vec3 diffuse_intensity = lightInt * scaler;

    return diffuse_intensity;
}