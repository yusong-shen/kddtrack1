// item time bin, time bias of different items in time bin
#include "kddcup_feat.h"

int num_index;
const int NUM_USER = 1000990;
int start[ NUM_USER ];
 
inline void load_stats( void ){
    FILE *fi = fopen_check( "user_daystats.txt", "r" );
    num_index = 0;
    int uid, nday;
    while( fscanf( fi, "%d%*d%*d%d", &uid, &nday ) == 2 ){
        start[ uid ] = num_index;
        num_index += nday;
    }
    fclose( fi );
}
int main( int argc, char *argv[] ){
    if( argc < 3 ){
        printf( "usage:<input> <input.userdayidx>\n" );
        return -1;
    }
    load_stats();
    char fout[ 256 ];
    sprintf( fout, "features/%s.UserDayBias", argv[1] );
    FILE *fi = fopen_check( argv[1], "r" );
    FILE *fd = fopen_check( argv[2], "r" );
    FILE *fo = fopen_check( fout, "w" );

    fprintf( fo, "%d\n", num_index );
    int uid, iid, rate, dayidx;
    while( fscanf( fi, "%d%d%d", &uid, &iid, &rate ) == 3 ){
        assert_true( fscanf( fd, "%d", &dayidx ) == 1 );
        fprintf( fo, "1 %d:1\n", start[ uid ] + dayidx );
    }
    fclose( fo );
    fclose( fd );
    fclose( fi );
    return 0;
}
