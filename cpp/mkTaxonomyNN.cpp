// cpp version of make user ID
#include <vector>
#include <algorithm>
#include "kddcup_feat.h"

const int NUM_ITEM  = 624961;
const int NUM_USER = 1000990;

// record of a users' history
class UserRecord{
private:
    struct Entry{
        FILE *fi;
        int   uid, iid, rate;
        Entry( const char *fname ){
            this->uid = -1; 
            fi = fopen_check( fname, "r" );
        }
        inline void init_rec( int uuid, int rec[ NUM_ITEM ], std::vector<int> &hist ){
            // find the user's history
            while( uid != uuid ){
                assert_true( fscanf( fi, "%d%d%d", &uid, &iid, &rate ) == 3 );            
            } 
            // doing record
            while( uid == uuid ){
                if( rec[ iid ] < 0 ){
                    hist.push_back( iid );
                    rec[ iid ] = rate;
                }
                if( fscanf( fi, "%d%d%d", &uid, &iid, &rate ) != 3 ) break; 
            }         
        }
    };
    int last_uid;
    std::vector<int> hist;
    std::vector<Entry> fp;
private:    
    // get avg score of current user
    inline float avg_score( bool smooth = true ){        
        double sum = 0.0f;        
        for( size_t i = 0; i < hist.size(); i ++ ){
            sum += rec[ hist[i] ];
        }
        if( smooth ){
            return static_cast<float>( ( sum + 49.0 ) / ( hist.size() + 1.0 ) );
        }else{
            return static_cast<float>( sum/ hist.size() );
        }
    }
public:
    float mean_score;
    int   rec[ NUM_ITEM ];   
public:
    UserRecord( int argc, char **argv ){ 
        std::fill( rec, rec + NUM_ITEM, -1 );
        last_uid = -1;
        for( int i = 0; i < argc; i ++ ){
            fp.push_back( Entry( argv[i] ));
        }
    }
    ~UserRecord(){ 
        for( size_t i = 0; i < fp.size(); i ++ ){
            fclose( fp[i].fi );
        }
    }
    inline void init_record( int uuid ) {
        if( last_uid == uuid ) return;
        last_uid = uuid;
        // clear history
        for( size_t i = 0; i < hist.size(); i ++ ){
            rec[ hist[i] ] = -1;
        } 
        hist.clear();
        for( size_t i = 0; i < fp.size(); i ++ ){
            fp[i].init_rec( uuid, rec, hist );
        }
        mean_score = this->avg_score();
    }
};

int parent[ NUM_ITEM ];
inline void load_taxonomy(  const char *fname ){
    FILE *fi = fopen_check( fname, "r" );
    for( int i = 0; i < NUM_ITEM; i ++ ){
        assert_true( fscanf( fi, "%d", &parent[i] ) == 1 );
    }
    fclose( fi );
}

const char *suffix = "";

int main( int argc, char *argv[] ){
    if( argc < 5 ){
        printf( "usage:<input> <taxomonyfile> <suffix> [infofiles...]\n"\
                "make item knn feature, infofile must be in the order of input so merge-sort style algorithm can be used\n" );
        return -1;
    }
    char fout[ 256 ];
    sprintf( fout, "features/%s.%s", argv[1], argv[3] );
    FILE *fi = fopen_check( argv[1], "r" );
    FILE *fo = fopen_check( fout, "w" );
    load_taxonomy( argv[2] );

    UserRecord urecord( argc - 4, argv + 4 );    
    // use iid as feature id, one for implicit and another for explicit
    fprintf( fo, "%d\n", NUM_ITEM );
    int uid, iid, rate;
    while( fscanf( fi, "%d%d%d", &uid, &iid, &rate ) == 3 ){
        urecord.init_record( uid );
        if( parent[iid] >= 0 && urecord.rec[ parent[iid] ] >= 0 ){            
            // output both explicit feedback and implicit feedback
            fprintf( fo, "1 %d:%f\n", iid, (urecord.rec[parent[iid]]-urecord.mean_score)/100.0f );
        }else{
            fprintf( fo, "0\n" ); 
        }
    }    
    fclose( fo );
    fclose( fi );
    return 0;
}
