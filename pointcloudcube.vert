attribute highp vec3 vertex;
uniform mediump mat4 matrix;
varying mediump vec4 color;
vec3 hsv(float h, float s, float v) {
    vec4 t = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(vec3(h) + t.xyz) * 6.0 - vec3(t.w));
    return v * mix(vec3(t.x), clamp(p - vec3(t.x), 0.0, 1.0), s);
}
void main()
{
    float h = (vertex.y + 5.0) / 10.0;
    color = vec4(hsv(h, 0.8, 0.8), 1.0);
    gl_Position = matrix * vec4(vertex, 1.0);
}
