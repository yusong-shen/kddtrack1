/*
 *  Copyright 2009-2010 APEX Data & Knowledge Management Lab, Shanghai Jiao Tong University
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*!
 * \author Tianqi Chen: tqchen@apex.sjtu.edu.cn
 */
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <ctime>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <algorithm>

#include "apex_svd_data.h"
#include "apex-utils/apex_utils.h"

using namespace apex_svd;

struct Header{
    FILE *fi;
    int   tmp_num;
    int   base;
    int   num_feat;
};


inline int norm( std::vector<Header> &vec ){
    int n = 0;
    for( size_t i = 0; i < vec.size(); i ++ ){
        vec[i].base = n; n += vec[i].num_feat;
    }
    return n;        
}

inline void vclose( std::vector<Header> &vec ){
    for( size_t i = 0; i < vec.size(); i ++ ){
        fclose( vec[i].fi );
    }
}

inline int readnum( std::vector<Header> &vec ){
    int n = 0;
    for( size_t i = 0; i < vec.size(); i ++ ){
        apex_utils::assert_true( fscanf( vec[i].fi, "%d", &vec[i].tmp_num ) == 1, "load num" );
        n += vec[i].tmp_num;
    }
    return n;        
}

inline void restart( std::vector<Header> &vec ){
    for( size_t i = 0; i < vec.size(); i ++ ){
        fseek( vec[i].fi, 0, SEEK_SET );
        int num_feat;
        apex_utils::assert_true( fscanf( vec[i].fi, "%d", &num_feat ) == 1, "num feat" );
    }
}

class FeatureBlockLoader: public IDataIterator<SVDPlusBlock>{
private:
    // number of remaining lines to be loaded
    int nline_remain;
private:
    std::vector<unsigned> index_ufeedback;
    std::vector<float>    value_ufeedback; 
private:        
    std::vector<float> row_label;
    std::vector<int>   row_ptr;        
    std::vector<unsigned>   feat_index;
    std::vector<float>      feat_value;        
private:
    struct Elem{
        unsigned index;
        float    value;
        inline bool operator<( const Elem &b ) const{
            return index < b.index;
        }
    };

    inline void load( std::vector<Elem> &data, std::vector<Header> &vec ){
        for( size_t i = 0; i < vec.size(); i ++ ){
            for( int j = 0; j < vec[i].tmp_num; j ++ ){
                Elem e;
                apex_utils::assert_true( fscanf ( vec[i].fi, "%u:%f", &e.index, &e.value ) == 2, "load feat" );   
                e.index += vec[i].base;
                data.push_back( e );
            }
        }
        sort( data.begin(), data.end() );
    }

    inline void add( const std::vector<Elem> &vec ){
        for( size_t i = 0; i < vec.size(); i ++ ){
            feat_index.push_back( vec[i].index );
            feat_value.push_back( vec[i].value );
        } 
    }
public:
    FILE *fp, *finfo;
    std::vector< Header > fg, fu, fi;
public:
    int block_max_line;
    float scale_score;
    FeatureBlockLoader(){
        nline_remain = 0;
        scale_score = 100.0f;
        this->block_max_line = 10000; 
    }
    virtual void init( void ){}
    virtual void set_param( const char *name, const char *val ){}
    virtual void before_first(){}
    // load data into e, caller isn't responsible for space free
    // Loader will keep the space until next call of this function 
    virtual bool next( SVDPlusBlock &e ){        
        e.extend_tag = svdpp_tag::MIDDLE_TAG;
        // load from feedback file
        if( nline_remain == 0 ){
            int num_ufeedback;                        
            if( fscanf( finfo, "%d%d", &nline_remain, &num_ufeedback ) != 2 ) return false;
            index_ufeedback.resize( static_cast<size_t>( num_ufeedback ) );
            value_ufeedback.resize( static_cast<size_t>( num_ufeedback ) );
            for( int i = 0; i < num_ufeedback; i ++ ){
                apex_utils::assert_true( fscanf( finfo, "%d:%f", &index_ufeedback[i], &value_ufeedback[i] ) == 2, 
                                         "can't load implict feedback file" );
            }
            e.extend_tag = e.extend_tag & svdpp_tag::START_TAG;
        }
        
        // check lines to be loaded
        int num_line = nline_remain;
        if( nline_remain > block_max_line ){
            // smart arrangement to make data in similar size in each block
            int pc = ( nline_remain + block_max_line - 1 ) / block_max_line;
            num_line = ( nline_remain + pc - 1 ) / pc;
        }else{
            e.extend_tag = e.extend_tag & svdpp_tag::END_TAG;
        }
        this->nline_remain -= num_line;
        // set data 
        if( e.extend_tag == svdpp_tag::MIDDLE_TAG ){        
            e.num_ufeedback = 0;
            e.index_ufeedback = NULL;
            e.value_ufeedback = NULL;
        }else{
            e.num_ufeedback = static_cast<int>( index_ufeedback.size() );
            if( index_ufeedback.size() != 0 ){
                e.index_ufeedback = &index_ufeedback[0];
                e.value_ufeedback = &value_ufeedback[0];
            }
        }
        
        int num_elem = 0;
        row_label.resize( static_cast<size_t>( num_line ) );
        row_ptr.resize  ( static_cast<size_t>( num_line*3 + 1 ) );
        row_ptr[ 0 ] = 0;
        feat_index.clear();
        feat_value.clear();
        for( int i = 0; i < num_line; i ++ ){
            apex_utils::assert_true( fscanf( fp, "%*d%*d%f%*[^\n]\n", &row_label[i] ) == 1, "invalid feature format" );  
            row_label[ i ] /= scale_score;
            row_ptr[ i*3 + 1 ] = (num_elem += readnum(fg) );                
            row_ptr[ i*3 + 2 ] = (num_elem += readnum(fu) );
            row_ptr[ i*3 + 3 ] = (num_elem += readnum(fi) );
            std::vector<Elem> vg, vu, vi;
            this->load( vg, fg ); this->add( vg );
            this->load( vu, fu ); this->add( vu );
            this->load( vi, fi ); this->add( vi );
        }            
        e.data.num_row   = num_line;
        e.data.num_val   = num_elem;
        e.data.row_ptr   = &row_ptr[0];
        e.data.row_label = &row_label[0];
        e.data.feat_index= &feat_index[0];
        e.data.feat_value= &feat_value[0];
        
        return true;
    }
};

