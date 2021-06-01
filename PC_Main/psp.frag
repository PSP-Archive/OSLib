uniform sampler2D tex;  

void main (void) {
  vec4 value;
  value = texture2D(tex, vec2(gl_TexCoord[0]));
  if (value[0] == 0)
    discard;
  gl_FragColor = value;
}

