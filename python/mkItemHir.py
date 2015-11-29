#!/usr/bin/python
import sys

# item taxonomy feature, all the tracks of same artist share a extra parameter which is not shared with the artist
def load_hirx( fname, fmap, idxmap, nidx, n ):
    fi = open( fname, 'r' )
    for line in fi:
        arr = line.split('|')
        iid = int( arr[0].strip() )        
        
        s =  arr[n].strip()
        if s != 'None':
            idx = int( s )
            if not idxmap.has_key( idx ):
                idxmap[ idx ] = nidx
                nidx += 1
            fmap[ iid ] = idxmap[ idx ]                  
    fi.close()    
    return fmap, idxmap, nidx

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print 'Usage:<input>'
        exit( -1 )
    num_item = 624961
    # item hirx
    fmap, idxmap, nidx = load_hirx( 'albumData1.txt', {}  , {}, num_item, 1 )
    fmap, idxmap, nidx = load_hirx( 'trackData1.txt', fmap, {}, nidx, 2 )

    fi = open( sys.argv[1], 'r' )
    fo = open( 'features/'+sys.argv[1]+'.ItemHir', 'w' )
    fo.write( '%d\n' % nidx)
    for line in fi:
        iid  =  int( line.split()[1] )
        if fmap.has_key( iid ):
            fo.write('2 %d:1 %d:1\n' % ( iid, fmap[iid] ) )
        else:
            fo.write('1 %d:1\n' % iid )

    fi.close()
    fo.close()
    print '%s ItemHir generation end, max index=%d' % ( sys.argv[1], nidx )
