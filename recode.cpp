/** recode.cpp **/

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

#include "recode.h"
#include "tables/tables.h"

uchar custom[129];

uchar *TABLE[13] = {
	ik, ia, iw,
	ki, ka, kw,
	ak, ai, aw,
	wk, wa, wi,
      custom };

// перекодировки TABLE[i] и TABLE[REVERSE[i]] взаимно обратны
int REVERSE[12] = {
	3, 7, 11,
	0, 6, 9,
	4, 1, 10,
	5, 8, 2 };

char *RNAME[12] = {
	"iso->koi",
	"iso->alt",
	"iso->win",
	"koi-iso",
	"koi->alt",
	"koi->win",
	"alt->koi",
	"alt->iso",
	"alt->win",
	"win->koi",
	"win->alt",
	"win->iso"};

void recode( uchar *str, int n, uchar *cpy ) {
   uchar *c=str;
   uchar *table = TABLE[n];
   do {
      *c = *c > 0x7f ? table[*c & 0x7f] : *c;
      if( cpy )
      	*cpy++ = TOLOWER[*c];
   } while( *c++ );
      if( cpy )
      	*cpy++ = '\0';
}
