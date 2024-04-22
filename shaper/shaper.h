#ifndef shaper_set_MaxShapeTypes
  #define shaper_set_MaxShapeTypes 0x80
#endif

#ifndef shaper_set_MaxKeyAmount
  #define shaper_set_MaxKeyAmount 0x80
#endif

/* in bytes */
#ifndef shaper_set_MaxKeySize
  #define shaper_set_MaxKeySize 8
#endif

#ifdef shaper_set_MaxElementPerBlock
  #error did you meant to define shaper_set_MaxMaxElementPerBlock
#endif
#ifndef shaper_set_MaxMaxElementPerBlock
  #define shaper_set_MaxMaxElementPerBlock 0x8000
#endif

/* in bytes */
#ifdef shaper_set_MaxKeySizesSum
  #define shaper_set_MaxKeySizesSum 0xff
#endif

#ifndef shaper_set_MaxShapeRenderDataSize
  #define shaper_set_MaxShapeRenderDataSize 0xffff
#endif
#ifndef shaper_set_MaxShapeDataSize
  #define shaper_set_MaxShapeDataSize 0xffff
#endif

struct shaper_t{
  public: /* -------------------------------------------------------------------------------- */

  #if shaper_set_MaxShapeTypes <= 0xff
    typedef uint8_t ShapeTypeAmount_t;
  #elif shaper_set_MaxShapeTypes <= 0xffff
    typedef uint16_t ShapeTypeAmount_t;
  #elif shaper_set_MaxShapeTypes <= 0xffffffff
    typedef uint32_t ShapeTypeAmount_t;
  #else
    #error ?
  #endif
  #if shaper_set_MaxShapeTypes <= 0x100
    typedef uint8_t ShapeTypeIndex_t;
  #elif shaper_set_MaxShapeTypes <= 0x10000
    typedef uint16_t ShapeTypeIndex_t;
  #elif shaper_set_MaxShapeTypes <= 0x100000000
    typedef uint32_t ShapeTypeIndex_t;
  #else
    #error ?
  #endif

  #if shaper_set_MaxKeyAmount <= 0xff
    typedef uint8_t KeyAmount_t;
  #else
    #error ?
  #endif
  #if shaper_set_MaxKeyAmount <= 0x100
    typedef uint8_t KeyIndex_t;
  #else
    #error ?
  #endif

  #if shaper_set_MaxMaxElementPerBlock <= 0xff
    typedef uint8_t MaxElementPerBlock_t;
  #elif shaper_set_MaxMaxElementPerBlock <= 0xffff
    typedef uint16_t MaxElementPerBlock_t;
  #elif shaper_set_MaxMaxElementPerBlock <= 0xffffffff
    typedef uint32_t MaxElementPerBlock_t;
  #else
    #error ?
  #endif
  #if shaper_set_MaxMaxElementPerBlock <= 0x100
    typedef uint8_t ElementIndexInBlock_t;
  #elif shaper_set_MaxMaxElementPerBlock <= 0x10000
    typedef uint16_t ElementIndexInBlock_t;
  #elif shaper_set_MaxMaxElementPerBlock <= 0x100000000
    typedef uint32_t ElementIndexInBlock_t;
  #else
    #error ?
  #endif

  typedef uint8_t KeyData_t;

  #if shaper_set_MaxKeySize <= 0xff
    typedef uint8_t KeySizeInBytes_t;
  #else
    #error ?
  #endif
  #if shaper_set_MaxKeySize * 8 <= 0xff
    typedef uint8_t KeySizeInBits_t;
  #else
    #error ?
  #endif

  #if shaper_set_MaxKeySizesSum <= 0xff
    typedef uint8_t KeySizesSumInBytes_t;
  #else
    #error ?
  #endif