const char *folder = "features";
const char *fdsuffix = "imfb";
int main( int argc, char *argv[] ){
    if( argc < 3 ){
        printf("Usage:kddcup_combine_ugroup <inname> <outname> [options] -g [gf1] [gf2]... -u [uf1]... -i [itemf1] ..\n"\
               "options: -max_block max_block, -scale_score scale_score -fd feedback_suffix\n");
        return 0; 
    }
    FeatureBlockLoader loader;

    time_t start = time( NULL );
    int mode = 0;
    for( int i = 3; i < argc; i ++ ){        
        if( !strcmp( argv[i], "-g") ){
            mode = 0; continue;
        }
        if( !strcmp( argv[i], "-u") ){
            mode = 1; continue;
        }
        if( !strcmp( argv[i], "-i") ){
            mode = 2; continue;
        }
        if( !strcmp( argv[i], "-max_block") ){
            loader.block_max_line = atoi( argv[++i] ); continue;
        }
        if( !strcmp( argv[i], "-scale_score") ){
            loader.scale_score = (float)atof( argv[++i] ); continue;
        }
        if( !strcmp( argv[i], "-fd") ){
            fdsuffix = argv[ ++i ]; continue;
        }

        if( !strcmp( argv[i], "-skip") ){
            int skip = atoi( argv[++i] ); 
            switch( mode ){
            case 0: loader.fg.back().num_feat += skip; break;
            case 1: loader.fu.back().num_feat += skip; break;
            case 2: loader.fi.back().num_feat += skip; break;
            }
            continue;            
        }
        
        char name[ 256 ];
        sprintf( name, "%s/%s.%s", folder, argv[1], argv[i] );
        Header h;
        h.fi = apex_utils::fopen_check( name, "r" );
        apex_utils::assert_true( fscanf( h.fi, "%d", &h.num_feat ) == 1, "num feat" );
        switch( mode ){
        case 0: loader.fg.push_back( h ); break;
        case 1: loader.fu.push_back( h ); break;
        case 2: loader.fi.push_back( h ); break;            
        default: ;
        }                
    }
    {
        char name[ 256 ];
        sprintf( name, "%s.%s", argv[1], fdsuffix );
        loader.finfo = apex_utils::fopen_check( name   , "r" );
        loader.fp    = apex_utils::fopen_check( argv[1], "r" );
    }
    printf("num_global=%d, num_user=%d, num_item=%d\n", norm( loader.fg ), norm( loader.fu ), norm( loader.fi ) );
    
    printf("start creating buffer...\n");
    create_binary_buffer( argv[2], &loader );
    fclose( loader.fp );
    fclose( loader.finfo );
    vclose( loader.fg );
    vclose( loader.fu );
    vclose( loader.fi );
    printf("all generation end, %lu sec used\n", (unsigned long)(time(NULL) - start) );
    return 0;
}
