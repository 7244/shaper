#define output_res0 320
#define output_res1 240

#include <WITCH/WITCH.h>
#include <WITCH/A/A.h>
#include <WITCH/IO/IO.h>
#include <WITCH/IO/print.h>

void print(const char *format, ...){
  IO_fd_t fd_stdout;
  IO_fd_set(&fd_stdout, FD_OUT);
  va_list argv;
  va_start(argv, format);
  IO_vprint(&fd_stdout, format, argv);
  va_end(argv);
}
void printe(const char *format, ...){
  IO_fd_t fd_stdout;
  IO_fd_set(&fd_stdout, FD_ERR);
  va_list argv;
  va_start(argv, format);
  IO_vprint(&fd_stdout, format, argv);
  va_end(argv);
}

#include "pseudogl.h"

#include "../shaper/shaper.h"
shaper_t shaper;

using KeyType_depth = uint16_t;
using KeyType_prog = uint32_t;
using KeyType_sti = shaper_t::KeyTypeIndex_t;

/* key ids */
struct kids{enum : shaper_t::KeyTypeIndex_t{
  depth,
  prog,
  sti
};};

struct shapes{enum : shaper_t::ShapeTypeIndex_t{
  square,
  circle
};};

#pragma pack(push, 1)
  struct shape_square_RenderData_t{
    f32_t p0;
    f32_t p1;
    f32_t s;
  };
  struct shape_square_Data_t{
    uint8_t filler;
  };

  struct shape_circle_RenderData_t{
    uint8_t filler;
  };
  struct shape_circle_Data_t{
    uint8_t filler;
  };
#pragma pack(pop)

template<
  typename... Ts,
  uintptr_t s = (sizeof(Ts) + ...)
>constexpr shaper_t::ShapeID_t shape_add(
  shaper_t::ShapeTypeIndex_t sti,
  const auto& rd,
  const auto& d,
  Ts... args
){
  struct structarr_t {
    uint8_t p[s];
    uint8_t& operator[](uintptr_t i) {
      return p[i];
    }
  };
  structarr_t a;
  uintptr_t i = 0;
  ([&](auto arg) {
    __builtin_memcpy(&a[i], &arg, sizeof(arg));
    i += sizeof(arg);
    }(args), ...);
  constexpr uintptr_t count = (!!(sizeof(Ts) + 1) + ...);
  static_assert(count % 2 == 0);
  uintptr_t LastKeyOffset = s - (sizeof(Ts), ...) - sizeof(shaper_t::KeyTypeIndex_t);
  shaper.PrepareKeysForAdd(&a, LastKeyOffset);
  return shaper.add(sti, &a, s, &rd, &d);
}

#include <cmath>

