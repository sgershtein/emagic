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


// часто встречающиеся в русском тексте слова и часли слов, их веса
STRUCT_WORDS WORDS[] = {
    {" привет ", 150},
    {" здравств", 150},
    {" уваж", 150},
    {" мы ", 75},
    {" то ", 75},
    {" не ", 75},
    {" что ", 75},
    {" вы ", 75},
    {" вас ", 75},
    {" нас ", 75},
    {" вам ", 75},
    {" нам ", 75},
    {" это ", 75},
    {" кто ", 75},
    {" тот ", 75},
    {" пре", 25},
    {" при", 25},
    {" так", 25},
    // insert more words here if desired
    {NULL, 0} };

// всевозможные буквы. допустимые как одиночные слова
const char SINGLES[] = "укваоясиУКВАОЯСИ";

// Стоимости неопознаного и известного однобуквенного слова
const int BADSINGLE = -50;
const int GOODSINGLE = +50;

// Стоимость вкрапления неизвестного символа между известными
const int BADCHAR = -10;

uchar TOLOWER[256];
int FREQ[256];

void setFREQ() {
   for( int i=0; i<256; i++) {
   	FREQ[i] = 0;
   	TOLOWER[i] = i;
   }
   
   TOLOWER[(uchar)'А'] = 'а';
   TOLOWER[(uchar)'Б'] = 'б';
   TOLOWER[(uchar)'В'] = 'в';
   TOLOWER[(uchar)'Г'] = 'г';
   TOLOWER[(uchar)'Д'] = 'д';
   TOLOWER[(uchar)'Е'] = 'е';
   TOLOWER[(uchar)'Ё'] = 'ё';
   TOLOWER[(uchar)'Ж'] = 'ж';
   TOLOWER[(uchar)'З'] = 'з';
   TOLOWER[(uchar)'И'] = 'и';
   TOLOWER[(uchar)'Й'] = 'й';
   TOLOWER[(uchar)'К'] = 'к';
   TOLOWER[(uchar)'Л'] = 'л';
   TOLOWER[(uchar)'М'] = 'м';
   TOLOWER[(uchar)'Н'] = 'н';
   TOLOWER[(uchar)'О'] = 'о';
   TOLOWER[(uchar)'П'] = 'п';
   TOLOWER[(uchar)'Р'] = 'р';
   TOLOWER[(uchar)'С'] = 'с';
   TOLOWER[(uchar)'Т'] = 'т';
   TOLOWER[(uchar)'У'] = 'у';
   TOLOWER[(uchar)'Ф'] = 'ф';
   TOLOWER[(uchar)'Х'] = 'х';
   TOLOWER[(uchar)'Ц'] = 'ц';
   TOLOWER[(uchar)'Ч'] = 'ч';
   TOLOWER[(uchar)'Ш'] = 'ш';
   TOLOWER[(uchar)'Щ'] = 'щ';
   TOLOWER[(uchar)'Ъ'] = 'ъ';
   TOLOWER[(uchar)'Ы'] = 'ы';
   TOLOWER[(uchar)'Ь'] = 'ь';
   TOLOWER[(uchar)'Э'] = 'э';
   TOLOWER[(uchar)'Ю'] = 'ю';
   TOLOWER[(uchar)'Я'] = 'я';

   FREQ[(uchar)'А'] = 192;
   FREQ[(uchar)'Б'] = 39;
   FREQ[(uchar)'В'] = 104;
   FREQ[(uchar)'Г'] = 38;
   FREQ[(uchar)'Д'] = 72;
   FREQ[(uchar)'Е'] = 188;
   FREQ[(uchar)'Ж'] = 22;
   FREQ[(uchar)'З'] = 44;
   FREQ[(uchar)'И'] = 152;
   FREQ[(uchar)'Й'] = 24;
   FREQ[(uchar)'К'] = 84;
   FREQ[(uchar)'Л'] = 114;
   FREQ[(uchar)'М'] = 76;
   FREQ[(uchar)'Н'] = 155;
   FREQ[(uchar)'О'] = 250;
   FREQ[(uchar)'П'] = 70;
   FREQ[(uchar)'Р'] = 101;
   FREQ[(uchar)'С'] = 122;
   FREQ[(uchar)'Т'] = 142;
   FREQ[(uchar)'У'] = 66;
   FREQ[(uchar)'Ф'] = 4;
   FREQ[(uchar)'Х'] = 20;
   FREQ[(uchar)'Ц'] = 8;
   FREQ[(uchar)'Ч'] = 34;
   FREQ[(uchar)'Ш'] = 21;
   FREQ[(uchar)'Щ'] = 8;
   FREQ[(uchar)'Ы'] = 44;
   FREQ[(uchar)'Ь'] = 40;
   FREQ[(uchar)'Э'] = 9;
   FREQ[(uchar)'Ю'] = 12;
   FREQ[(uchar)'Я'] = 56;
   FREQ[(uchar)'Ё'] = 1;
   FREQ[(uchar)'а'] = 199;
   FREQ[(uchar)'б'] = 37;
   FREQ[(uchar)'в'] = 104;
   FREQ[(uchar)'г'] = 36;
   FREQ[(uchar)'д'] = 68;
   FREQ[(uchar)'е'] = 202;
   FREQ[(uchar)'ж'] = 23;
   FREQ[(uchar)'з'] = 43;
   FREQ[(uchar)'и'] = 173;
   FREQ[(uchar)'й'] = 28;
   FREQ[(uchar)'к'] = 79;
   FREQ[(uchar)'л'] = 119;
   FREQ[(uchar)'м'] = 76;
   FREQ[(uchar)'н'] = 155;
   FREQ[(uchar)'о'] = 254;
   FREQ[(uchar)'п'] = 68;
   FREQ[(uchar)'р'] = 115;
   FREQ[(uchar)'с'] = 119;
   FREQ[(uchar)'т'] = 148;
   FREQ[(uchar)'у'] = 66;
   FREQ[(uchar)'ф'] = 5;
   FREQ[(uchar)'х'] = 19;
   FREQ[(uchar)'ц'] = 11;
   FREQ[(uchar)'ч'] = 35;
   FREQ[(uchar)'ш'] = 19;
   FREQ[(uchar)'щ'] = 9;
   FREQ[(uchar)'ъ'] = 1;
   FREQ[(uchar)'ы'] = 47;
   FREQ[(uchar)'ь'] = 48;
   FREQ[(uchar)'э'] = 6;
   FREQ[(uchar)'ю'] = 15;
   FREQ[(uchar)'я'] = 53;
   FREQ[(uchar)'ё'] = 1;
}
