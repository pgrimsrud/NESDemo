
// Controller bits
#define BUTTON_A 0x80
#define BUTTON_B 0x40
#define BUTTON_SELECT 0x20
#define BUTTON_START 0x10
#define BUTTON_UP 0x08
#define BUTTON_DOWN 0x04
#define BUTTON_LEFT 0x02
#define BUTTON_RIGHT 0x01

// Registers
#define PPU_ADDRESS *((unsigned char*)0x2006)
#define PPU_DATA    *((unsigned char*)0x2007)

#pragma bss-name (push, "OAM")
unsigned char Sprites[256];
#pragma bss-name (pop)

#pragma bss-name (push, "BSS")

static unsigned char gController1;
static unsigned char gX;
static unsigned char gY;
static unsigned long gXScroll;
static unsigned char gYScroll;
static unsigned char gYNametable;
static unsigned char devnull;
static unsigned int  i;
static unsigned int  gVelocity;
static unsigned int  gVelocityDirection;
//static unsigned int  j;
//static unsigned long offset;
static unsigned char ppu_row;

extern void pMusicInit(unsigned char);
extern void pMusicPlay(void);

void __fastcall__ UnRLE(const unsigned char *data);

#include "Background/Level1Top.h"
#include "Background/Level1Bottom.h"

#pragma bss-name (pop)

#pragma data-name (push, "RODATA")

// 16 x (15 + 15)
// TODO should use a bit per block instead of byte, but this is a lot easier at the moment
unsigned char collision[480] = {
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};

unsigned char frog[4] = {0x00,0x01,0x02,0x03,};
unsigned char bird[2] = {0x04,0x05,};

#pragma data-name (pop)

#pragma data-name ("CHARS")

unsigned char pattern[176] = {0x00,0x00,0x00,0x01,0x01,0x07,0x09,0x17,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x08, // frog
                              0x60,0xD0,0xA8,0xA8,0x86,0x3E,0xF3,0x7C,0x00,0x20,0x70,0x70,0x78,0xC0,0x0C,0x80, // frog
                              0x2F,0x59,0x7E,0xF6,0xEC,0x9C,0x38,0x3F,0x10,0x20,0x00,0x00,0x00,0x01,0x00,0x00, // frog
                              0x60,0xC0,0xC0,0xC0,0xE6,0x7C,0x1E,0x80,0x80,0x00,0x18,0x10,0x00,0x00,0x00,0x00, // frog
                              0x18,0x2C,0xF9,0x03,0x03,0x01,0x01,0x00,0x00,0x00,0xE6,0x04,0x00,0x00,0x00,0x00, // bird
                              0x00,0x00,0xF0,0xFE,0xFF,0xDF,0xC0,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x0E, // bird
                              0xFF,0x00,0x78,0x40,0x5F,0x1F,0x3F,0x00,0x00,0xFF,0xFF,0xFF,0xE0,0xE0,0xC0,0xFF,0xFF,0x03,0xFF,0x0F,0xEF,0xE3,0x8F,0x3F,0x00,0xFC,0x00,0xF0,0x10,0x1C,0x70,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x38,0x78,0x58,0x18,0x18,0x7E,0x00,0x18,0x38,0x78,0x58,0x18,0x18,0x7E,0x00,0x3C,0x66,0x06,0x1E,0x38,0x70,0x7E,0x00,0x3C,0x66,0x06,0x1E,0x38,0x70,0x7E,0x00,};

void vblank(void)
{
    while((*((unsigned char*)0x2002) & 0x80) != 0x00);
    while((*((unsigned char*)0x2002) & 0x80) != 0x80);
}

