#!/usr/bin/python
import sys

def load_hir( fname, fmap, pos ):
    fi = open( fname , 'r' )
    for line in fi:
        arr = line.split('|')
        iid = int( arr[0].strip() )            
        k = arr[ pos ].strip()
        if k != 'None':
            fmap[ iid ] = int( k )
    fi.close()        
    return fmap

def write_hir( fname, fmap, nitem ):
    fo = open( fname, 'w' )  
    for i in xrange( nitem ):
        if fmap.has_key( i ):
            fo.write( '%d\n' % fmap[i] )
        else:
            fo.write( '-1\n' )
    fo.close()

if __name__ == '__main__':
    num_item = 624961
    # item hir
    fmap = load_hir( 'albumData1.txt', {}  , 1 )
    fmap = load_hir( 'trackData1.txt', fmap, 2 )
    write_hir( 'item2artist.txt', fmap, num_item )

    fmap = load_hir( 'trackData1.txt', fmap, 1 )
    write_hir( 'item2album.txt', fmap, num_item )
    
