uniform vec3 eye_pos;
uniform vec3 light_pos;
uniform sampler2D texture;
uniform sampler2D normal_texture;

varying vec3 coord;

void main() {
    vec3 normal = texture2D(normal_texture, gl_TexCoord[0].st).rgb;
    vec3 color = texture2D(texture, gl_TexCoord[0].st).rgb;
    normal = normal * 2.0 - 1.0;
    vec3 ambient = 0.1 * color;
    vec3 light_direction = normalize(light_pos - coord);
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = diff * color;
    vec3 view_direction = normalize(eye_pos - coord);
    vec3 reflect_direction = reflect(-light_direction, normal);
    vec3 halfway_direction = normalize(light_direction + view_direction);
    float spec = pow(max(dot(normal, halfway_direction), 0.0), 32.0);
    vec3 specular = vec3(0.2) * spec;
    gl_FragColor = vec4(ambient + specular + diffuse, 1.0);
}