void ppuinit(void)
{
    // from NESDev:
    // +---------+----------------------------------------------------------+
    // |  $2000  | PPU Control Register #1 (W)                              |
    // |         |                                                          |
    // |         |    D7: Execute NMI on VBlank                             |
    // |         |           0 = Disabled                                   |
    // |         |           1 = Enabled                                    |
    // |         |    D6: PPU Master/Slave Selection --+                    |
    // |         |           0 = Master                +-- UNUSED           |
    // |         |           1 = Slave               --+                    |
    // |         |    D5: Sprite Size                                       |
    // |         |           0 = 8x8                                        |
    // |         |           1 = 8x16                                       |
    // |         |    D4: Background Pattern Table Address                  |
    // |         |           0 = $0000 (VRAM)                               |
    // |         |           1 = $1000 (VRAM)                               |
    // |         |    D3: Sprite Pattern Table Address                      |
    // |         |           0 = $0000 (VRAM)                               |
    // |         |           1 = $1000 (VRAM)                               |
    // |         |    D2: PPU Address Increment                             |
    // |         |           0 = Increment by 1                             |
    // |         |           1 = Increment by 32                            |
    // |         | D1-D0: Name Table Address                                |
    // |         |         00 = $2000 (VRAM)                                |
    // |         |         01 = $2400 (VRAM)                                |
    // |         |         10 = $2800 (VRAM)                                |
    // |         |         11 = $2C00 (VRAM)                                |
    // +---------+----------------------------------------------------------+
    // +---------+----------------------------------------------------------+
    // |  $2001  | PPU Control Register #2 (W)                              |
    // |         |                                                          |
    // |         | D7-D5: Full Background Colour (when D0 == 1)             |
    // |         |         000 = None  +------------+                       |
    // |         |         001 = Green              | NOTE: Do not use more |
    // |         |         010 = Blue               |       than one type   |
    // |         |         100 = Red   +------------+                       |
    // |         | D7-D5: Colour Intensity (when D0 == 0)                   |
    // |         |         000 = None            +--+                       |
    // |         |         001 = Intensify green    | NOTE: Do not use more |
    // |         |         010 = Intensify blue     |       than one type   |
    // |         |         100 = Intensify red   +--+                       |
    // |         |    D4: Sprite Visibility                                 |
    // |         |           0 = Sprites not displayed                      |
    // |         |           1 = Sprites visible                            |
    // |         |    D3: Background Visibility                             |
    // |         |           0 = Background not displayed                   |
    // |         |           1 = Background visible                         |
    // |         |    D2: Sprite Clipping                                   |
    // |         |           0 = Sprites invisible in left 8-pixel column   |
    // |         |           1 = No clipping                                |
    // |         |    D1: Background Clipping                               |
    // |         |           0 = BG invisible in left 8-pixel column        |
    // |         |           1 = No clipping                                |
    // |         |    D0: Display Type                                      |
    // |         |           0 = Colour display                             |
    // |         |           1 = Monochrome display                         |
    // +---------+----------------------------------------------------------+

    // Set PPU registers
    *((unsigned char*)0x2000) = 0x84;
    *((unsigned char*)0x2001) = 0x1E;
}

void ppudisable(void)
{
    // Disable graphics
    *((unsigned char*)0x2000) = 0x00;
    *((unsigned char*)0x2001) = 0x00;
}

void apuinit(void)
{
    // Initialize the APU to a known basic state
    *((unsigned char*)0x4015) = 0x0F;

    *((unsigned char*)0x4000) = 0x30;
    *((unsigned char*)0x4001) = 0x08;
    *((unsigned char*)0x4002) = 0x00;
    *((unsigned char*)0x4003) = 0x00;
    *((unsigned char*)0x4004) = 0x30;
    *((unsigned char*)0x4005) = 0x08;
    *((unsigned char*)0x4006) = 0x00;
    *((unsigned char*)0x4007) = 0x00;
    *((unsigned char*)0x4008) = 0x80;
    *((unsigned char*)0x4009) = 0x00;
    *((unsigned char*)0x400A) = 0x00;
    *((unsigned char*)0x400B) = 0x00;
    *((unsigned char*)0x400C) = 0x30;
    *((unsigned char*)0x400D) = 0x00;
    *((unsigned char*)0x400E) = 0x00;
    *((unsigned char*)0x400F) = 0x00;
    *((unsigned char*)0x4010) = 0x00;
    *((unsigned char*)0x4011) = 0x00;
    *((unsigned char*)0x4012) = 0x00;
    *((unsigned char*)0x4013) = 0x00;
    *((unsigned char*)0x4014) = 0x00;
    *((unsigned char*)0x4015) = 0x0F;
    *((unsigned char*)0x4016) = 0x00;
    *((unsigned char*)0x4017) = 0x40;
}