  #if shaper_set_MaxShapeRenderDataSize <= 0xff
    typedef uint8_t ShapeRenderDataSize_t;
  #elif shaper_set_MaxShapeRenderDataSize <= 0xffff
    typedef uint16_t ShapeRenderDataSize_t;
  #elif shaper_set_MaxShapeRenderDataSize <= 0xffffffff
    typedef uint32_t ShapeRenderDataSize_t;
  #else
    #error ?
  #endif

  #if shaper_set_MaxShapeDataSize <= 0xff
    typedef uint8_t ShapeDataSize_t;
  #elif shaper_set_MaxShapeDataSize <= 0xffff
    typedef uint16_t ShapeDataSize_t;
  #elif shaper_set_MaxShapeDataSize <= 0xffffffff
    typedef uint32_t ShapeDataSize_t;
  #else
    #error ?
  #endif

  typedef uint8_t ShapeRenderData_t;
  typedef uint8_t ShapeData_t;

  private: /* ------------------------------------------------------------------------------- */

  /* key tree and block manager node reference type */
  typedef uint16_t ktbmnr_t;

  /* block list node reference */
  /*
    total element a ShapeType can have is =
      (pow(2, (sizeof(blnr_t) * 8)) * MaxElementPerBlock)
  */
  typedef uint16_t blnr_t;

  #define BDBT_set_prefix kt
  #define BDBT_set_type_node ktbmnr_t
  #define BDBT_set_lcpp
  #define BDBT_set_KeySize 0
  #ifdef shaper_set_MaxKeySize
    #define BDBT_set_MaxKeySize (shaper_set_MaxKeySize * 8)
  #endif
  #define BDBT_set_AreWeInsideStruct 1
  #include <BDBT/BDBT.h>
  kt_t kt;
  kt_NodeReference_t kt_root;
  typedef kt_Key_t Key_t;

  public: /* -------------------------------------------------------------------------------- */

  #pragma pack(push, 1)
    struct KeyInfo_t{
      KeySizeInBytes_t Size;
      Key_t::BitOrder_t BitOrder;

      /* size in bits */
      KeySizeInBits_t sibit(){
        return (KeySizeInBits_t)Size * 8;
      }
    };
  #pragma pack(pop)

  typedef Key_t::BitOrder_t KeyBitOrder_t;
  constexpr static KeyBitOrder_t KeyBitOrderLow = Key_t::BitOrderLow;
  constexpr static KeyBitOrder_t KeyBitOrderHigh = Key_t::BitOrderHigh;
  constexpr static KeyBitOrder_t KeyBitOrderAny = Key_t::BitOrderAny;

  private: /* ------------------------------------------------------------------------------- */

  #define BLL_set_prefix BlockList
  #define BLL_set_Language 1
  #define BLL_set_Link 1
  #define BLL_set_NoSentinel
  #define BLL_set_UseUninitialisedValues 0
  #define BLL_set_AreWeInsideStruct 1
  #define BLL_set_type_node blnr_t
  #include <WITCH/BLL/BLL.h>
  struct Block_t{
    /* 
      (RenderDataSize + DataSize + sizeof(ShapeList_t::nr_t)) * (MaxElementPerBlock_m1 + 1) +
      sizeof(PerBlockData_t)
    */
    BlockList_t List;

    ElementIndexInBlock_t MaxElementPerBlock_m1; /* minus 1, (p.MaxElementPerBlock[n] - 1) */
    ShapeRenderDataSize_t RenderDataSize;
    ShapeDataSize_t DataSize;

    MaxElementPerBlock_t MaxElementPerBlock(){
      return (MaxElementPerBlock_t)MaxElementPerBlock_m1 + 1;
    }
  }*Blocks;

  #pragma pack(push, 1)
    struct bm_BaseData_t{
      ShapeTypeAmount_t ShapeType;
      BlockList_NodeReference_t FirstBlockNR;
      BlockList_NodeReference_t LastBlockNR;
      ElementIndexInBlock_t LastBlockElementCount;
    };
  #pragma pack(pop)

