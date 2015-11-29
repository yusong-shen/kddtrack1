#!/usr/bin/python
import sys

if __name__ == '__main__':    
    if len(sys.argv) < 3:
        print 'Usage: input output'
        exit(0)
    fi = open( sys.argv[1], 'r' )
    fo = open( sys.argv[2], 'w' )
    
    for line in fi:        
        if line.find('|') != -1:
            uid = line.split('|')[0]
        else:
            fo.write( uid + '\t' + '\t'.join( line.split( None , 2 )[0:2] ) + '\n' )    
    fi.close()
    fo.close()