void palattes(void)
{
    // Background 0
    PPU_ADDRESS = 0x3F;
    PPU_ADDRESS = 0x00;
    PPU_DATA =    0x0E;
    PPU_ADDRESS = 0x3F;
    PPU_ADDRESS = 0x01;
    PPU_DATA    = 0x0A;
    PPU_ADDRESS = 0x3F;
    PPU_ADDRESS = 0x02;
    PPU_DATA    = 0x1A;
    PPU_ADDRESS = 0x3F;
    PPU_ADDRESS = 0x03;
    PPU_DATA    = 0x08;

    // Background 1
    PPU_ADDRESS = 0x3F;
    PPU_ADDRESS = 0x04;
    PPU_DATA =    0x0E;
    PPU_ADDRESS = 0x3F;
    PPU_ADDRESS = 0x05;
    PPU_DATA    = 0x00;
    PPU_ADDRESS = 0x3F;
    PPU_ADDRESS = 0x06;
    PPU_DATA    = 0x10;
    PPU_ADDRESS = 0x3F;
    PPU_ADDRESS = 0x07;
    PPU_DATA    = 0x20;


    // Sprite 0
    PPU_ADDRESS = 0x3F;
    PPU_ADDRESS = 0x11;
    PPU_DATA    = 0x0A;
    PPU_ADDRESS = 0x3F;
    PPU_ADDRESS = 0x12;
    PPU_DATA    = 0x1A;
    PPU_ADDRESS = 0x3F;
    PPU_ADDRESS = 0x13;
    PPU_DATA    = 0x08;

    // Sprite 1
    PPU_ADDRESS = 0x3F;
    PPU_ADDRESS = 0x15;
    PPU_DATA    = 0x11;
    PPU_ADDRESS = 0x3F;
    PPU_ADDRESS = 0x16;
    PPU_DATA    = 0x02;
    PPU_ADDRESS = 0x3F;
    PPU_ADDRESS = 0x17;
    PPU_DATA    = 0x27;
}

//void ppu_copy( unsigned char* addr, unsigned int len )
//{
//    for( i = 0; i < len; i++ )
//    {
//        PPU_ADDRESS = 0x20;
//        PPU_ADDRESS = i;
//        PPU_DATA = ((unsigned char*) addr)[i];
//    }
//}

void patterntables(void)
{
    //for( i = 0; i < 768; i++ )
    //{
    //    PPU_ADDRESS = 0x00;
    //    PPU_ADDRESS = 0;
    //    //PPU_DATA = j;//((unsigned char*) addr)[i];
    //    PPU_DATA = ((unsigned char*) pattern)[i];
    //    PPU_ADDRESS = 0x10;
    //    PPU_ADDRESS = 0;
    //    //PPU_DATA = j;//((unsigned char*) addr)[i];
    //    PPU_DATA = ((unsigned char*) pattern)[i];
    //}
    //PPU_ADDRESS = 0x00;
    //PPU_ADDRESS = 0;
    //for( i = 0; i < 1024; i++ )
    //{
    //    //PPU_DATA = pattern[i];
    //    PPU_DATA = i;//pattern[i];
    //}
}

void sprites(void)
{
    unsigned int i;

    // clear sprite ram
    for( i = 0; i < 256; i++ )
    {
        *((unsigned char*)(0x200 + i)) = 0x00;
    }

    // From NESDev:
    //+------+----------+--------------------------------------+
    //+ Byte | Bits     | Description                          |
    //+------+----------+--------------------------------------+
    //|  0   | YYYYYYYY | Y Coordinate - 1. Consider the coor- |
    //|      |          | dinate the upper-left corner of the  |
    //|      |          | sprite itself.                       |
    //|  1   | IIIIIIII | Tile Index #                         |
    //|  2   | vhp000cc | Attributes                           |
    //|      |          |   v = Vertical Flip   (1=Flip)       |
    //|      |          |   h = Horizontal Flip (1=Flip)       |
    //|      |          |   p = Background Priority            |
    //|      |          |         0 = In front                 |
    //|      |          |         1 = Behind                   |
    //|      |          |   c = Upper two (2) bits of colour   |
    //|  3   | XXXXXXXX | X Coordinate (upper-left corner)     |
    //+------+----------+--------------------------------------+}

    *((unsigned char*)0x200) = 0x50;
    *((unsigned char*)0x201) = 0x00;
    *((unsigned char*)0x202) = 0x00;
    *((unsigned char*)0x203) = 0x50;

    *((unsigned char*)0x204) = 0x50;
    *((unsigned char*)0x205) = 0x01;
    *((unsigned char*)0x206) = 0x00;
    *((unsigned char*)0x207) = 0x58;

    *((unsigned char*)0x208) = 0x58;
    *((unsigned char*)0x209) = 0x02;
    *((unsigned char*)0x20A) = 0x00;
    *((unsigned char*)0x20B) = 0x50;

    *((unsigned char*)0x20C) = 0x58;
    *((unsigned char*)0x20D) = 0x03;
    *((unsigned char*)0x20E) = 0x00;
    *((unsigned char*)0x20F) = 0x58;

    gX = 0x40;
    gY = 0x50;
    gVelocity = 0;
    gVelocityDirection = 0;
    gXScroll = 0;
    gYScroll = 0;
    gYNametable = 2;

    *((unsigned char*)0x210) = 0x30;
    *((unsigned char*)0x211) = 0x04;
    *((unsigned char*)0x212) = 0x01;
    *((unsigned char*)0x213) = 0x50;

    *((unsigned char*)0x214) = 0x30;
    *((unsigned char*)0x215) = 0x05;
    *((unsigned char*)0x216) = 0x01;
    *((unsigned char*)0x217) = 0x58;
}