  #define BLL_set_prefix bm
  #define BLL_set_Language 1
  #define BLL_set_Link 0
  #define BLL_set_UseUninitialisedValues 0
  #define BLL_set_AreWeInsideStruct 1
  #define BLL_set_type_node ktbmnr_t
  #include <WITCH/BLL/BLL.h>
  bm_t bm;
  /* sizeof(bm_BaseData_t) + KeySizesSum */

  #pragma pack(push, 1)
    struct shape_t{
      bm_t::nr_t bmid;
      BlockList_t::nr_t blid;
      ElementIndexInBlock_t ElementIndex;
    };
  #pragma pack(pop)

  #define BLL_set_prefix ShapeList
  #define BLL_set_Language 1
  #define BLL_set_Link 0
  #define BLL_set_UseUninitialisedValues 0
  #define BLL_set_NodeDataType shape_t
  #define BLL_set_AreWeInsideStruct 1
  // actually it needs to be uint24_t
  #define BLL_set_type_node uint32_t
  #include <WITCH/BLL/BLL.h>
  ShapeList_t ShapeList;

  KeyAmount_t KeyAmount;
  KeyInfo_t KeyInfos[8];
  KeySizesSumInBytes_t KeySizesSum;
  ShapeTypeAmount_t ShapeTypeAmount;

  /* function internal datas */
  struct fid_t{
    union{
      void *base;
      /* used in remove */
      kt_NodeReference_t *knrs;
      /* used in traverse for store keys */
      KeyData_t *KeyDatas;
    };

    Key_t::Traverse_t *tra;
    #ifndef shaper_set_MaxKeySize
      Key_t::Traverse_t::ta_t **ta;
    #endif

    void Open(const shaper_t *shaper){
      base = ::operator new(
        sizeof(kt_NodeReference_t) * shaper->KeyAmount > shaper->KeySizesSum ? 
        sizeof(kt_NodeReference_t) * shaper->KeyAmount :
        shaper->KeySizesSum
      );

      tra = new Key_t::Traverse_t[shaper->KeyAmount];

      #ifndef shaper_set_MaxKeySize
        ta = new Key_t::Traverse_t::ta_t *[shaper->KeyAmount];
        for(KeyIndex_t KeyIndex = 0; KeyIndex < shaper->KeyAmount; KeyIndex++){
          ta[KeyIndex] = new Key_t::Traverse_t::ta_t[
            Key_t::Traverse_t::GetTraverseArraySize(shaper->KeyInfos[KeyIndex].sibit())
          ];
        }
      #endif
    }
    void Close(const shaper_t *shaper){
      #ifndef shaper_set_MaxKeySize
        for(KeyIndex_t KeyIndex = 0; KeyIndex < shaper->KeyAmount; KeyIndex++){
          delete ta[KeyIndex];
        }
        delete ta;
      #endif

      delete tra;

      ::operator delete(base);
    }
  }fid;

  /*
    block data format is this. %30
    RenderData   Data         ShapeID
    [|||       ] [|||       ] [|||       ]
  */

  /* TODO those are made uintptr_t to not overflow, maybe there is better way? */
  ShapeRenderData_t *_GetRenderData(
    ShapeTypeAmount_t ShapeType,
    BlockList_t::nr_t blid,
    uintptr_t ElementIndex
  ){
    return &((ShapeRenderData_t *)Blocks[ShapeType].List[blid])[
      (uintptr_t)Blocks[ShapeType].RenderDataSize * ElementIndex
    ];
  }
  ShapeData_t *_GetData(
    ShapeTypeAmount_t ShapeType,
    BlockList_t::nr_t blid,
    uintptr_t ElementIndex
  ){
    return &_GetRenderData(
      ShapeType,
      blid,
      Blocks[ShapeType].MaxElementPerBlock()
    )[(uintptr_t)Blocks[ShapeType].DataSize * ElementIndex];
  }
  ShapeList_t::nr_t &_GetShapeID(
    ShapeTypeAmount_t ShapeType,
    BlockList_t::nr_t blid,
    uintptr_t ElementIndex
  ){
    return *(ShapeList_t::nr_t *)&_GetData(
      ShapeType,
      blid,
      Blocks[ShapeType].MaxElementPerBlock()
    )[sizeof(ShapeList_t::nr_t) * ElementIndex];
  }

