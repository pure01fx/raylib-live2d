static const char* VertShaderSrcSetupMask =
"#version 330 core\n"
"attribute vec4 a_position;"
"attribute vec2 a_texCoord;"
"varying vec2 v_texCoord;"
"varying vec4 v_myPos;"
"uniform mat4 u_clipMatrix;"
"void main()"
"{"
"gl_Position = u_clipMatrix * a_position;"
"v_myPos = u_clipMatrix * a_position;"
"v_texCoord = a_texCoord;"
"v_texCoord.y = 1.0 - v_texCoord.y;"
"}";

static const char* FragShaderSrcSetupMask =
"#version 330 core\n"
"varying vec2 v_texCoord;"
"varying vec4 v_myPos;"
"uniform sampler2D s_texture0;"
"uniform vec4 u_channelFlag;"
"uniform vec4 u_baseColor;"
"void main()"
"{"
"float isInside = "
"  step(u_baseColor.x, v_myPos.x/v_myPos.w)"
"* step(u_baseColor.y, v_myPos.y/v_myPos.w)"
"* step(v_myPos.x/v_myPos.w, u_baseColor.z)"
"* step(v_myPos.y/v_myPos.w, u_baseColor.w);"

"gl_FragColor = u_channelFlag * texture2D(s_texture0 , v_texCoord).a * isInside;"
"}";

static const char* VertShaderSrc =
"#version 330 core\n"
"attribute vec4 a_position;" //v.vertex
"attribute vec2 a_texCoord;" //v.texcoord
"varying vec2 v_texCoord;" //v2f.texcoord
"uniform mat4 u_matrix;"
"void main()"
"{"
"gl_Position = u_matrix * a_position;"
"v_texCoord = a_texCoord;"
"v_texCoord.y = 1.0 - v_texCoord.y;"
"}";

static const char* VertShaderSrcMasked =
"#version 330 core\n"
"attribute vec4 a_position;"
"attribute vec2 a_texCoord;"
"varying vec2 v_texCoord;"
"varying vec4 v_clipPos;"
"uniform mat4 u_matrix;"
"uniform mat4 u_clipMatrix;"
"void main()"
"{"
"gl_Position = u_matrix * a_position;"
"v_clipPos = u_clipMatrix * a_position;"
"v_texCoord = a_texCoord;"
"v_texCoord.y = 1.0 - v_texCoord.y;"
"}";

static const char* FragShaderSrc =
"#version 330 core\n"
"varying vec2 v_texCoord;" //v2f.texcoord
"uniform sampler2D s_texture0;" //_MainTex
"uniform vec4 u_baseColor;" //v2f.color
"uniform vec4 u_multiplyColor;"
"uniform vec4 u_screenColor;"
"void main()"
"{"
"vec4 texColor = texture2D(s_texture0 , v_texCoord);"
"texColor.rgb = texColor.rgb * u_multiplyColor.rgb;"
"texColor.rgb = texColor.rgb + u_screenColor.rgb - (texColor.rgb * u_screenColor.rgb);"
"vec4 color = texColor * u_baseColor;"
"gl_FragColor = vec4(color.rgb * color.a,  color.a);"
"}";

static const char* FragShaderSrcPremultipliedAlpha =
"#version 330 core\n"
"varying vec2 v_texCoord;"
"uniform sampler2D s_texture0;" //_MainTex
"uniform vec4 u_baseColor;" //v2f.color
"uniform vec4 u_multiplyColor;"
"uniform vec4 u_screenColor;"
"void main()"
"{"
"vec4 texColor = texture2D(s_texture0 , v_texCoord);"
"texColor.rgb = texColor.rgb * u_multiplyColor.rgb;"
"texColor.rgb = (texColor.rgb + u_screenColor.rgb * texColor.a) - (texColor.rgb * u_screenColor.rgb);"
"gl_FragColor = texColor * u_baseColor;"
"}";