void dma_sprites(void)
{
    *((unsigned char*)0x4013) = 0x00;
    *((unsigned char*)0x4014) = 0x02;
}

void input_poll(void)
{
    // Strobe 0x4016 bit 0 to gather input
    *((unsigned char*)0x4016) |= 0x01;
    *((unsigned char*)0x4016) &= 0xFE;

    gController1 = 0x00;

     if(*((unsigned char*)0x4016) & 0x01)
     {
         gController1 |= BUTTON_A;
     }
     if(*((unsigned char*)0x4016) & 0x01)
     {
         gController1 |= BUTTON_B;
     }
     if(*((unsigned char*)0x4016) & 0x01)
     {
         gController1 |= BUTTON_SELECT;
     }
     if(*((unsigned char*)0x4016) & 0x01)
     {
         gController1 |= BUTTON_START;
     }
     if(*((unsigned char*)0x4016) & 0x01)
     {
         gController1 |= BUTTON_UP;
     }
     if(*((unsigned char*)0x4016) & 0x01)
     {
         gController1 |= BUTTON_DOWN;
     }
     if(*((unsigned char*)0x4016) & 0x01)
     {
         gController1 |= BUTTON_LEFT;
     }
     if(*((unsigned char*)0x4016) & 0x01)
     {
         gController1 |= BUTTON_RIGHT;
     }

}

