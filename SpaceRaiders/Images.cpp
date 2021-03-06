﻿#include "Base.h"
#include "Images.h"
#include "Vector2D.h"
#include "Colors.h"
#include <cassert>
#include <fstream>
#include <sys/stat.h>

namespace
{

#if XMAS_EDITION

// 0: red
// 2: green
// 8: white
// 9  white intense
// :  light blue
// ;  lightBlueIntense
// <  violet
// =  violetIntense

const byte alien_color[2 + 8 * 4] = 
R"(
1111199
1111111
8888888
8888888
)";

#else
	#define alien_color nullptr
#endif

const wchar_t alien0_0Str[2 + 8 * 4] = 
LR"(
 ▀▄▄▄▀ 
▄▀▀█▀▀▄
▀█▀▀▀█▀
  ▀ ▀  
)";

const wchar_t alien0_1Str[2 + 8 * 4] = 
LR"(
 ▀▄▄▄▀ 
▄▀▀█▀▀▄
█▀▀▀▀▀█
 ▀   ▀ 
)";


const wchar_t alien1_0Str[2 + 8 * 4] = 
LR"(
 ▄▄█▄▄ 
█▀▀█▀▀█
▀█▀▀▀█▀
  ▀ ▀  
)";

const wchar_t alien1_1Str[2 + 8 * 4] = 
LR"(
 ▄▄█▄▄ 
█▀▀█▀▀█
█▀▀▀▀▀█
 ▀   ▀ 
)";

const wchar_t alien2_0Str[2 + 8 * 4] = 
LR"(
 ▄ ▄ ▄ 
█▀▀█▀▀█
█▀▀▀▀▀█
 ▀   ▀ 
)";

const wchar_t alien2_1Str[2 + 8 * 4] = 
LR"(
 ▄ ▄ ▄ 
█▀▀█▀▀█
▀█▀▀▀█▀
  ▀ ▀  
)";

const wchar_t alien3_0Str[2 + 8 * 4] = 
LR"(
  ▄ ▄  
▄▀▀█▀▀▄
▀█▀▀▀█▀
  ▀ ▀  
)";

const wchar_t alien3_1Str[2 + 8 * 4] = 
LR"(
  ▄ ▄  
▄▀▀█▀▀▄
█▀▀▀▀▀█
 ▀   ▀ 
)";


const wchar_t boss0_0Str[] = 
LR"(
  ▀▄   ▄▀  
 ▄█▀███▀█▄ 
███▄▄█▄▄███
▄▀  ▀ ▀  ▀▄
 ▀▀     ▀▀ 
)";

const wchar_t boss0_1Str[] = 
LR"(
  ▀▄   ▄▀  
 ▄█▀███▀█▄ 
███▄▄█▄▄███
 ▄▀ ▀ ▀ ▀▄ 
  ▀▀   ▀▀  
)";

const wchar_t boss1_0Str[] = 
LR"(
   ▄███▄   
 ▄▀█████▀▄ 
██▄▄███▄▄██
  ▄▀▀ ▀▀▄  
   ▀   ▀   
)";

const wchar_t boss1_1Str[] = 
LR"(
   ▄███▄   
 ▄▀█████▀▄ 
██▄▄███▄▄██
 ▄▀▀   ▀▀▄ 
  ▀     ▀  
)";

const wchar_t boss2_0Str[] = 
LR"(
 ▄▄██ ██▄▄ 
██▀█████▀██
██▄▄███▄▄██
 ▄▀ ▀ ▀ ▀▄ 
  ▀     ▀  
)";

const wchar_t boss2_1Str[] = 
LR"(
 ▄▄██ ██▄▄ 
██▀█████▀██
██▄▄███▄▄██
▄▀  ▀ ▀  ▀▄
 ▀       ▀ 
)";

const wchar_t laserStr[] =
LR"(
║
║
║
)";

