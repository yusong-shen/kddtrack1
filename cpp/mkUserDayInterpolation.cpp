// item time bin, time bias of different items in time bin
#include "kddcup_feat.h"

int num_index;
const int NUM_USER = 1000990;
int first[ NUM_USER ], last[ NUM_USER ], pustart[ NUM_USER ];
 
inline void load_stats( void ){
    num_index = 0;
    FILE *fi = fopen_check( "user_daystats.txt", "r" );
    int uid, st, ed;
    while( fscanf( fi, "%d%d%d%*d", &uid, &st, &ed ) == 3 ){
        first[ uid ] = st;
        last [ uid ] = ed;
        pustart[ uid ] = num_index;
        num_index ++;
        if( ed != st ) num_index ++;
    }
    fclose( fi );
}
int main( int argc, char *argv[] ){
    if( argc < 3 ){
        printf( "usage:<input> <input.day>\n" );
        return -1;
    }
    load_stats();
    char fout[ 256 ];
    sprintf( fout, "features/%s.UserDayInterpolation", argv[1] );
    FILE *fi = fopen_check( argv[1], "r" );
    FILE *fd = fopen_check( argv[2], "r" );
    FILE *fo = fopen_check( fout, "w" );

    fprintf( fo, "%d\n", num_index );
    int uid, iid, rate, day;
    while( fscanf( fi, "%d%d%d", &uid, &iid, &rate ) == 3 ){
        assert_true( fscanf( fd, "%d", &day ) == 1 );
        const int base = pustart[ uid ];
        if( day == first[ uid ] ){ 
            fprintf( fo, "1 %d:1\n", base ); continue;
        }
        if( day == last[ uid ] ){ 
            fprintf( fo, "1 %d:1\n", base+1 ); continue;
        }
        fprintf( fo, "2 %d:%f %d:%f\n", 
                 base  , static_cast<float>( last[uid] - day  )/(last[uid]-first[uid]),  
                 base+1, static_cast<float>( day - first[uid] )/(last[uid]-first[uid]) );        
    }
    fclose( fo );
    fclose( fd );
    fclose( fi );
    return 0;
}