void update_sprites(void)
{
    if(gController1 & BUTTON_UP)
    {
    }
    if(gController1 & BUTTON_DOWN)
    {
    }
    if(gController1 & BUTTON_LEFT)
    {
        if(gX > 0x08)
        {

            if( gYNametable == 2 )
            {
              //x = gX >> 4;
              //y = gY >> 4
              //index[y*16 + x]
                if( collision[240 + (((gY+1)&0xF0)) + ((gX-1) >> 4)] == 0 &&
                    collision[240 + (((gY+0x10)&0xF0)) + ((gX-1) >> 4)] == 0 )
                {
                    gX -= 1;
                }
            }
            else
            {
                if((gYScroll + gY + 1) >= 0xF0)
                {
                    if( collision[240 + (((gYScroll + gY + 1 - 0xF0) & 0xF0) ) + ((gX-1) >> 4)] == 0 &&
                        collision[240 + (((gYScroll + gY + 0x10 - 0xF0) & 0xF0) ) + ((gX-1) >> 4)] == 0 )
                    {
                        gX -= 1;
                    }
                }
                else
                {
                    if( collision[(((gYScroll + gY + 1) & 0xF0) ) + ((gX-1) >> 4)] == 0 &&
                        collision[(((gYScroll + gY + 0x10) & 0xF0) ) + ((gX-1) >> 4)] == 0 )
                    {
                        gX -= 1;
                    }
                }
            }
            //gX -= 1;
        }
    }
    if(gController1 & BUTTON_RIGHT)
    {
        if(gX < 0xE8)
        {
            if( gYNametable == 2 )
            {
                if( collision[240 + (((gY+1)&0xF0) ) + ((gX+0x10) >> 4)] == 0 &&
                    collision[240 + (((gY+0x10)&0xF0) ) + ((gX+0x10) >> 4)] == 0 )
                {
                    gX += 1;
                }
            }
            else
            {
                if((gYScroll + gY + 1) >= 0xF0)
                {
                    if( collision[240 + (((gYScroll + gY + 1 - 0xF0) & 0xF0) ) + ((gX+0x10) >> 4)] == 0 &&
                        collision[240 + (((gYScroll + gY + 0x10 - 0xF0) & 0xF0) ) + ((gX+0x10) >> 4)] == 0 )
                    {
                        gX += 1;
                    }
                }
                else
                {
                    if( collision[(((gYScroll + gY + 1) & 0xF0) ) + ((gX+0x10) >> 4)] == 0 &&
                        collision[(((gYScroll + gY + 0x10) & 0xF0) ) + ((gX+0x10) >> 4)] == 0 )
                    {
                        gX += 1;
                    }
                }
            }
            //gX += 1;
        }
    }
    if(gController1 & BUTTON_B)
    {
      // Currently does nothing
    }
    if(gController1 & BUTTON_A)
    {
        ////raw period = 111860.8/frequency - 1
        //
        //// Make a sound
        //
        //// raw period
        //*((unsigned char*)0x4003) = 0x01;
        //*((unsigned char*)0x4002) = 0x17;
        //// duty cycle and volume
        //*((unsigned char*)0x4000) = 0xBF;
        gVelocity = 16;
        gVelocityDirection = 1;
    }
    if(!((gController1 & BUTTON_A) || (gController1 & BUTTON_B)))
    {
        // Stop making the sound
        //*((unsigned char*)0x4000) = 0x30;
    }

    *((unsigned char*)0x200) = gY;
    *((unsigned char*)0x203) = gX;
    *((unsigned char*)0x204) = gY;
    *((unsigned char*)0x207) = gX+8;
    *((unsigned char*)0x208) = gY+8;
    *((unsigned char*)0x20B) = gX;
    *((unsigned char*)0x20C) = gY+8;
    *((unsigned char*)0x20F) = gX+8;
}

