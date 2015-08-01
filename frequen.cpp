/** frequen.cpp **/

/*************************************************************************
 *
 *  Encoding Magic by Sergey Gershtein
 *  ( sg@mplik.ru, http://www.mplik.ru/~sg )
 *
 *  This program tries to determine encoding of a given Russian text
 *  based on character frequences and presence (or absence :) ) of some
 *  frequently used Russian words. It then decodes the text into base
 *  encoding.  The program handles cases where several consequitive
 *  recodings are necessary. The following encodings are recognized:  KOI-8,
 *  cp866 (DOS Alternative), windows-1251 (Windows Russian), ISO-8859-5.
 *
 *  Encoding Magic (emagic) is copyright (c) 1997 by Sergey Gershtein.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License (in file named COPYING) for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *************************************************************************/

#include <stdlib.h>
#include "frequen.h"
#include "recode.h"


// ��� ������騥�� � ���᪮� ⥪�� ᫮�� � �᫨ ᫮�, �� ���
STRUCT_WORDS WORDS[] = {
    {" �ਢ�� ", 150},
    {" ��ࠢ��", 150},
    {" 㢠�", 150},
    {" �� ", 75},
    {" � ", 75},
    {" �� ", 75},
    {" �� ", 75},
    {" �� ", 75},
    {" ��� ", 75},
    {" ��� ", 75},
    {" ��� ", 75},
    {" ��� ", 75},
    {" �� ", 75},
    {" �� ", 75},
    {" �� ", 75},
    {" ��", 25},
    {" ��", 25},
    {" ⠪", 25},
    // insert more words here if desired
    {NULL, 0} };

// �ᥢ������� �㪢�. �����⨬� ��� ������� ᫮��
const char SINGLES[] = "㪢���ᨓ�������";

// �⮨���� ������������ � �����⭮�� �����㪢������ ᫮��
const int BADSINGLE = -50;
const int GOODSINGLE = +50;

// �⮨����� ��࠯����� �������⭮�� ᨬ���� ����� ������묨
const int BADCHAR = -10;

uchar TOLOWER[256];
int FREQ[256];

void setFREQ() {
   for( int i=0; i<256; i++) {
   	FREQ[i] = 0;
   	TOLOWER[i] = i;
   }
   
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';
   TOLOWER[(uchar)'�'] = '�';

   FREQ[(uchar)'�'] = 192;
   FREQ[(uchar)'�'] = 39;
   FREQ[(uchar)'�'] = 104;
   FREQ[(uchar)'�'] = 38;
   FREQ[(uchar)'�'] = 72;
   FREQ[(uchar)'�'] = 188;
   FREQ[(uchar)'�'] = 22;
   FREQ[(uchar)'�'] = 44;
   FREQ[(uchar)'�'] = 152;
   FREQ[(uchar)'�'] = 24;
   FREQ[(uchar)'�'] = 84;
   FREQ[(uchar)'�'] = 114;
   FREQ[(uchar)'�'] = 76;
   FREQ[(uchar)'�'] = 155;
   FREQ[(uchar)'�'] = 250;
   FREQ[(uchar)'�'] = 70;
   FREQ[(uchar)'�'] = 101;
   FREQ[(uchar)'�'] = 122;
   FREQ[(uchar)'�'] = 142;
   FREQ[(uchar)'�'] = 66;
   FREQ[(uchar)'�'] = 4;
   FREQ[(uchar)'�'] = 20;
   FREQ[(uchar)'�'] = 8;
   FREQ[(uchar)'�'] = 34;
   FREQ[(uchar)'�'] = 21;
   FREQ[(uchar)'�'] = 8;
   FREQ[(uchar)'�'] = 44;
   FREQ[(uchar)'�'] = 40;
   FREQ[(uchar)'�'] = 9;
   FREQ[(uchar)'�'] = 12;
   FREQ[(uchar)'�'] = 56;
   FREQ[(uchar)'�'] = 1;
   FREQ[(uchar)'�'] = 199;
   FREQ[(uchar)'�'] = 37;
   FREQ[(uchar)'�'] = 104;
   FREQ[(uchar)'�'] = 36;
   FREQ[(uchar)'�'] = 68;
   FREQ[(uchar)'�'] = 202;
   FREQ[(uchar)'�'] = 23;
   FREQ[(uchar)'�'] = 43;
   FREQ[(uchar)'�'] = 173;
   FREQ[(uchar)'�'] = 28;
   FREQ[(uchar)'�'] = 79;
   FREQ[(uchar)'�'] = 119;
   FREQ[(uchar)'�'] = 76;
   FREQ[(uchar)'�'] = 155;
   FREQ[(uchar)'�'] = 254;
   FREQ[(uchar)'�'] = 68;
   FREQ[(uchar)'�'] = 115;
   FREQ[(uchar)'�'] = 119;
   FREQ[(uchar)'�'] = 148;
   FREQ[(uchar)'�'] = 66;
   FREQ[(uchar)'�'] = 5;
   FREQ[(uchar)'�'] = 19;
   FREQ[(uchar)'�'] = 11;
   FREQ[(uchar)'�'] = 35;
   FREQ[(uchar)'�'] = 19;
   FREQ[(uchar)'�'] = 9;
   FREQ[(uchar)'�'] = 1;
   FREQ[(uchar)'�'] = 47;
   FREQ[(uchar)'�'] = 48;
   FREQ[(uchar)'�'] = 6;
   FREQ[(uchar)'�'] = 15;
   FREQ[(uchar)'�'] = 53;
   FREQ[(uchar)'�'] = 1;
}
