#!/usr/bin/python
import sys

if __name__ == '__main__':
    if len(sys.argv) < 4:
        print 'Usage:<input> <input.day> <input.clock>'
        exit( -1 )    
    # load clock index information
    order_map = {}
    cnt = 0
    fi = open( 'overlapuser_clocks.txt', 'r' )
    for line in fi:
        info = line.strip().split('\t')
        uid = int(info[0])
        day = int(info[1])
        for i in range(2,len(info)):
            order_map[(uid,day,int(info[i]))] = cnt
            cnt += 1
    fi.close()
    print 'num of clock bias: %d' %(cnt)
    
    # start generation
    fi = open( sys.argv[1], 'r' )
    fd = open( sys.argv[2], 'r' )
    fc = open( sys.argv[3], 'r' )
    fo = open( 'features/'+sys.argv[1]+'.UserSessionBias', 'w' )
    fo.write( '%d\n' % cnt )
    for line in fi:
        uid = int( line.split()[0] )
        day = int( fd.readline() )
        clk = int( fc.readline() )
        if order_map.has_key( (uid,day,clk) ):
            fo.write('1 %d:1\n' % order_map[(uid,day,clk) ] )
        else:
            fo.write('0\n')
        
    fi.close()
    fo.close()
    fd.close()
    fc.close()
    print '%s UserSessionBias generation end' % sys.argv[1]

