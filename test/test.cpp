#include <WITCH/WITCH.h>
#include <WITCH/A/A.h>
#include <WITCH/RAND/RAND.h>

#include "../shaper/shaper.h"
shaper_t shaper;

using KeyType_blending = uint8_t;
using KeyType_depth = uint16_t;
using KeyType_sti = shaper_t::KeyTypeIndex_t;

/* key ids */
struct kids{enum : shaper_t::KeyTypeIndex_t{
  blending,
  depth,
  sti
};};

struct shapes{enum : shaper_t::ShapeTypeIndex_t{
  rectangle
};};

#pragma pack(push, 1)
  struct shape_rectangle_RenderData_t{
    f32_t SizeX;
    f32_t SizeY;
    // ...
  };
  struct shape_rectangle_Data_t{
    uint8_t filler;
  };
#pragma pack(pop)

template <uintptr_t s>
struct structarr_t {
  uint8_t p[s];
  uint8_t &operator[](uintptr_t i){
    return p[i];
  }
};
template<
  typename... Ts,
  uintptr_t s = (sizeof(Ts) + ...)
>constexpr shaper_t::ShapeID_t shape_add(
  shaper_t::ShapeTypeIndex_t sti,
  const auto &rd,
  const auto &d,
  Ts... args
){
  structarr_t<s> a;
  uintptr_t i = 0;
  ([&](auto arg) {
    __builtin_memcpy(&a[i], &arg, sizeof(arg));
    i += sizeof(arg);
  }(args), ...);
  constexpr uintptr_t count = (!!(sizeof(Ts) + 1) + ...);
  static_assert(count % 2 == 0);
  uintptr_t LastKeyOffset = s - (sizeof(Ts), ...) - 1;
  shaper.PrepareKeysForAdd(&a, LastKeyOffset);
  return shaper.add(sti, &a, s, &rd, &d);
}

int main(){
  shaper.Open();

  shaper.AddKey(kids::blending, sizeof(KeyType_blending), shaper_t::KeyBitOrderLow);

  /* changed with blending */
  shaper.AddKey(kids::depth, sizeof(KeyType_depth), shaper_t::KeyBitOrderLow);

  shaper.AddKey(kids::sti, sizeof(KeyType_sti), shaper_t::KeyBitOrderAny);

  shaper.SetShapeType(shapes::rectangle, {
    .MaxElementPerBlock = 0xff,
    .RenderDataSize = sizeof(shape_rectangle_RenderData_t),
    .DataSize = 0,
    .Flag = 0
  });

  auto sid = shape_add(shapes::rectangle,
    shape_rectangle_RenderData_t{
      .SizeX = 7,
      .SizeY = 9
    },
    shape_rectangle_Data_t{},
    kids::blending, (KeyType_blending)1,
    kids::depth, (KeyType_depth)5,
    kids::sti, shapes::rectangle
  );
  //shaper.remove(sid);

  shaper_t::KeyTraverse_t kt;
  kt.Init(shaper);
  while(kt.Loop(shaper)){
    shaper_t::KeyTypeIndex_t kti = kt.kti(shaper);
    uint32_t Key = 0;
    if(kti == kids::blending){
      Key = *(KeyType_blending *)kt.kd();
    }
    if(kti == kids::depth){
      Key = *(KeyType_depth *)kt.kd();
    }
    if(kti == kids::sti){
      Key = *(KeyType_sti *)kt.kd();
    }

    printf("salsa+\n");
    printf("%u\n", kti);
    if(kti != (shaper_t::KeyTypeIndex_t)-1){
      printf("%u\n", Key);
    }
    printf("salsa-\n");

    if(kt.isbm){
      shaper_t::BlockTraverse_t BlockTraverse;
      auto sti = BlockTraverse.Init(shaper, kt.bmid());
      do{
        printf("block came %u\n", sti);
      }while(BlockTraverse.Loop(shaper));
    }
  }

  shaper.Close();

  return 0;
}
