#!/usr/bin/python
import sys
import os
if len(sys.argv) < 3:
    print 'Usage:<filename> <nround> [checkterm] [expr]'
    exit(-1)

if len(sys.argv) > 3:
    checkterm = sys.argv[3]
else:
    checkterm = 'model_save'

if len(sys.argv) > 4:
    expr = sys.argv[4]
else:
    expr = '='


nrnd = int( sys.argv[2] )
fi = open( sys.argv[1], 'r' )


hit = 0
for line in fi:
    if line.find('=') == -1:
        continue
    arr = line.split('=')
    if arr[0].strip() == checkterm:        
        if expr == '=':
            if nrnd == int( arr[1].strip().strip('"') ):
                hit = 1
        elif expr == '>':
            if nrnd > int( arr[1].strip().strip('"') ):
                hit = 1
        elif expr == '<':
            if nrnd < int( arr[1].strip().strip('"') ):
                hit = 1

fi.close()
print hit