  public: /* -------------------------------------------------------------------------------- */

  #define BLL_set_prefix BlockQueue
  #define BLL_set_Language 1
  #define BLL_set_Link 1
  #define BLL_set_UseUninitialisedValues 0
  #define BLL_set_AreWeInsideStruct 1
  #define BLL_set_NodeData \
    bm_t::nr_t bmid; \
    BlockList_t::nr_t blid;
  #define BLL_set_type_node uint16_t
  #include <WITCH/BLL/BLL.h>
  BlockQueue_t BlockQueue;

  struct PerBlockData_t{
    /* durum opengl id stuff here */
    uint32_t MinEdit;
    uint32_t MaxEdit;
    BlockQueue_t::nr_t bqid;
    void si(){ /* set invalid */
      bqid = (decltype(bqid.NRI))-2;
    }
    bool ii(){ /* is invalid */
      return bqid == (decltype(bqid.NRI))-2;
    }
  };

  using BlockID_t = BlockList_t::nr_t;
  using bmid_t = bm_t::nr_t;
  using ShapeID_t = ShapeList_t::nr_t;

  ShapeRenderData_t *GetRenderData(
    ShapeID_t ShapeID
  ){
    auto &shape = ShapeList[ShapeID];
    auto bmbase = (bm_BaseData_t *)bm[shape.bmid];
    return _GetRenderData(bmbase->ShapeType, shape.blid, shape.ElementIndex);
  }
  ShapeData_t *GetData(
    ShapeID_t ShapeID
  ){
    auto &shape = ShapeList[ShapeID];
    auto bmbase = (bm_BaseData_t *)bm[shape.bmid];
    return _GetData(bmbase->ShapeType, shape.blid, shape.ElementIndex);
  }
  PerBlockData_t &GetPerBlockData(
    ShapeTypeAmount_t ShapeType,
    BlockID_t BlockID
  ){
    return *(PerBlockData_t *)&_GetShapeID(
      ShapeType,
      BlockID,
      Blocks[ShapeType].MaxElementPerBlock()
    );
  }

  struct BlockProperties_t{
    MaxElementPerBlock_t MaxElementPerBlock;
    decltype(Block_t::RenderDataSize) RenderDataSize;
    decltype(Block_t::DataSize) DataSize;
  };

  struct OpenProperties_t{
    decltype(KeyAmount) KeyAmount;
    std::remove_reference<decltype(KeyInfos[0])>::type *KeyInfos;

    /* how many shapes we have like rectangle circle sprite, means 3 */
    ShapeTypeAmount_t ShapeTypeAmount;

    /* this needs to be array of length of ShapeTypeAmount */
    BlockProperties_t *BlockProperties;
  };