void do_physics(void)
{
    if( *((unsigned char*)0x210) != gY )
    {
        if( *((unsigned char*)0x210) < gY )
        {
            *((unsigned char*)0x210) += 1;
            *((unsigned char*)0x214) += 1;
        }
        else
        {
            *((unsigned char*)0x210) -= 1;
            *((unsigned char*)0x214) -= 1;
        }
    }
    if( *((unsigned char*)0x213) != gX )
    {
        if( *((unsigned char*)0x213) < gX )
        {
            *((unsigned char*)0x213) += 1;
            *((unsigned char*)0x217) += 1;

            *((unsigned char*)0x211) = 0x05;
            *((unsigned char*)0x212) = 0x41;
            *((unsigned char*)0x215) = 0x04;
            *((unsigned char*)0x216) = 0x41;
        }
        else
        {
            *((unsigned char*)0x213) -= 1;
            *((unsigned char*)0x217) -= 1;

            *((unsigned char*)0x211) = 0x04;
            *((unsigned char*)0x212) = 0x01;
            *((unsigned char*)0x215) = 0x05;
            *((unsigned char*)0x216) = 0x01;
        }
    }


    if( gVelocityDirection == 1 ) // moving up
    {
        for( i = 0; (i<<2) < gVelocity; i++ )
        {
            if( gYNametable == 2 )
            {
                if( collision[240 + (((gY)&0xF0) ) + ((gX) >> 4)] == 0 &&
                    collision[240 + (((gY)&0xF0) ) + ((gX+0xF) >> 4)] == 0 )
                {
                    if(gY > 0x0F)
                    {
                        gY -= 1;
                    }
                    else
                    {
                        gYScroll = 0xEF;
                        gYNametable = 0;
                    }
                }
                else
                {
                    gVelocity = 0;
                    gVelocityDirection = 0;
                    break;
                }
            }
            else if((gYScroll + gY) >= 0xF0 )
            {
                if( collision[240 + (((gYScroll + gY - 0xF0)&0xF0) ) + ((gX) >> 4)] == 0 &&
                    collision[240 + (((gYScroll + gY - 0xF0)&0xF0) ) + ((gX+0xF) >> 4)] == 0 )
                {
                    if(gY > 0x0F)
                    {
                        gY -= 1;
                    }
                    else
                    {
                        gYScroll -= 1;
                    }
                }
                else
                {
                    gVelocity = 0;
                    gVelocityDirection = 0;
                    break;
                }
            }
            else
            {
                if( collision[(((gYScroll + gY - 0x100) & 0xF0) ) + ((gX) >> 4)] == 0 &&
                    collision[(((gYScroll + gY - 0x100) & 0xF0) ) + ((gX+0xF) >> 4)] == 0 )
                {
                    if(gY > 0x0F)
                    {
                        gY -= 1;
                    }
                    else
                    {
                        if( gYScroll > 0 )
                        {
                            gYScroll -= 1;
                        }
                    }
                }
                else
                {
                    gVelocity = 0;
                    gVelocityDirection = 0;
                    break;
                }
            }
        }
        if( i == ((gVelocity+3)>>2) )
        {
            if( gVelocity == 0 )
            {
                gVelocityDirection = 0;
            }
            else
            {
                gVelocity -= 1;
            }
        }
    }
    else // moving down
    {
        for( i = 0; (i<<2) < gVelocity; i++ )
        {
            if(gYNametable == 2 )
            {
                if( collision[240 + (((gY+0x11)&0xF0) ) + ((gX) >> 4)] == 0 &&
                    collision[240 + (((gY+0x11)&0xF0) ) + ((gX+0xF) >> 4)] == 0 )
                {
                    if(gY < 0xCF)
                    {
                        gY += 1;
                    }
                    else
                    {
                        gVelocity = 0;
                        break;
                    }
                }
                else
                {
                    gVelocity = 0;
                    break;
                }
            }
            else if((gYScroll + gY + 0x11) >= 0xF0)
            {
                if( collision[240 + (((gYScroll + gY+0x11 - 0xF0)&0xF0) ) + ((gX) >> 4)] == 0 &&
                    collision[240 + (((gYScroll + gY+0x11 - 0xF0)&0xF0) ) + ((gX+0xF) >> 4)] == 0 )
                {
                    if(gY < 0xCF)
                    {
                        gY += 1;
                    }
                    else
                    {
                        if( gYScroll == 0xEF )
                        {
                            gYNametable = 2;
                            gYScroll = 0;
                            gVelocity = 0;
                            break;
                        }
                        else
                        {
                            gYScroll+=1;
                        }
                    }
                }
                else
                {
                    gVelocity = 0;
                    break;
                }
            }
            else
            {
                if( collision[(((gYScroll + gY+0x11)&0xF0) ) + ((gX) >> 4)] == 0 &&
                    collision[(((gYScroll + gY+0x11)&0xF0) ) + ((gX+0xF) >> 4)] == 0 )
                {
                    if(gY < 0xCF)
                    {
                        gY += 1;
                    }
                    else
                    {
                        gYScroll+=1;
                    }
                }
                else
                {
                    gVelocity = 0;
                    break;
                }
            }
        }

        if( i == ((gVelocity+3)>>2) )
        {
            gVelocity+=1;
        }
    }
}



void scroll_right(void)
{
    // set bits [1:0] to 0 for nametable
    *((unsigned char*)0x2000) = 0x84 + gYNametable;
    // read 0x2002 to reset the address toggle
    devnull = *((unsigned char*)0x2002);
    // scroll a number of pixels in the X direction
    *((unsigned char*)0x2005) = gXScroll;
    // scroll a number of pixels in the Y direction
    *((unsigned char*)0x2005) = gYScroll;
}

void main(void)
{
    vblank();
    vblank();
    vblank();
    vblank();
    vblank();

    ppudisable();

    palattes();
    vblank();
    patterntables();

    //TODO should move th
    PPU_ADDRESS = 0x20; // address of nametable #0 \
  	PPU_ADDRESS = 0x00;
  	UnRLE(Level1Top);	// uncompresses our data

  	PPU_ADDRESS = 0x28; // address of nametable #2
  	PPU_ADDRESS = 0x00;
  	UnRLE(Level1Bottom);	// uncompresses our data

    vblank();
    sprites();

    apuinit();

    pMusicInit(0x1);

    vblank();
    vblank();
    vblank();
    vblank();
    vblank();

    ppuinit();

    while(1)
    {
        vblank();

        input_poll();

        update_sprites();

        dma_sprites();

        scroll_right();

        do_physics();
    }
}
