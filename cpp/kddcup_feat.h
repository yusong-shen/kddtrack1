#include <cstdio>
#include <cstdlib>

inline FILE *fopen_check( const char *fname, const char *flag ){
    FILE *fi = fopen( fname, flag );
    if( fi == NULL ){
        fprintf( stderr, "can't open file %s\n", fname );
        exit( -1 );
    }
    return fi;
}
inline void assert_true( bool exp ){
    if( !exp ){
        fprintf( stderr, "assert error\n" );
        exit( -1 );
    }
}

inline void assert_true( bool exp, const char *msg ){
    if( !exp ){
        fprintf( stderr, "assert error:%s\n", msg );
        exit( -1 );
    }
}
