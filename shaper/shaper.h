struct shaper_t{
  public: /* -------------------------------------------------------------------------------- */

  typedef uint8_t ShapeTypeAmount_t;

  private: /* ------------------------------------------------------------------------------- */

  /* key tree and block manager node reference type */
  typedef uint16_t ktbmnr_t;

  /* block list node reference */
  /* total element a shape can have is = (pow(2, (sizeof(blnr_t) * 8)) * MaxElementPerBlock) */
  typedef uint16_t blnr_t;

  #define BDBT_set_prefix kt
  #define BDBT_set_type_node ktbmnr_t
  #define BDBT_set_lcpp
  #define BDBT_set_KeySize 0
  #ifdef shaper_set_MaxKeySize
    #define BDBT_set_MaxKeySize shaper_set_MaxKeySize
  #endif
  #define BDBT_set_AreWeInsideStruct 1
  #include <BDBT/BDBT.h>
  kt_t kt;
  kt_NodeReference_t kt_root;
  typedef kt_Key_t Key_t;

  public: /* -------------------------------------------------------------------------------- */

  #pragma pack(push, 1)
    struct KeyInfo_t{
      uint8_t Size;
      Key_t::BitOrder_t BitOrder;
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
      (RenderDataSize + DataSize + sizeof(ShapeList_t::nr_t)) * (MaxElementPerBlock + 1) +
      sizeof(PerBlockData_t)
    */
    BlockList_t List;

    uint8_t MaxElementPerBlock; /* (p.MaxElementPerBlock[n] - 1) */
    uint16_t RenderDataSize;
    uint16_t DataSize;
  }*Blocks;

  #pragma pack(push, 1)
    struct bm_BaseData_t{
      ShapeTypeAmount_t ShapeType;
      BlockList_NodeReference_t FirstBlockNR;
      BlockList_NodeReference_t LastBlockNR;
      uint8_t LastBlockElementCount;
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
      uint8_t ElementIndex;
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

  typedef uint8_t KeyIndex_t;

  KeyIndex_t KeyAmount;
  KeyInfo_t KeyInfos[8];
  uint8_t KeySizesSum;
  ShapeTypeAmount_t ShapeTypeAmount;

  /* function internal datas */
  struct fid_t{
    union{
      uint8_t *base;
      /* used in remove */
      kt_NodeReference_t *knrs;
      /* used in traverse for store keys */
      uint8_t *KeyDatas;
    };

    Key_t::Traverse_t *tra;
    #ifndef shaper_set_MaxKeySize
      Key_t::Traverse_t::ta_t **ta;
    #endif

    void Open(const shaper_t *shaper){
      base = new uint8_t[
        sizeof(kt_NodeReference_t) * shaper->KeyAmount > shaper->KeySizesSum ? 
        sizeof(kt_NodeReference_t) * shaper->KeyAmount :
        shaper->KeySizesSum
      ];

      tra = new Key_t::Traverse_t[shaper->KeyAmount];

      #ifndef shaper_set_MaxKeySize
        ta = new Key_t::Traverse_t::ta_t *[shaper->KeyAmount];
        for(KeyIndex_t KeyIndex = 0; KeyIndex < shaper->KeyAmount; KeyIndex++){
          ta[KeyIndex] = new Key_t::Traverse_t::ta_t[
            Key_t::Traverse_t::GetTraverseArraySize(shaper->KeyInfos[KeyIndex].Size * 8)
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

      delete base;
    }
  }fid;

  /*
    block data format is this. %30
    RenderData   Data         ShapeID
    [|||       ] [|||       ] [|||       ]
  */

  /* TODO those are made uintptr_t to not overflow, maybe there is better way? */
  uint8_t *_GetRenderData(
    ShapeTypeAmount_t ShapeType,
    BlockList_t::nr_t blid,
    uintptr_t ElementIndex
  ){
    return &((uint8_t *)Blocks[ShapeType].List[blid])[
      Blocks[ShapeType].RenderDataSize * ElementIndex
    ];
  }
  uint8_t *_GetData(
    ShapeTypeAmount_t ShapeType,
    BlockList_t::nr_t blid,
    uintptr_t ElementIndex
  ){
    return &_GetRenderData(
      ShapeType,
      blid,
      (uintptr_t)Blocks[ShapeType].MaxElementPerBlock + 1
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
      (uintptr_t)Blocks[ShapeType].MaxElementPerBlock + 1
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

  uint8_t *GetRenderData(
    ShapeID_t ShapeID
  ){
    auto &shape = ShapeList[ShapeID];
    auto bmbase = (bm_BaseData_t *)bm[shape.bmid];
    return _GetRenderData(bmbase->ShapeType, shape.blid, shape.ElementIndex);
  }
  uint8_t *GetData(
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
      (uintptr_t)Blocks[ShapeType].MaxElementPerBlock + 1
    );
  }

  struct BlockProperties_t{
    uint16_t MaxElementPerBlock;
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
      Blocks[i].MaxElementPerBlock = p.BlockProperties[i].MaxElementPerBlock - 1;
      Blocks[i].RenderDataSize = p.BlockProperties[i].RenderDataSize;
      Blocks[i].DataSize = p.BlockProperties[i].DataSize;

      Blocks[i].List.Open(
        (
          Blocks[i].RenderDataSize + Blocks[i].DataSize + sizeof(ShapeList_t::nr_t)
        ) * (Blocks[i].MaxElementPerBlock + 1) + sizeof(PerBlockData_t)
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

    auto _KeyDataArray = (uint8_t *)KeyDataArray;

    kt_NodeReference_t nr = kt_root;
    for(KeyIndex_t KeyIndex = 0; KeyIndex < KeyAmount; KeyIndex++){
      Key_t::KeySize_t bdbt_ki;
      Key_t::q(&kt, KeyInfos[KeyIndex].Size * 8, _KeyDataArray, &bdbt_ki, &nr);
      if(bdbt_ki != KeyInfos[KeyIndex].Size * 8){
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

          Key_t::a(&kt, KeyInfos[KeyIndex].Size * 8, _KeyDataArray, bdbt_ki, nr, out);
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

    if(bmbase->LastBlockElementCount == Block.MaxElementPerBlock){
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
      bmbase->LastBlockElementCount = Block->MaxElementPerBlock;
      return;
    }

    /* good luck we need to remove block manager completely */
    /* aaaaaaaaaaaa */

    *PerBlockData = GetPerBlockData(ShapeType, bmbase->LastBlockNR);
    Block->List.Recycle(bmbase->LastBlockNR);

    auto KeyDataArray = (uint8_t *)&bmbase[1];
    auto knr = kt_root;

    for(KeyIndex_t KeyIndex = 0; KeyIndex < KeyAmount; KeyIndex++){
      fid.knrs[KeyIndex] = knr;
      Key_t::cq(&kt, KeyInfos[KeyIndex].Size * 8, KeyDataArray, &knr);
      KeyDataArray += KeyInfos[KeyIndex].Size;
    }

    /* TODO this part can be faster if used some different function instead of .r */
    for(KeyIndex_t KeyIndex = KeyAmount - 1;;){
      KeyDataArray -= KeyInfos[KeyIndex].Size;
      Key_t::r(&kt, KeyInfos[KeyIndex].Size * 8, KeyDataArray, fid.knrs[KeyIndex]);
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
    uint8_t *KeyData;

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
    uint8_t LastBlockElementCount;

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
    uint16_t GetAmount(shaper_t &shaper){ /* TODO uint16_t is bad */
      if(From == To){
        return LastBlockElementCount;
      }
      return shaper.Blocks[ShapeType].MaxElementPerBlock;
    }
    void *GetRenderData(shaper_t &shaper){
      return shaper._GetRenderData(ShapeType, From, 0);
    }
    void *GetData(shaper_t &shaper){
      return shaper._GetData(ShapeType, From, 0);
    }
  };
};

#ifdef shaper_set_MaxKeySize
  #undef shaper_set_MaxKeySize
#endif
