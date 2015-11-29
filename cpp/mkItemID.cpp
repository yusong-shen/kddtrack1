// cpp version of make item ID
#include "kddcup_feat.h"

const int NUM_ITEM  = 624961;

int main( int argc, char *argv[] ){
    if( argc < 2 ){
        printf( "usage:<input>\n" );
        return -1;
    }
    char fout[ 256 ];
    sprintf( fout, "features/%s.ItemID", argv[1] );
    FILE *fi = fopen_check( argv[1], "r" );
    FILE *fo = fopen_check( fout, "w" );

    fprintf( fo, "%d\n", NUM_ITEM );
    int uid, iid, rate;
    while( fscanf( fi, "%d%d%d", &uid, &iid, &rate ) == 3 ){
        fprintf( fo, "1 %d:1\n", iid );
    }
    fclose( fo );
    fclose( fi );
    return 0;
}