const wchar_t laserLeftStr[] =
LR"(
\  
 \ 
  \
)";

const wchar_t laserRightStr[] =
LR"(
  /
 / 
/  
)";

const wchar_t playerShipStr[] =
LR"(
  ▄     ▄  
  ██   ██  
▄█████████▄
)";
const byte playerShipColors[] =
R"(
  :     :  
  :9   9:  
99999999999
)";

const wchar_t explosionStr[] = 
LR"(
 ▀▄ ▄▀ 
▀▀█▀█▀▀
 ▀   ▀ 
)";

const wchar_t DbonusStr[] = 
LR"(
 ▄▄▄ 
█ D █
 ▀▀▀ 
)";

const wchar_t TbonusStr[] = 
LR"(
 ▄▄▄ 
█ T █
 ▀▀▀ 
)";
 
const wchar_t IbonusStr[] =
LR"(
 ▄▄▄ 
█ I █
 ▀▀▀ 
)";
const wchar_t SbonusStr[] = 
LR"(
 ▄▄▄ 
█ S █
 ▀▀▀ 
)";
const wchar_t shieldBonusImg[] = 
LR"(
 ▄▄▄ 
█SHI█
 ▀▀▀ 
)";
const wchar_t FbonusStr[] = 
LR"(
 ▄▄▄ 
█ F █
 ▀▀▀ 
)";
const wchar_t bombStr[] = 
LR"(
 ▄▄█▀
█ B █
 ▀▀▀ 
)";

/*const wchar_t FbonusStr[] = 
LR"(
╭---╮
| f |
╰---╯
)";*/
const wchar_t solidWallStr[] = 
LR"(
████
████
)";
const wchar_t brittleWallStr[] = 
LR"(
░░░░
░░░░
)";
const wchar_t shieldStr[] =
LR"(
▄ ▄ ▄▄▄▄▄▄▄ ▄ ▄
)";

const wchar_t girlStr[] =
LR"(
  .__. 
 {('')}
 -/  \/
 /____\
 _/ |_ 
)";

const wchar_t planetStr[] = 
LR"(
     ~+                          
            *       +            
        '                  |     
    ()    .-.,="``"=.    - o -   
          '=/_       \     |     
       *   |  '=._    |          
            \     `=./`,        '
         .   '=.__.=' `='      * 
+                         +      
     O      *        '       .   
)";


const wchar_t _0Img[] =
LR"(
   ____ 
  / __ \
 / / / /
/ /_/ / 
\____/  
)";

const wchar_t _1Img[] =
LR"(
    __
   / /
  / / 
 / /  
/_/   
)";

const wchar_t _2Img[] =
LR"(
   ___ 
  |__ \
  __/ /
 / __/ 
/____/ 
)";

const wchar_t _3Img[] =
LR"(
   _____
  |__  /
   /_ < 
 ___/ / 
/____/  
)";

const wchar_t levelImg[] =
LR"(
    __                   __
   / /   ___ _   _____  / /
  / /   / _ \ | / / _ \/ / 
 / /___/  __/ |/ /  __/ /  
/_____/\___/|___/\___/_/   
)";

const wchar_t snowFlakeImg[] =
LR"(
*
)";


const wchar_t giftImg[] =
LR"(
            .-" +' "-.  
           /.'.'A_'*`.\ 
          |:.*'/\-\. ':|
          |:.'.||"|.'*:|
           \:~^~^~^~^:/ 
            /`-....-'\  
           /          \ 
           `-.,____,.-' 
)";

const wchar_t happyHolImg[] =
LR"(
 __,  ,___,                   