static const char* FragShaderSrcMask =
"#version 330 core\n"
"varying vec2 v_texCoord;"
"varying vec4 v_clipPos;"
"uniform sampler2D s_texture0;"
"uniform sampler2D s_texture1;"
"uniform vec4 u_channelFlag;"
"uniform vec4 u_baseColor;"
"uniform vec4 u_multiplyColor;"
"uniform vec4 u_screenColor;"
"void main()"
"{"
"vec4 texColor = texture2D(s_texture0 , v_texCoord);"
"texColor.rgb = texColor.rgb * u_multiplyColor.rgb;"
"texColor.rgb = texColor.rgb + u_screenColor.rgb - (texColor.rgb * u_screenColor.rgb);"
"vec4 col_formask = texColor * u_baseColor;"
"col_formask.rgb = col_formask.rgb  * col_formask.a ;"
"vec4 clipMask = (1.0 - texture2D(s_texture1, v_clipPos.xy / v_clipPos.w)) * u_channelFlag;"
"float maskVal = clipMask.r + clipMask.g + clipMask.b + clipMask.a;"
"col_formask = col_formask * maskVal;"
"gl_FragColor = col_formask;"
"}";

static const char* FragShaderSrcMaskInverted =
"#version 330 core\n"
"varying vec2 v_texCoord;"
"varying vec4 v_clipPos;"
"uniform sampler2D s_texture0;"
"uniform sampler2D s_texture1;"
"uniform vec4 u_channelFlag;"
"uniform vec4 u_baseColor;"
"uniform vec4 u_multiplyColor;"
"uniform vec4 u_screenColor;"
"void main()"
"{"
"vec4 texColor = texture2D(s_texture0 , v_texCoord);"
"texColor.rgb = texColor.rgb * u_multiplyColor.rgb;"
"texColor.rgb = texColor.rgb + u_screenColor.rgb - (texColor.rgb * u_screenColor.rgb);"
"vec4 col_formask = texColor * u_baseColor;"
"col_formask.rgb = col_formask.rgb  * col_formask.a ;"
"vec4 clipMask = (1.0 - texture2D(s_texture1, v_clipPos.xy / v_clipPos.w)) * u_channelFlag;"
"float maskVal = clipMask.r + clipMask.g + clipMask.b + clipMask.a;"
"col_formask = col_formask * (1.0 - maskVal);"
"gl_FragColor = col_formask;"
"}";

static const char* FragShaderSrcMaskPremultipliedAlpha =
"#version 330 core\n"
"varying vec2 v_texCoord;"
"varying vec4 v_clipPos;"
"uniform sampler2D s_texture0;"
"uniform sampler2D s_texture1;"
"uniform vec4 u_channelFlag;"
"uniform vec4 u_baseColor;"
"uniform vec4 u_multiplyColor;"
"uniform vec4 u_screenColor;"
"void main()"
"{"
"vec4 texColor = texture2D(s_texture0 , v_texCoord);"
"texColor.rgb = texColor.rgb * u_multiplyColor.rgb;"
"texColor.rgb = (texColor.rgb + u_screenColor.rgb * texColor.a) - (texColor.rgb * u_screenColor.rgb);"
"vec4 col_formask = texColor * u_baseColor;"
"vec4 clipMask = (1.0 - texture2D(s_texture1, v_clipPos.xy / v_clipPos.w)) * u_channelFlag;"
"float maskVal = clipMask.r + clipMask.g + clipMask.b + clipMask.a;"
"col_formask = col_formask * maskVal;"
"gl_FragColor = col_formask;"
"}";

static const char* FragShaderSrcMaskInvertedPremultipliedAlpha =
"#version 330 core\n"
"varying vec2 v_texCoord;"
"varying vec4 v_clipPos;"
"uniform sampler2D s_texture0;"
"uniform sampler2D s_texture1;"
"uniform vec4 u_channelFlag;"
"uniform vec4 u_baseColor;"
"uniform vec4 u_multiplyColor;"
"uniform vec4 u_screenColor;"
"void main()"
"{"
"vec4 texColor = texture2D(s_texture0 , v_texCoord);"
"texColor.rgb = texColor.rgb * u_multiplyColor.rgb;"
"texColor.rgb = (texColor.rgb + u_screenColor.rgb * texColor.a) - (texColor.rgb * u_screenColor.rgb);"
"vec4 col_formask = texColor * u_baseColor;"
"vec4 clipMask = (1.0 - texture2D(s_texture1, v_clipPos.xy / v_clipPos.w)) * u_channelFlag;"
"float maskVal = clipMask.r + clipMask.g + clipMask.b + clipMask.a;"
"col_formask = col_formask * (1.0 - maskVal);"
"gl_FragColor = col_formask;"
"}";
