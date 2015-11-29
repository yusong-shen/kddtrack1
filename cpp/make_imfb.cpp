#include <cstdio>
#include <vector>
#include <algorithm>
#include <cmath>
#include "kddcup_feat.h"

using namespace std;

class CSRMatrix{
private:
    struct Entry{
        int y, x;
        Entry(){}
        Entry( int y, int x ){
            this->y = y; this->x = x;
        }
        inline static bool cmp( const Entry &a, const Entry &b ){
            if( a.y < b.y ) return true;
            if( a.y > b.y ) return false;
            return a.x < b.x;
        }
    };    
    vector<Entry> tmp;
    vector<int>   row_ptr;
    vector<int>   data;
public:
    inline int *operator[]( int y ){
        return &data[ row_ptr[ y ] ]; 
    }
    inline int row_len( int y ){
        return row_ptr[ y + 1 ] - row_ptr[ y ];
    }
    inline void add( int y, int x ){
        tmp.push_back( Entry( y, x ));
    }
    inline void build( void ){
        sort( tmp.begin(), tmp.end(), Entry::cmp );   
        int last_y = -1;        
        for( size_t i = 0; i < tmp.size(); i ++ ){
            if( tmp[i].y != last_y ){
                last_y = tmp[i].y;
                row_ptr.push_back( i );
            }
            data.push_back( tmp[i].x );
        }
        row_ptr.push_back( (int)tmp.size() );    
        tmp.clear();
    }
};

CSRMatrix data;
inline void load( const char *fname ){
    int uid, iid;
    FILE *fi = fopen_check( fname, "r" );
    while( fscanf( fi, "%d%d%*[^\n]\n", &uid, &iid ) == 2 ){
        data.add( uid, iid );
    }
    fclose( fi );
} 

inline void output_imfb( FILE *fo, int last_uid, int nline ){
    int *arr = data[ last_uid ];
    int nlen = data.row_len( last_uid );
    float inv= 1.0 / sqrtf( nlen );
    fprintf( fo, "%d %d", nline, nlen );
    
    for( int i = 0; i < nlen; i ++ ){
        fprintf( fo, " %d:%f", arr[i], inv );
    }  
    fprintf( fo, "\n" );
}

int main( int argc, char *argv[] ){
    if( argc < 3 ) {
        printf("usage:input output [data that will be used as imfb history..]\n");
        return 0;
    }

    for( int i = 3; i < argc; i ++ )
        load( argv[i] );
    data.build();

    int uid, last_uid = -1, nline = 0;    
    FILE *fi = fopen_check( argv[1], "r" );
    FILE *fo = fopen_check( argv[2], "w" );
    while( fscanf( fi, "%d%*[^\n]\n", &uid ) == 1 ){
        if( uid != last_uid ) {
            if( nline != 0 ){
                output_imfb( fo, last_uid, nline );
            }
            last_uid = uid; 
            nline = 0;
        }
        nline ++;
    }
    if( nline > 0 ) output_imfb( fo, last_uid, nline );
    fclose( fi );
    fclose( fo );
    return 0;
}
