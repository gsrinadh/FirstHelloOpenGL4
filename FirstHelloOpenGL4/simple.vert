#version 430

in vec3 vp;
void main () {
  gl_Position = vec4 (vp.x/2.0, vp.y/2.0, vp.z, 1.0);
}
