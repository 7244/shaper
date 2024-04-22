#include <WITCH/WITCH.h>
#include <WITCH/RAND/RAND.h>

#include "shaper/shaper.h"
shaper_t shaper;

#pragma pack(push, 1)
  struct KeyPack_t{
    uint16_t depth;
    uint8_t ShapeType;
  };
#pragma pack(pop)

constexpr uint8_t KeyAmount = 2;
shaper_t::KeyInfo_t KeyInfos[KeyAmount] = {
  {.Size = sizeof(KeyPack_t::depth), .BitOrder = shaper_t::KeyBitOrderLow},
  {.Size = sizeof(KeyPack_t::ShapeType), .BitOrder = shaper_t::KeyBitOrderAny}
};

#pragma pack(push, 1)
  struct RenderDataRectangle_t{
    f32_t xSize;
    f32_t ySize;
  };
  struct DataRectangle_t{

  };
  struct RenderDataCircle_t{
    f32_t radius;
  };
  struct DataCircle_t{
    
  };
#pragma pack(pop)

auto rectangle_push_back(KeyPack_t KeyPack, RenderDataRectangle_t RenderData, DataRectangle_t Data){
  KeyPack.ShapeType = 0;
  shaper_t::BlockID_t BlockID;
  auto r = shaper.add(&KeyPack, KeyPack.ShapeType, &RenderData, &Data, &BlockID);
  if(!BlockID.iic()){
    auto &PerBlockData = shaper.GetPerBlockData(KeyPack.ShapeType, BlockID);
    // add block list
  }
  return r;
}
void rectangle_remove(shaper_t::ShapeID_t shapeid){
  shaper_t::PerBlockData_t PerBlockData;
  shaper.remove(shapeid, &PerBlockData);
  if(!PerBlockData.ii()){
    /* we need to remove */
  }
}
auto circle_push_back(KeyPack_t KeyPack, RenderDataCircle_t RenderData, DataCircle_t Data){
  KeyPack.ShapeType = 1;
  shaper_t::BlockID_t BlockID;
  auto r = shaper.add(&KeyPack, KeyPack.ShapeType, &RenderData, &Data, &BlockID);
  if(!BlockID.iic()){
    // add block list
  }
  return r;
}

int main(){
  constexpr static uintptr_t ShapeAmount = 2;

  shaper_t::BlockProperties_t BlockProperties[ShapeAmount] = {
    {
      .MaxElementPerBlock = 0x100,
      .RenderDataSize = sizeof(RenderDataRectangle_t),
      .DataSize = sizeof(DataRectangle_t)
    },
    {
      .MaxElementPerBlock = 0x80,
      .RenderDataSize = sizeof(RenderDataCircle_t),
      .DataSize = sizeof(DataCircle_t)
    },
  };
  shaper.Open({
    .KeyAmount = KeyAmount,
    .KeyInfos = KeyInfos,
    .ShapeTypeAmount = ShapeAmount,
    .BlockProperties = BlockProperties
  });

  shaper_t::ShapeID_t sid_list[5000];

  uintptr_t TotalPush = 1;
  for(uintptr_t i = 0; i < TotalPush; i++){
    sid_list[i] = rectangle_push_back(
      {
        .depth = (uint16_t)(1 * 8)
      },
      {
        .xSize = (f32_t)(RAND_hard_32() % 100),
        .ySize = (f32_t)(RAND_hard_32() % 100)
      },
      {}
    );
  }


  for (uint32_t i = 0; i < TotalPush / 2; ++i) {
    //shaper.remove(sid_list[i]);
  }

  shaper_t::KeyTraverse_t KeyTraverse;
  KeyTraverse.Init(shaper);
  while(KeyTraverse.Loop(shaper)){
    uint32_t Key;
    if(KeyTraverse.KeyIndex == 0){ Key = *(decltype(KeyPack_t::depth) *)KeyTraverse.KeyData; }
    if(KeyTraverse.KeyIndex == 1){ Key = *(decltype(KeyPack_t::ShapeType) *)KeyTraverse.KeyData; }
    printf("salsa %u %u\n", KeyTraverse.KeyIndex, Key);

    if(KeyTraverse.KeyIndex == KeyAmount - 1){
      shaper_t::BlockTraverse_t BlockTraverse;
      auto ShapeType = BlockTraverse.Init(shaper, KeyTraverse.Get(shaper));
      do{
        printf("block came\n");
        static uint32_t salsa;
        if(salsa++ > 10){
          exit(1);
        }
      }while(BlockTraverse.Loop(shaper));
    }
  }

  return 0;
}