  void Open(
    const OpenProperties_t p
  ){
    KeyAmount = p.KeyAmount;
    __MemoryCopy(p.KeyInfos, KeyInfos, KeyAmount * sizeof(KeyInfos[0]));
    KeySizesSum = 0;
    for(uintptr_t i = 0; i < KeyAmount; i++){
      KeySizesSum += KeyInfos[i].Size;
    }

    ShapeTypeAmount = p.ShapeTypeAmount;

    Blocks = new Block_t[ShapeTypeAmount];
    for(ShapeTypeAmount_t i = 0; i < ShapeTypeAmount; i++){
      if(p.BlockProperties[i].MaxElementPerBlock > 0x100){
        __abort();
      }
      Blocks[i].MaxElementPerBlock_m1 = p.BlockProperties[i].MaxElementPerBlock - 1;
      Blocks[i].RenderDataSize = p.BlockProperties[i].RenderDataSize;
      Blocks[i].DataSize = p.BlockProperties[i].DataSize;

      Blocks[i].List.Open(
        (
          (uintptr_t)Blocks[i].RenderDataSize + Blocks[i].DataSize + sizeof(ShapeList_t::nr_t)
        ) * Blocks[i].MaxElementPerBlock() + sizeof(PerBlockData_t)
      );
    }

    kt.Open();
    kt_root = kt_NewNode(&kt);
    bm.Open(sizeof(bm_BaseData_t) + KeySizesSum);
    BlockQueue.Open();
    ShapeList.Open();

    fid.Open(this);
  }
  void Close(){
    fid.Close(this);

    ShapeList.Close();
    BlockQueue.Close();
    bm.Close();
    kt.Close();

    for(ShapeTypeAmount_t i = 0; i < ShapeTypeAmount; i++){
      Blocks[i].List.Close();
    }
  }

  ShapeID_t add(
    const void *KeyDataArray,
    ShapeTypeAmount_t ShapeType,
    const void *RenderData,
    const void *Data,
    BlockID_t *BlockID
  ){
    bm_NodeReference_t bmnr;
    bm_BaseData_t *bmbase;

    Block_t &Block = Blocks[ShapeType];

    auto _KeyDataArray = (KeyData_t *)KeyDataArray;

    kt_NodeReference_t nr = kt_root;
    for(KeyIndex_t KeyIndex = 0; KeyIndex < KeyAmount; KeyIndex++){
      Key_t::KeySize_t bdbt_ki;
      Key_t::q(&kt, KeyInfos[KeyIndex].sibit(), _KeyDataArray, &bdbt_ki, &nr);
      if(bdbt_ki != KeyInfos[KeyIndex].sibit()){
        /* query failed to find rest so lets make new block manager */

        bmnr = bm.NewNode();
        bmbase = (bm_BaseData_t *)bm[bmnr];
        bmbase->ShapeType = ShapeType;
        bmbase->FirstBlockNR = Block.List.NewNode();
        *BlockID = bmbase->FirstBlockNR;
        bmbase->LastBlockNR = bmbase->FirstBlockNR;
        bmbase->LastBlockElementCount = 0;
        __MemoryCopy(KeyDataArray, &bmbase[1], KeySizesSum);

        do{
          kt_NodeReference_t out;
          if(KeyIndex + 1 != KeyAmount){
            out = kt_NewNode(&kt);
          }
          else{
            out = *(kt_NodeReference_t *)&bmnr;
          }

          Key_t::a(&kt, KeyInfos[KeyIndex].sibit(), _KeyDataArray, bdbt_ki, nr, out);
          bdbt_ki = 0;

          nr = out;

          _KeyDataArray += KeyInfos[KeyIndex].Size;
        }while(++KeyIndex < KeyAmount);

        goto gt_NoNewBlockManager;
      }

      _KeyDataArray += KeyInfos[KeyIndex].Size;
    }

    bmnr = *(bm_NodeReference_t *)&nr;
    bmbase = (bm_BaseData_t *)bm[bmnr];

    if(bmbase->LastBlockElementCount == Block.MaxElementPerBlock_m1){
      bmbase->LastBlockElementCount = 0;
      auto blnr = Block.List.NewNode();
      *BlockID = blnr;
      Block.List.linkNextOfOrphan(bmbase->LastBlockNR, blnr);
      bmbase->LastBlockNR = blnr;
    }
    else{
      BlockID->sic();
      bmbase->LastBlockElementCount++;
    }

    gt_NoNewBlockManager:

    auto shapeid = ShapeList.NewNode();
    ShapeList[shapeid].bmid = bmnr;
    ShapeList[shapeid].blid = bmbase->LastBlockNR;
    ShapeList[shapeid].ElementIndex = bmbase->LastBlockElementCount;

    __MemoryCopy(
      RenderData,
      _GetRenderData(ShapeType, bmbase->LastBlockNR, bmbase->LastBlockElementCount),
      Block.RenderDataSize
    );
    __MemoryCopy(
      Data,
      _GetData(ShapeType, bmbase->LastBlockNR, bmbase->LastBlockElementCount),
      Block.DataSize
    );
    _GetShapeID(ShapeType, bmbase->LastBlockNR, bmbase->LastBlockElementCount) = shapeid;

    return shapeid;
  }

