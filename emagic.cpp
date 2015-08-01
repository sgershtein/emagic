/** emagic.cpp **/

/*************************************************************************
 *
 *  Encoding Magic by Sergey Gershtein
 *  ( sg@mplik.ru , http://www.mplik.ru/~sg )
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
#include <string.h>
#include <math.h>
#include <fstream.h>

#ifdef MSDOS
#include <except.h>
#endif

#include "frequen.h"
#include "recode.h"
#include "emagic.h"

const int SHOW_USAGE = 10;   // exception (and error) code for bad usage
const int NO_MEMORY  = 9;    // error code for memory allocation error
const int FILE_ERROR = 8;    // exception (and error) code for file open error

int weight( uchar*, uchar* );   // весовая функция
uchar *makesample( uchar* ); // make sample from given buffer string
void head(), usage(), warranty();

int opt_q=0, opt_v=0, opt_n=2;    // options -v and -n and -q

int maxweight = 0;
int rec[20], process[20];
void tryall( uchar*, int );

uchar *exb = NULL;

int main(int argc, char *argv[]) {
   head();
   setFREQ();
   try {
      char *iname = NULL, *oname = NULL;
      fstream *fo = NULL;
      // обработка аргументов
      for( int i = 1; i < argc; i++ ) {
         if( argv[i][0] == '-' && argv[i][1] ) { // ключи
            for( char *s = argv[i]+1; *s; s++ ) {
               switch( *s ) {

                  case 'v': // set verbose
                     opt_v = 1;
                     if( opt_q )
                        throw SHOW_USAGE;
                     break;

                  case 'q': // set verbose
                     opt_q = 1;
                     if( opt_v )
                        throw SHOW_USAGE;
                     break;

                  case 'o': // файл для вывода
                     if( !*(s+1) ) {
                        if( ++i >= argc )
                           throw SHOW_USAGE;
                        oname = argv[i];
                     } else
                        oname = s+1;
                     if( strcmp( oname, "-" ) == 0 )
                        fo = new fstream(1);   // 1 - stdout
                     else
                        fo = new fstream(oname,
                             ios::out
#ifdef DOS
                             | ios::binary
#endif
                             );
                     if( !*fo ) {
                        cerr << "emagic: Error creating file " << iname << "\n";
                        throw FILE_ERROR;
                     }
                     goto NEXT_ARG;

                  case 'n': // глубина перебора
                     if( !*(s+1) ) {
                        if( ++i >= argc )
                           throw SHOW_USAGE;
                        s = argv[i];
                     } else
                        s++;
                     if( *s < '0' || *s > '9' )
                        throw SHOW_USAGE;
                     opt_n = atoi(s);
                     if( opt_n > 10 || opt_n < 1 )
                        throw SHOW_USAGE;
                     goto NEXT_ARG;

                  case 'w':
                     warranty();
                     return 0;

                  default:
                     throw SHOW_USAGE; // unknown option

               } // switch
            } // цикл по символам (ключам)
         } else { // параметры (не начинается с '-')
            // последовательно обрабатываем файл за файлом
            iname = argv[i];

            int filepos = -1; // для работы с файлом "на месте"
            if( !oname )
               if( strcmp( iname, "-" ) == 0 )
                  fo = new fstream(1);   // 1 - stdout
               else {
                  fo = new fstream(iname,
                       ios::out | ios::in | ios::ate
#ifdef DOS
                       | ios::binary
#endif
                       );
                  fo->seekp( filepos = 0 );
               }

            fstream *fi;
            if( strcmp( iname, "-" ) == 0 )
               fi = new fstream(0);
            else if( !oname )
               fi = fo;
            else
               fi = new fstream( iname, ios::in
#ifdef DOS
               | ios::binary
#endif
               );

            if( !*fi || !*fo ) {
               cerr << "emagic: Error opening file " << iname << "\n";
               throw FILE_ERROR;
            }

            uchar *buf = new uchar[RECODEBUFLEN+1];

            fi->seekg(0);
            int done = 0; int check = 1;
            uchar *cbuf = NULL;

            while( !done ) {
               fi->read( buf, RECODEBUFLEN );
               if( fi->bad() ) {
                  cerr << "Error reading from input file " << iname << "\n";
                  throw FILE_ERROR;
               }
               int bread = fi->good() ? RECODEBUFLEN : fi->gcount();
               if( fi->eof() ) {
                  fi->clear();
                  done = 1;
               }
               buf[bread]=0;

               if( check ) {  // определяем нужный тип перекодировки
                  check = 0;

                  // строим таблицу перекодировки
                  int i;
                  for( i = 0; i < 128; i++ )
                    custom[i] = i | 0x80;
                  custom[128] = 0;

                  cbuf = makesample( buf );

                  if( cbuf ) {
                     if( opt_v )
                        cerr << "Sample of " << strlen((char*)cbuf) <<
                             " bytes read from file " << iname << "\n";
                     rec[0] = process[1] = process[0] = -1;

                     // определяем кодировку
                     exb = new uchar[CHARBUFLEN+2];

                     recode( cbuf, RCUSTOM, exb );
                     maxweight = weight( cbuf, exb );
               
                     tryall( cbuf, 0 );
                     
                     delete exb;
                     exb = NULL;

                     if( !opt_q ) {
                        cerr << iname << ": ";
                        if( process[1] < 0 )
                           cerr << "no recoding seems to be necessary";
                        else
                           cerr << "the decoding sequence seems to be: ";
                     }

                     for( i = 1; process[i] >= 0; i++ ) {
                        if( !opt_q )
                           cerr << RNAME[process[i]] << ' ';
                        recode( custom, process[i], NULL );
                     }
                     if( !opt_q )
                        cerr << endl;
                  } else if( !opt_q ) 
                     cerr << iname << ": no Russian text found\n";
               }

               if( filepos >= 0 )
                  fo->seekp( filepos );
               recode( buf, RCUSTOM, NULL );
               fo->write( buf, bread );
               if( filepos >= 0 )
                  filepos = fo->tellp();
               if( fo->bad() ) {
                  cerr << "Error writing output to " << (oname ? oname : iname ) << "\n";
                  throw FILE_ERROR;
               }
               if( filepos >= 0 )
                  fi->seekg( filepos );
            }
            if( fi!=fo ) {
               fi->close();
               delete fi;
            }
            if( !oname ) {
               fo->close();
               delete fo;
            }
            delete buf; buf = NULL;
            if( cbuf ) {
               delete cbuf; 
               cbuf = NULL;
            }
         }
         NEXT_ARG: ;
      }
      // работа завершена (или ее не было)
      if( !iname )
         throw SHOW_USAGE;

      if( oname ) {
         fo->close();
         delete fo;
      }

   } catch( int errno ) {
      if( errno == SHOW_USAGE )
         usage();
      return errno;
#ifdef MSDOS
   } catch( xalloc x ) {
      cerr << "emagic: Cannot allocate memory for recoding buffer\n";
      return NO_MEMORY;
#endif
   } catch( ... ) {
      cerr << "emagic: Unhandled exception caught. Exiting. (:-0)\n";
      return 99;
   }
   return 0;
}

int weight( uchar *str, uchar *exb ) {
   int w = 10000;
   int count[256];
   int i;
   int maxcount=1;
   for( i = 0; i<256; i++ ) count[(uchar)i] = 0;
   uchar *s;
   for( s = str; *s; s++ ) {
      if( *s < 128 )
         continue;
      count[*s]++;
      if( count[*s] > maxcount ) maxcount = count[*s];
      if( *s >= 128 && FREQ[*s] == 0 &&
          FREQ[*(s-1)] > 0 && FREQ[*(s+1)] > 0 )
         w += BADCHAR; // штраф за неизвестный символ среди известных
   }
   if( opt_v )
      cerr << "weight: b = " << w;

   for( i = 128; i<256; i++ )
      w -= abs( count[(uchar)i]*254/maxcount - FREQ[(uchar)i] );

   if( opt_v )
      cerr << "; d = " << w;

   // анализируем однобуквенные слова

   for( s = str+1; *s; s++ )
      if( *(s-1) == ' ' && *(s+1) == ' ' && *s > 127 ) 
         w += strchr( SINGLES, *s ) ? GOODSINGLE : BADSINGLE;

   if( opt_v )
      cerr << "; s = " << w;

   // теперь считаем вес по словам .....
   for( i = 0; WORDS[i].word; i++ ) {
      int count = 0;
      for( char *pos = (char*)exb; (pos = strstr( pos, WORDS[i].word )) != NULL;
            pos++) count++;
      w += WORDS[i].weight * count;
   }

   if( opt_v )
      cerr << "; w = " << w << endl;

   return w;
}

void tryall( uchar *buf, int level ) {
   if( ++level > opt_n )
      return;
   int i;
   for( i = 0; i < 12; i++ ) {
      int n3 = rec[level-1]/3, np3 = rec[level-1]%3+1;
      if( (i/3) == ((n3==np3)?0:np3) )
         continue;  // неэффективные последовательности
                    // (koi->iso, iso->alt) == koi->alt
      recode( buf, rec[level] = i, exb );
      if( opt_v ) {
         cerr << "Trying: ";
         for( int j = 1; j <= level; j++)
            cerr << RNAME[rec[j]] << ", ";
      }
      int w = weight( buf, exb ) - (level+1)*20;
      if( w > maxweight ) {
         maxweight = w;
         for( int k = 0; k <= level; k++ )
            process[ k ] = rec[ k ];
         process[ level + 1 ] = -1;
      }
      tryall( buf, level );
      recode( buf, REVERSE[i], NULL );  // перекодировать обратно
   }
}

uchar *makesample( uchar* buf0 ) {
// пропускаем все английские буквы, ужимаем все пробелы и пунктуацию до
// одного пробела

   uchar *buf = new uchar[CHARBUFLEN+2];
   uchar *pos = buf;
   *pos = ' ';
   uchar *ch;

   for( ch = buf0; *ch && pos-buf < CHARBUFLEN; ch++ ) {
      if( *ch<='"' || *ch==')' || *ch=='(' || *ch==',' ||
          *ch=='-' || *ch==':' || *ch==';' ) { // пробельные символы
         if( *pos != ' ' )
            *++pos = ' ';
      } else if( *ch >=128 ) { // возможно русская буква
         *++pos = *ch;
      } else { // латинская буква или знак
         if( *pos >= 128 )
            *++pos = *ch;
         // мы не хотим создавать одиночные русские буквы там. где их нет
      }
   }
   if( pos == buf ) {
      delete buf;
      return NULL;
   }
   *++pos = ' ';
   *++pos = '\0';

   return buf;
}

void warranty() {
cerr << endl <<
"                               NO WARRANTY\n"
"\n"
"     1. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY\n"
"   FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN\n"
"   OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES\n"
"   PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED\n"
"   OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF\n"
"   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS\n"
"   TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE\n"
"   PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,\n"
"   REPAIR OR CORRECTION.\n"
"\n"
"     2. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING\n"
"   WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR\n"
"   REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,\n"
"   INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING\n"
"   OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED\n"
"   TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY\n"
"   YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER\n"
"   PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE\n"
"   POSSIBILITY OF SUCH DAMAGES.\n";
}

void head() {
   cerr <<
   "Encoding Magic Version 1.1. Copyright (c) 1997 by Sergey Gershtein\n";
}

void usage() {
   cerr <<
   "\n"
   "This program tries to determine encoding of given Russian text and\n"
   "convert it into correct base encoding\n"
   "\n"
   "Usage: emagic [-v|-q] [-n depth] [-o outfile] file1 file2 ...\n"
   "       emagic -w\n"
   "\n"
   "       fileN    - input file name or - for standard input\n"
   "       outfile  - output file name, if not specified replace input file(s)\n"
   "       -v       - a lot of verbose debug output to screen\n"
   "       -q       - be quiet (no output to screen at all)\n"
   "       -n depth - do up to depth consequitive recodings (2 by default)\n"
   "       -w       - show details on warranty\n"
   "\n"
   "Emagic comes with ABSOLUTELY NO WARRANTY; for details type emagic -w\n"
   "This is free software, and you are welcome to redistribute it\n"
   "under certain conditions; see file COPYING in original package for details\n"
   "Feel free to send email to sg@mplik.ru with your questions and comments\n"
   "You can get all the Encoding Magic source files from\n"
   "ftp://ftp.mplik.ru/pub/unix/utils/emagic_1.1.tgz\n";
}
