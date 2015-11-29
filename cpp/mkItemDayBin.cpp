// item time bin, time bias of different items in time bin
#include "kddcup_feat.h"

int bin_num   = 40;
int bin_size  = 100;
const int START_DAY = 2672;
const int NUM_DAY   = 3974;
const int NUM_ITEM  = 624961;
 
int main( int argc, char *argv[] ){
    if( argc < 3 ){
        printf( "usage:<input> <input.day> [binsize=100]\n" );
        return -1;
    }
    if( argc > 3 ){
        bin_size = atoi( argv[3] );
        if( 3974 % bin_size == 0 ){
            bin_num = NUM_DAY / bin_size;
        }else{
            bin_num = NUM_DAY / bin_size + 1;
        }
    }
    char fout[ 256 ];
    sprintf( fout, "features/%s.ItemDayBin", argv[1] );
    FILE *fi = fopen_check( argv[1], "r" );
    FILE *fd = fopen_check( argv[2], "r" );
    FILE *fo = fopen_check( fout, "w" );

    fprintf( fo, "%d\n", bin_num * NUM_ITEM );
    int uid, iid, rate, day;
    while( fscanf( fi, "%d%d%d", &uid, &iid, &rate ) == 3 ){
        assert_true( fscanf( fd, "%d", &day ) == 1 );
        fprintf( fo, "1 %d:1\n", iid * bin_num + (day-START_DAY) / bin_size );
    }
    fclose( fo );
    fclose( fd );
    fclose( fi );
    return 0;
}