  void remove(
    ShapeList_t::nr_t shapeid,
    PerBlockData_t *PerBlockData
  ){
    bm_t::nr_t bmid;
    bm_BaseData_t *bmbase;
    ShapeTypeAmount_t ShapeType;

    Block_t *Block;

    {
      auto &shape = ShapeList[shapeid];
      bmid = shape.bmid;
      bmbase = (bm_BaseData_t *)bm[bmid];

      ShapeType = bmbase->ShapeType;
      Block = &Blocks[ShapeType];

      auto &lshape = ShapeList[
        _GetShapeID(ShapeType, bmbase->LastBlockNR, bmbase->LastBlockElementCount)
      ];

      __MemoryMove(
        _GetRenderData(ShapeType, shape.blid, shape.ElementIndex),
        _GetRenderData(ShapeType, lshape.blid, lshape.ElementIndex),
        Block->RenderDataSize
      );
      __MemoryMove(
        _GetData(ShapeType, shape.blid, shape.ElementIndex),
        _GetData(ShapeType, lshape.blid, lshape.ElementIndex),
        Block->DataSize
      );
      _GetShapeID(ShapeType, shape.blid, shape.ElementIndex) =
        _GetShapeID(ShapeType, lshape.blid, lshape.ElementIndex);

      lshape.blid = shape.blid;
      lshape.ElementIndex = shape.ElementIndex;

      ShapeList.Recycle(shapeid);
    }

    /* shapeid we deleted may be same as last so lets no longer access shape plus lshape */

    /* we just deleted last so lets check if we can just decrease count */
    if(bmbase->LastBlockElementCount != 0){
      bmbase->LastBlockElementCount--;
      PerBlockData->si();
      return;
    }
    
    /* looks like we deleted first element in block so we need to delete last block */
    /* before do that we need to be sure if that we dont delete first block in block manager */
    if(bmbase->FirstBlockNR != bmbase->LastBlockNR){
      auto blid = bmbase->LastBlockNR;
      bmbase->LastBlockNR = blid.Prev(&Block->List);
      *PerBlockData = GetPerBlockData(ShapeType, blid);
      Block->List.Recycle(blid);
      bmbase->LastBlockElementCount = Block->MaxElementPerBlock_m1;
      return;
    }

    /* good luck we need to remove block manager completely */
    /* aaaaaaaaaaaa */

    *PerBlockData = GetPerBlockData(ShapeType, bmbase->LastBlockNR);
    Block->List.Recycle(bmbase->LastBlockNR);

    auto KeyDataArray = (KeyData_t *)&bmbase[1];
    auto knr = kt_root;

    for(KeyIndex_t KeyIndex = 0; KeyIndex < KeyAmount; KeyIndex++){
      fid.knrs[KeyIndex] = knr;
      Key_t::cq(&kt, KeyInfos[KeyIndex].sibit(), KeyDataArray, &knr);
      KeyDataArray += KeyInfos[KeyIndex].Size;
    }

    /* TODO this part can be faster if used some different function instead of .r */
    for(KeyIndex_t KeyIndex = KeyAmount - 1;;){
      KeyDataArray -= KeyInfos[KeyIndex].Size;
      Key_t::r(&kt, KeyInfos[KeyIndex].sibit(), KeyDataArray, fid.knrs[KeyIndex]);
      if(kt_inrhc(&kt, fid.knrs[KeyIndex])){
        break;
      }
      if(KeyIndex == 0){
        break;
      }
      kt_Recycle(&kt, fid.knrs[KeyIndex]);
      KeyIndex--;
    }

    bm.Recycle(bmid);
  }