int main(){
  gl_t gl;
  gl.Open(output_res0, output_res1);

  gl.ClearColor(0, 0, 0, 0);

  auto vertex = gl.CreateShader(gl_t::VERTEX_SHADER);
  gl.ShaderSource(vertex, (void *)+[](
    gl_t::draw_options_t *draw_options,
    shape_square_RenderData_t *rd
  ){
    draw_options->PointSize = rd->s;
    gl_t::point_t r;
    r.p[0] = rd->p0;
    r.p[1] = rd->p1;
    return r;
  });
  gl.CompileShader(vertex);

  auto frag0 = gl.CreateShader(gl_t::FRAGMENT_SHADER);
  gl.ShaderSource(frag0, (void *)+[](
    shape_square_RenderData_t *rd,
    f32_t rp0, f32_t rp1,
    f32_t ap0, f32_t ap1
  ){
    gl_t::pixel_t r;
    r.c[0] = 0xff;
    r.c[1] = 0;
    r.c[2] = 0xff;
    return r;
  });
  gl.CompileShader(frag0);
  auto prog0 = gl.CreateProgram();
  gl.AttachShader(prog0, vertex);
  gl.AttachShader(prog0, frag0);
  gl.LinkProgram(prog0);
  gl.DeleteShader(frag0);

  auto frag1 = gl.CreateShader(gl_t::FRAGMENT_SHADER);
  gl.ShaderSource(frag1, (void *)+[](
    shape_square_RenderData_t *rd,
    f32_t rp0, f32_t rp1,
    f32_t ap0, f32_t ap1
  ){
    gl_t::pixel_t r;
    r.c[0] = 0xff;
    r.c[1] = 0;
    r.c[2] = 0;
    return r;
  });
  gl.CompileShader(frag1);
  auto prog1 = gl.CreateProgram();
  gl.AttachShader(prog1, vertex);
  gl.AttachShader(prog1, frag1);
  gl.LinkProgram(prog1);
  gl.DeleteShader(frag1);

  shaper.Open();

  shaper.AddKey(kids::depth, sizeof(KeyType_depth), shaper_t::KeyBitOrderHigh);
  shaper.AddKey(kids::prog, sizeof(KeyType_prog), shaper_t::KeyBitOrderAny);
  shaper.AddKey(kids::sti, sizeof(KeyType_sti), shaper_t::KeyBitOrderAny);

  shaper.AddShapeType(shapes::square, {
    .MaxElementPerBlock = 0xff,
    .RenderDataSize = sizeof(shape_square_RenderData_t),
    .DataSize = sizeof(shape_square_Data_t)
  });
  shaper.AddShapeType(shapes::circle, {
    .MaxElementPerBlock = 0xff,
    .RenderDataSize = sizeof(shape_circle_RenderData_t),
    .DataSize = sizeof(shape_circle_Data_t)
  });

  auto sid0 = shape_add(shapes::square,
    shape_square_RenderData_t{
      .p0 = -8,
      .p1 = -8,
      .s = 32
    },
    shape_square_Data_t{},
    kids::depth, (KeyType_depth)0,
    kids::prog, prog0,
    kids::sti, shapes::square
  );
  auto sid1 = shape_add(shapes::square,
    shape_square_RenderData_t{
      .p0 = +8,
      .p1 = +8,
      .s = 32
    },
    shape_square_Data_t{},
    kids::depth, (KeyType_depth)1,
    kids::prog, prog1,
    kids::sti, shapes::square
  );

  for(uint32_t iframe = 0; iframe < 120; iframe++){
    gl.Clear(gl_t::COLOR_BUFFER_BIT);

    {
      auto sid0rd = (shape_square_RenderData_t *)sid0.GetRenderData(shaper);
      sid0rd->p0 = -8 + std::sin(0.25 * iframe) * 32;
      sid0rd->p1 = -8 + std::cos(0.25 * iframe) * 32;
      auto sid1rd = (shape_square_RenderData_t *)sid1.GetRenderData(shaper);
      sid1rd->p0 = +8 + std::cos(0.25 * iframe) * 32;
      sid1rd->p1 = +8 + std::sin(0.25 * iframe) * 32;
    }

    shaper_t::KeyTraverse_t kt;
    kt.Init(shaper);
    while(kt.Loop(shaper)){
      shaper_t::KeyTypeIndex_t kti = kt.kti(shaper);
      if(kti == kids::depth){
        /* ~lost inside himself~ */
      }
      else if(kti == kids::prog){
        gl.UseProgram(*(KeyType_prog *)kt.kd());
      }
      else if(kti == kids::sti){
        /* ~sitititi~ */
      }
      else{
        /* internal error */
        __abort();
      }

      if(kt.isbm){
        shaper_t::BlockTraverse_t bt;
        auto sti = bt.Init(shaper, kt.bmid());
        do{
          gl.DrawArrays(
            gl_t::POINTS,
            bt.GetRenderData(shaper),
            sizeof(shape_square_RenderData_t),
            bt.GetAmount(shaper)
          );
        }while(bt.Loop(shaper));
      }
    }

    print("%.*s", (uintptr_t)output_res0 * output_res1 * 3, gl.pixmap);
  }

  shaper.Close();

  gl.Close();

  return 0;
}
