/*
 *               MBRTest     Test MBR code      v00.10.00
 *
 *                   Author: Benjamin David Lunt
 *                     Forever Young Software
 *                     Copyright (c) 1984-2017
 *  
 *  This source code is included and purposed for the testing of MBR code.
 *   Please see license.txt for more information.
 *  
 *  You may modify and use it in your own projects as long as they are
 *   for non profit only and not distributed.  Any project for profit that 
 *   uses this code must have written permission from the author.
 *
 *  Please see http://www.fysnet.net/mtools.htm for the original source of
 *   this package.
 *
 *
 *  To compile using DJGPP:  (http://www.delorie.com/djgpp/)
 *    gcc -Os mbrtest.c -o mbrtest.exe -s  (DOS .EXE requiring DPMI)
 * 
 *  Compiles as is with MS VC++ 6.x         (Win32 .EXE file)
 * 
 *  Compiles as is with MS QC2.5            (TRUE DOS only)  ??????????????
 *  
 * Usage:
 *   mbrtest filename.img mbr.bin
 *
 */

#ifdef _MSC_VER
  #if (_MSC_VER > 1000)
    #pragma warning(disable: 4103)
    #define _CRT_SECURE_NO_WARNINGS
    #define PUTCH(x) _putch(x)
  #else
    #define PUTCH(x) putch(x)
  #endif
#else
  #define PUTCH(x) putch(x)
#endif

// depending on the compiler, the include file is in a different directory
#ifdef _MSC_VER
  #if (_MSC_VER > 1000)  // MS Quick C is version 1000 (????)
    #include "../ctype.h"
  #else
    #include "../../ctype.h"
  #endif
#elif defined(DJGPP)
  #include "../../ctype.h"
#endif

#include <ctype.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbrtest.h"   // our include

#define PART_CNT         3
#define TYPE_DUMMY     'S'   // I chose the Simple File System id, just because I am currently working on my SFS implementation
#define TYPE_SM_EXT   0x05
#define TYPE_LG_EXT   0x0F
#define ACTIVE        0x80
#define NON_ACTIVE    0x00

struct PARTS {
  bit32u lba;
  struct ENTRIES {
    bool   active;
    bit8u  type;
    bit32u base;
  } entries[4];
} parts[PART_CNT] = {
  // LBA 0 is the MBR
  { 0,
    {
      { 
        0,            // active ?
        TYPE_DUMMY,   // type
        1             // base LBA
      },
      { 
        0,            // active ?
        TYPE_LG_EXT,  // type
        2             // base LBA 2
      },
      { 
        0,            // active ?
        0,            // type
        0             // base LBA
      },
      { 
        1,            // active ?
        TYPE_DUMMY,   // type
        7             // base LBA
      },
    }
  },
  // LBA 2 is the an extended mbr
  { 2,
    {
      { 
        0,            // active ?
        TYPE_DUMMY,   // type
        3             // base LBA 3
      },
      { 
        0,            // active ?
        TYPE_DUMMY,   // type
        4             // base LBA 4
      },
      { 
        0,            // active ?
        TYPE_LG_EXT,  // type
        8             // base LBA 8
      },
      { 
        0,            // active ?
        TYPE_DUMMY,   // type
        6             // base LBA 6
      },
    }
  },
  // LBA 8 is the an extended mbr
  { 8,
    {
      { 
        0,            // active ?
        TYPE_DUMMY,   // type
        9             // base LBA
      },
      { 
        0,            // active ?
        TYPE_DUMMY,   // type
        10            // base LBA
      },
      { 
        0,            // active ?
        TYPE_DUMMY,   // type
        11            // base LBA
      },
      { 
        0,            // active ?
        TYPE_DUMMY,   // type
        12            // base LBA
      },
    }
  },
  
};

int main(int argc, char *argv[]) {
  struct PART_TBLE *table;
  int i, j;
  bit8u test_mbr[512];
  bit8u test_vbr[512];
  bit8u buffer[512];
  FILE *fp, *targ;
  char  filename[80];     // filename
  char  bootname[80];     // boot filename
  
  // print start string
  printf(strtstr);
  
  strcpy(filename, argv[1]);
  strcpy(bootname, argv[2]);
  
  // create target file
  if ((targ = fopen(filename, "w+b")) == NULL) {
    printf("Error creating file [%s]\n", filename);
    return 1;
  }
  
  // open test boot binary file
  if ((fp = fopen(bootname, "rb")) == NULL) {
    printf("Error opening file [%s]\n", bootname);
    fclose(targ);
    return 2;
  }
  
  // read in the test boot bin file
  if (fread(test_mbr, 1, 512, fp) < 512) {
    printf("Error reading from file [%s]\n", bootname);
    fclose(targ);
    fclose(fp);
    return 3;
  }
  // close the source file
  fclose(fp);
  
  // open our temp vbr file
  if ((fp = fopen("test.bin", "rb")) == NULL) {
    printf("Error opening test.bin file\n");
    fclose(targ);
    return 4;
  }
  // read in the test vbr bin file, and close it
  fread(test_vbr, 1, 512, fp);
  fclose(fp);
  
  // now go through all the part entries and build the image.
  for (j=0; j<PART_CNT; j++) {
    if (parts[j].lba == 0)
      memcpy(buffer, test_mbr, 512);
    else {
      memset(buffer, 0, 510);
      buffer[510] = 0x55;
      buffer[511] = 0xAA;
    }
    table = (struct PART_TBLE *) &buffer[512-2-(sizeof(struct PART_TBLE)*4)];
    for (i=0; i<4; i++) {
      memset(&table[i], 0, sizeof(struct PART_TBLE));
      table[i].bi = (parts[j].entries[i].active) ? ACTIVE : NON_ACTIVE;
      table[i].si = parts[j].entries[i].type;
      table[i].startlba = (parts[j].entries[i].base - parts[j].lba);
      switch (parts[j].entries[i].type) {
        case 0:
          break;
        case TYPE_SM_EXT:
        case TYPE_LG_EXT:
          break;
        default:
          memset(&test_vbr[256], 0, 80);
          sprintf((char *) &test_vbr[256], "Loaded VBR at lba %i", parts[j].entries[i].base);
          fseek(targ, parts[j].entries[i].base * 512, SEEK_SET);
          fwrite(test_vbr, 1, 512, targ);
          break;
      }
    }
    fseek(targ, parts[j].lba * 512, SEEK_SET);
    fwrite(buffer, 1, 512, targ);
  }
  
  fclose(targ);
  return 0;
}
