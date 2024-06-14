#pragma pack(push, 1)
struct gl_t{
  uint32_t res0 = 320;
  uint32_t res1 = 240;

  struct pixel_t{
    uint8_t c[3];
  }*pixmap;

  struct point_t{
    f32_t p[2];
  };

  struct draw_options_t{
    f32_t PointSize;
  }draw_options;

  /*
    draw_options
    rd == render data
  */
  typedef point_t(*vertexcb_t)(draw_options_t *, void *);
  /*
    rd == render data
    rp0 == relative position0
    rp1 == relative position1
    ap0 == absolute position0
    ap1 == absolute position1
  */
  typedef pixel_t(*fragcb_t)(void *, f32_t, f32_t, f32_t, f32_t);

  #define BLL_set_prefix ShaderList
  #define BLL_set_Link 0
  #define BLL_set_Recycle 1
  #define BLL_set_IntegerNR 1
  #define BLL_set_AreWeInsideStruct 1
  #define BLL_set_NodeData \
    uint8_t type; \
    void *func;
  #define BLL_set_type_node uint8_t
  #include <BLL/BLL.h>
  ShaderList_t ShaderList;
  #define BLL_set_prefix ProgramList
  #define BLL_set_Link 0
  #define BLL_set_Recycle 1
  #define BLL_set_IntegerNR 1
  #define BLL_set_AreWeInsideStruct 1
  #define BLL_set_NodeData \
    vertexcb_t vertexcb; \
    fragcb_t fragcb;
  #define BLL_set_type_node uint8_t
  #include <BLL/BLL.h>
  ProgramList_t ProgramList;

  uint8_t CurrentProgram;
  pixel_t _ClearColor;

  enum GLenum{
    VERTEX_SHADER,
    FRAGMENT_SHADER,

    POINTS
  };
  using GLbitfield = uint32_t;
  constexpr static GLbitfield COLOR_BUFFER_BIT = 0x00000001;

  void ClearColor(f32_t red, f32_t green, f32_t blue, f32_t alpha){
    _ClearColor.c[0] = red * 0xff;
    _ClearColor.c[1] = green * 0xff;
    _ClearColor.c[2] = blue * 0xff;
  }
  void Clear(GLbitfield mask){
    if(mask & COLOR_BUFFER_BIT){
      mask ^= COLOR_BUFFER_BIT;

      for(uint32_t i1 = 0; i1 < res1; i1++){
        for(uint32_t i0 = 0; i0 < res0; i0++){
          pixmap[i1 * res0 + i0] = _ClearColor;
        }
      }
    }

    if(mask){
      __abort();
    }
  }

  uint32_t CreateShader(GLenum shaderType){
    if(shaderType == VERTEX_SHADER){

    }
    else if(shaderType == FRAGMENT_SHADER){

    }
    else{
      __abort();
      return 0;
    }

    auto r = ShaderList.NewNode();
    ShaderList[r].type = shaderType;
    ShaderList[r].func = NULL;
    return r;
  }
  void ShaderSource(uint32_t shader, void *func){
    /* TOOD check if shader is valid */

    ShaderList[shader].func = func;
  }
  void CompileShader(uint32_t shader){
    /* TOOD check if shader is valid */

    /* super complex compiling */
  }
  void DeleteShader(uint32_t shader){
    /* TOOD check if shader is valid */

    ShaderList.Recycle(shader);
  }

  uint32_t CreateProgram(){
    return ProgramList.NewNode();
  }
  void AttachShader(uint32_t program, uint32_t shader){
    /* TOOD check if program is valid */
    /* TOOD check if shader is valid */

    auto &p = ProgramList[program];
    auto &s = ShaderList[shader];

    if(s.type == FRAGMENT_SHADER){
      p.fragcb = (fragcb_t)s.func;
    }
    else if(s.type == VERTEX_SHADER){
      p.vertexcb = (vertexcb_t)s.func;
    }
    else{
      /* internal error since CreateShader supposed to not give this */
      __abort();
    }
  }
  void LinkProgram(uint32_t program){
    /* TOOD check if program is valid */

    auto &p = ProgramList[program];
    if(p.vertexcb == NULL){
      __abort(); /* fail */
    }
    if(p.fragcb == NULL){
      __abort(); /* fail */
    }
  }
  void UseProgram(uint32_t program){
    /* TOOD check if program is valid */

    CurrentProgram = program;
  }

  void DrawArrays(GLenum mode, void *buffer, uint32_t rdsize, uint32_t count){
    auto &p = ProgramList[CurrentProgram];

    for(uint32_t i = 0; i < count; i++){
      auto bptr = &((uint8_t *)buffer)[i * rdsize];

      if(mode == POINTS){
        auto point = p.vertexcb(&draw_options, bptr);
        point.p[0] += res0 / 2;
        point.p[1] += res1 / 2;

        f32_t s0_f = point.p[0] - draw_options.PointSize;
        if(s0_f < 0){
          s0_f = 0;
        }
        uint32_t s0_i = s0_f;
        f32_t s1_f = point.p[1] - draw_options.PointSize;
        if(s1_f < 0){
          s1_f = 0;
        }
        uint32_t s1_i = s1_f;
        f32_t e0_f = point.p[0] + draw_options.PointSize;
        if(e0_f > res0){
          e0_f = res0;
        }
        uint32_t e0_i = e0_f;
        f32_t e1_f = point.p[1] + draw_options.PointSize;
        if(e1_f > res1){
          e1_f = res1;
        }
        uint32_t e1_i = e1_f;

        for(uint32_t p1 = s1_i; p1 < e1_i; p1++){
          for(uint32_t p0 = s0_i; p0 < e0_i; p0++){
            pixmap[p1 * res0 + p0] = p.fragcb(bptr, 0, 0, 0, 0);
          }
        }
      }
      else{
        __abort();
      }
    }
  }

  void PointSize(f32_t size){
    draw_options.PointSize = size;
  }

  void Open(uint32_t res0, uint32_t res1){
    CurrentProgram = 0;

    draw_options.PointSize = 1;

    this->res0 = res0;
    this->res1 = res1;

    pixmap = (pixel_t *)A_resize(NULL, res0 * res1 * sizeof(pixel_t));

    /* 0 is reserved in opengl */
    ShaderList.Open();
    ShaderList.NewNode();
    ProgramList.Open();
    ProgramList.NewNode();
  }
  void Close(){
    ProgramList.Close();
    ShaderList.Close();

    A_resize(pixmap, 0);
  }
};
#pragma pack(pop)
