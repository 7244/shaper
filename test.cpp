#include <WITCH/WITCH.h>
#include <WITCH/A/A.h>
#include <WITCH/RAND/RAND.h>

#include "shaper/shaper.h"
shaper_t shaper;

/* key ids */
struct kids{enum{
  sti,
  blending,
  depth
};};

struct kp{enum{
  common
};};

struct shapes{enum{
  rectangle
};};

#pragma pack(push, 1)
  struct shape_rectangle_KeyPack_t{
    uint8_t blending;
    uint16_t depth;
    shaper_t::ShapeTypeIndex_t sti = shapes::rectangle;
  };
  struct shape_rectangle_RenderData_t{
    f32_t SizeX;
    f32_t SizeY;
    // ...
  };
  struct shape_rectangle_Data_t{
    uint8_t filler;
  };
#pragma pack(pop)

shaper_t::ShapeID_t shape_rectangle_add(
  const shape_rectangle_KeyPack_t KeyPack,
  const shape_rectangle_RenderData_t RenderData,
  const shape_rectangle_Data_t Data
){
  return shaper.add(shapes::rectangle, &KeyPack, &RenderData, &Data);
}

int main(){
  shaper.Open();

  shaper.AddKey(kids::sti, sizeof(shaper_t::ShapeTypeIndex_t), shaper_t::KeyBitOrderAny);

  shaper.AddKey(kids::blending, sizeof(uint8_t), shaper_t::KeyBitOrderLow);

  /* changed with blending */
  shaper.AddKey(kids::depth, sizeof(uint16_t), shaper_t::KeyBitOrderLow);

  {
    shaper_t::KeyTypeIndex_t ktia[] = {
      kids::blending,
      kids::depth,
      kids::sti
    };
    shaper.AddKeyPack(kp::common, sizeof(ktia) / sizeof(ktia[0]), ktia);
  }

  shaper.AddShapeType(shapes::rectangle, kp::common, {
    .MaxElementPerBlock = 0xff,
    .RenderDataSize = sizeof(shape_rectangle_RenderData_t),
    .DataSize = 0
  });

  auto sid = shape_rectangle_add(
    {
      .blending = false,
      .depth = 5
    },{
      .SizeX = 7,
      .SizeY = 9
    },{

    }
  );

  shaper_t::KeyPackTraverse_t KeyPackTraverse;
  KeyPackTraverse.Init(shaper);
  while(KeyPackTraverse.Loop(shaper)){
    shaper_t::KeyTraverse_t KeyTraverse;
    KeyTraverse.Init(shaper, KeyPackTraverse.kpi);
    shaper_t::KeyTypeIndex_t kti;
    while(KeyTraverse.Loop(shaper, kti)){
      uint32_t Key = 0;
      if(kti == kids::blending){
        Key = *(decltype(shape_rectangle_KeyPack_t::blending) *)KeyTraverse.KeyData;
      }
      if(kti == kids::depth){
        Key = *(decltype(shape_rectangle_KeyPack_t::depth) *)KeyTraverse.KeyData;
      }
      if(kti == kids::sti){
        Key = *(decltype(shape_rectangle_KeyPack_t::sti) *)KeyTraverse.KeyData;
      }

      printf("salsa+\n");
      printf("%u\n", kti);
      if(kti != (shaper_t::KeyTypeIndex_t)-1){
        printf("%u\n", Key);
      }
      printf("salsa-\n");

      if(kti == (shaper_t::KeyTypeIndex_t)-1){
        shaper_t::BlockTraverse_t BlockTraverse;
        auto sti = BlockTraverse.Init(shaper, KeyPackTraverse.kpi, KeyTraverse.bmid(shaper));
        do{
          printf("block came %u\n", sti);
        }while(BlockTraverse.Loop(shaper));
      }
    }
  }

  return 0;
}
