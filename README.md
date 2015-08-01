NAME

emagic - automatically convert Russian text into koi8-r

SYNOPSIS

emagic [-v|-q] [-n depth] [-o outfile] file1 file2 ... emagic -w

DESCRIPTION

Encoding Magic Version 1.1 tries to determine encoding of given Russian text and convert it into base encoding of koi8-r. It handles the cases of incorrect encodings (for example, when windows-1251 -> koi8-r conversion was mistakenly applied to file in MSDOS cp866 encoding).

The program tries all combinations of consequtive conversions between known encoding up to specified depth (2 by default) and uses a weight fuction based on character distribution and some frequent words to determine the correct one.

The following encodings are known:

koi8-r - UNIX koi8-r encoding
windows-1251 - Microsoft Windows(R) code page 1251
dos866 - MS-DOS(R) code page 866
iso8859-5 - ISO 8859-5 Cyrillic
The ASCII characters are not changed during all the conversions.

OPTIONS

-v Produce verbose debug output to screen
-q Be quiet - no output to screen at all
-n depth Try conversions up to depth levels deep.
-o outname Write all output to file outname. If -o is not specified, the input files are rewritten.
COPYRIGHT
Encoding Magic is copyright (c) 1997 by Sergey Gershtein.

This is free software, and you are welcome to redistribute it under certain conditions; see file COPYING in original package for details Feel free to send email to sg@mplik.ru with your questions and comments You can get all the Encoding Magic source files from ftp://ftp.mplik.ru/pub/unix/utils/emagic_1.1.tgz

BUGS

The result of applying emagic to non-Russian text is unpredictable.

If parts of input file are in different encodings of Cyrillic the program gets confused and the output is again unpredictable.