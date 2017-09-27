
char strtstr[] = "\nMTOOLS   Test MBR  v00.10.00    Forever Young Software 1984-2017\n\n";

// set it to 1 (align on byte)
#ifdef _MSC_VER
  #if (_MSC_VER > 1000)
    #pragma pack(push, 1)
  #else
    #pragma pack(1)
  #endif
#elif defined(DJGPP)
  #pragma pack(push, 1)
#endif

struct PART_TBLE {
  bit8u  bi;
  bit8u  s_head;    // 8 bit head count
  bit8u  s_sector;  // hi 2 bits is hi 2 bits of cyl, bottom 6 bits is sector
  bit8u  s_cyl;     // bottom 8 bits
  bit8u  si;
  bit8u  e_head;    // 8 bit head count
  bit8u  e_sector;  // hi 2 bits is hi 2 bits of cyl, bottom 6 bits is sector
  bit8u  e_cyl;     // bottom 8 bits
  bit32u startlba;
  bit32u size;
};

#ifdef _MSC_VER
  #if (_MSC_VER > 1000)
    #pragma pack(pop)
  #endif
#elif defined(DJGPP)
  #pragma pack(pop)
#endif
