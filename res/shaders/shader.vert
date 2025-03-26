#version 460

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;


struct ObjectData{
	vec4 positionAndScale;
    vec4 rotationAndColor;
};

layout(std140,set = 0, binding = 0) readonly buffer ObjectBuffer{

	ObjectData objects[];
} objectBuffer;


mat3 calculateTRS(vec2 translation, float rotation, vec2 scale);

void main() {
    ObjectData data = objectBuffer.objects[gl_BaseInstance];
    mat3 model = calculateTRS(data.positionAndScale.xy, data.rotationAndColor.x, data.positionAndScale.zw);
    //mat3 model = calculateTRS(vec2(0), data.rotationAndColor.x, data.positionAndScale.zw);

    gl_Position = vec4(model * vec3(inPosition, 1.0), 1.0);
    //gl_Position = vec4(model * vec3(inPosition + data.positionAndScale.xy, 0.0), 1.0);
    fragColor = objectBuffer.objects[gl_BaseInstance].rotationAndColor.yzw;
    //fragColor = vec3(1);
}


mat3 calculateTRS(vec2 translation, float rotation, vec2 scale)
{
    float c = cos(rotation);
    float s = sin(rotation);

    mat3 model = mat3(
         c * scale.x,  s * scale.x,    0.0,
        -s * scale.y,  c * scale.y,    0.0,
         translation.x, translation.y, 1.0
    );
    return model;
}