  struct KeyTraverse_t{
    KeyIndex_t KeyIndex;
    KeyData_t *KeyData;

    void Init(shaper_t &shaper){
      KeyIndex = (KeyIndex_t)-1;
      KeyData = shaper.fid.KeyDatas - shaper.KeyInfos->Size;
    }
    bool Loop(shaper_t &shaper){

      if(++KeyIndex == shaper.KeyAmount){
        --KeyIndex;
      }
      else{
        shaper.fid.tra[KeyIndex].i0(
          #ifndef shaper_set_MaxKeySize
            shaper.fid.ta[KeyIndex],
          #endif
          KeyIndex == 0 ? shaper.kt_root : shaper.fid.tra[KeyIndex - 1].Output,
          shaper.KeyInfos[KeyIndex].BitOrder
        );
        if(KeyIndex == 0){ /* TODO if is badddddddddddddddd */
          KeyData += shaper.KeyInfos[KeyIndex].Size;
        }
        else{
          KeyData += shaper.KeyInfos[KeyIndex - 1].Size;
        }
      }

      gt_tra:

      bool r = shaper.fid.tra[KeyIndex].t0(
        &shaper.kt,
        #ifndef shaper_set_MaxKeySize
          shaper.fid.ta[KeyIndex],
        #endif
        shaper.KeyInfos[KeyIndex].Size * 8,
        KeyData,
        shaper.KeyInfos[KeyIndex].BitOrder
      );

      if(r == false){
        if(KeyIndex == 0){
          return false;
        }
        KeyIndex--;
        KeyData -= shaper.KeyInfos[KeyIndex].Size;
        goto gt_tra;
      }

      return true;
    }
    bmid_t Get(shaper_t &shaper){
      return *(bmid_t *)&shaper.fid.tra[shaper.KeyAmount - 1].Output;
    }
  };

  struct BlockTraverse_t{
    private:

    ShapeTypeAmount_t ShapeType;
    BlockList_t::nr_t From;
    BlockList_t::nr_t To;
    ElementIndexInBlock_t LastBlockElementCount;

    public:

    ShapeTypeAmount_t Init(shaper_t &shaper, bmid_t bmid){
      bm_BaseData_t *bmbase = (bm_BaseData_t *)shaper.bm[bmid];
      ShapeType = bmbase->ShapeType;
      From = bmbase->FirstBlockNR;
      To = bmbase->LastBlockNR;
      LastBlockElementCount = bmbase->LastBlockElementCount;
      return ShapeType;
    }
    bool Loop(shaper_t &shaper){
      if(From == To){
        return false;
      }
      From = From.Next(&shaper.Blocks[ShapeType].List);
      return true;
    }
    MaxElementPerBlock_t GetAmount(shaper_t &shaper){
      if(From == To){
        return (MaxElementPerBlock_t)LastBlockElementCount + 1;
      }
      return shaper.Blocks[ShapeType].MaxElementPerBlock();
    }
    void *GetRenderData(shaper_t &shaper){
      return shaper._GetRenderData(ShapeType, From, 0);
    }
    void *GetData(shaper_t &shaper){
      return shaper._GetData(ShapeType, From, 0);
    }
  };
};

#undef shaper_set_MaxShapeDataSize
#undef shaper_set_MaxShapeRenderDataSize
#undef shaper_set_MaxKeySizesSum
#undef shaper_set_MaxMaxElementPerBlock
#undef shaper_set_MaxKeySize
#undef shaper_set_MaxKeyAmount
#undef shaper_set_MaxShapeTypes