(--|__| _,,_ ,_               
  _|  |(_||_)|_)\/            
          |  | _/             
        __,  ,___,            
       (--|__| _ |' _| _,   , 
         _|  |(_)||(_|(_|\//_)
        (               _/    
)";

const wchar_t leafImg[] =
LR"(
      _/\_     __/\__
      )   (_  _) .' (
      `) '.( ) .'  (`
       `-._\()/__(~` 
           ()()      
          / |`\      
          ) : (      
          `)_/`      
)";

const Image images[numImages] =
{
	{ nullptr, nullptr, 0, 0 },
	{ planetStr, nullptr, 33, 10 },
	{ alien0_0Str, alien_color, 7, 4 },
	{ alien0_1Str, alien_color, 7, 4 },
	{ alien1_0Str, alien_color, 7, 4 },
	{ alien1_1Str, alien_color, 7, 4 },
	{ alien2_0Str, alien_color, 7, 4 },
	{ alien2_1Str, alien_color, 7, 4 },
	{ alien3_0Str, alien_color, 7,	4 },
	{ alien3_1Str, alien_color, 7,	4 },
	{ boss0_0Str, nullptr, 11, 5 },
	{ boss0_1Str, nullptr, 11, 5 },
	{ boss1_0Str, nullptr, 11, 5 },
	{ boss1_1Str, nullptr, 11, 5 },
	{ boss2_0Str, nullptr, 11,	5 },
	{ boss2_1Str, nullptr, 11,	5 },
	{ laserStr,	nullptr, 1, 3 },
	{ laserLeftStr,nullptr, 	3, 3 },
	{ laserRightStr, nullptr, 3,	3 },
	{ laserStr,	nullptr, 1, 3 },
	{ playerShipStr, playerShipColors, 11, 3 },
	{ explosionStr,	nullptr, 7, 3 },
	{ DbonusStr, nullptr, 5,	3 },
	{ TbonusStr, nullptr, 5,	3 },
	{ SbonusStr, nullptr, 5, 3 },
	{ FbonusStr, nullptr, 5, 3 },
	{ IbonusStr, nullptr, 5, 3 },
	{ shieldBonusImg, nullptr, 5, 3 },
	{ shieldStr, nullptr, 15, 1 },
	{ solidWallStr,	nullptr, 4, 2 },
	{ brittleWallStr, nullptr, 4, 2 },
	{ bombStr, nullptr, 5, 3 },
	{ snowFlakeImg, nullptr, 1, 1 },
	{ giftImg, nullptr, 24, 8 },
	{ happyHolImg, nullptr, 30, 8 },
	{ leafImg, nullptr, 21, 8 },
	{ levelImg, nullptr, 27, 5},
	{ girlStr, nullptr, 7, 5 },
	{ _0Img, nullptr, 8, 5 },
	{ _1Img, nullptr, 6, 5 },
	{ _2Img, nullptr, 7, 5 },
	{ _3Img, nullptr, 8, 5 },
};

}


const Image& GetImage(ImageId imageId)
{
	return images[(int)imageId];
}


Vector2D GetImageSize(ImageId imageId)
{
	const Image& image = GetImage(imageId);
	return { (float)image.width, (float)image.height };
}


long GetFileSize(const char* fileName)
{
	struct stat stat_buf;
	int rc = stat(fileName, &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}


bool LoadTxtImage(ImageA& image, int width, int height, const char* fileName)
{
	const long fileSize = GetFileSize(fileName);
	if (fileSize == -1)
	{
		return false;
	}
	const size_t imgSize = width * height;
	if (fileSize != (width + 2) * height) // +2: CR LF
	{
		return false;
	}

	std::ifstream inFile;
	inFile.open(fileName);
    if (!inFile) 
	{
		return false;
    }
    
	char* img = static_cast<char*>( malloc(imgSize * sizeof(char)) );
	char* dst = img;
	for (int row = 0; row < height; ++row)
	{
		inFile.read(dst, width);
		// skip CR/LF
		char CRLF;
		inFile.read(&CRLF, 1);
		dst += width;
	}
	image.img = img;
	image.width = width;
	image.height = height;
	image.freeMemory = true;
    
    inFile.close();
	return true;
}


