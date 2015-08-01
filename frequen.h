/** frequen.h **/

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

// часто встречающиеся в русском тексте слова и часли слов, их веса
struct STRUCT_WORDS {
    char *word;
    int  weight;
};

extern STRUCT_WORDS WORDS[];

// всевозможные буквы. допустимые как одиночные слова
extern const char SINGLES[];

// веса - неизвестное однобуквенное слово и известное однобуквенное слово
extern const int BADSINGLE;
extern const int GOODSINGLE;

// вес неизвестного символа, вкрапленного в известные
extern const int BADCHAR;

extern int FREQ[256];

void setFREQ();
