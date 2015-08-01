/******************
 Simple utility program to build conversion table data
 Gets table file names as arguments and creates tables,h file
 ******************/

#include <fstream.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	ofstream fo("tables.h");
	if( !fo ) {
		cerr << "cannot create tables.h\n";
		return 1;
        }
        for(int i = 1; i<argc; i++) {
        	ifstream fi(argv[i]);
        	int tbl[128],res[128];
        	int j;
        	for( j=0; j<128; j++ ) {
        		tbl[j]=-1;
        		res[j]=-1;
                }
        	if( !fi ) {
        		cerr << "cannot open " << argv[i] << endl;
        		fo.close();
        		return 2;
                }
                while( !fi.eof() ) {
                	char word[500];
                	fi >> word;
                	if( word[0] == '#' ) {
                		fi.getline(word,499);
                		continue;
                        }
                        int a, b;
                        sscanf(word,"%x",&a);
                        fi >> word;
                        sscanf(word,"%x",&b);
                        if( fi.bad() ) {
				cerr << "Error in file " << argv[i] << endl;
				return 3;
                        }
                        tbl[a-128] = b;
                        res[b-128] = a;
                }

                // отображаем само в себя то, что можно
                for( j=0; j<128; j++ )
                	if( tbl[j] == -1 && res[j] == -1 ) // можно само в себя
                		res[j] = tbl[j] = j+128;

		// теперь то, что нельзя - заботимся о биективности
                for( j=0; j<128; j++ )
                	if( tbl[j] == -1 )
                		for( int k=0; k<128; k++ )
                			if( res[k] == -1 ) {
                				tbl[j] = k+128;
                				res[k] = j+128;
                				break;
                                        }
                		
                fi.close();
                
                // Печатаем прямое преобразование
                fo << "uchar " << argv[i][0] << argv[i][1] << "[] = {";
                for( j=0; j<128; j++ ) {
                	fo << tbl[j];
                	fo << (( j==127 ) ? "};" : ",");
                	if( j%16 == 15 ) fo << endl << "              ";
                }
                fo << endl;

                // Печатаем обратное преобразование
                fo << "uchar " << argv[i][1] << argv[i][0] << "[] = {";
                for( j=0; j<128; j++ ) {
                	fo << res[j];
                	fo << (( j==127 ) ? "};" : ",");
                	if( j%16 == 15 ) fo << endl << "              ";
                }
                fo << endl;
        }
        return 0;
}
