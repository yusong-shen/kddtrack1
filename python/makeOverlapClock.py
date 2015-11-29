#!/usr/bin/python

def getClock(fi, overlapDay, clockMap):
    line = fi.readline()
    cnt = int(line.strip().split('|')[1])
    isOverlap = 0
    for i in xrange(0, cnt):
	attr = fi.readline().strip().split(None)
	day = int(attr[2])
	clock = int(attr[3].split(':')[0])*60 + int(attr[3].split(':')[1])
	if day == overlapDay:
	    isOverlap = 1
	    if not clockMap.has_key(clock):
		clockMap[clock] = 1
    return clockMap, isOverlap

if __name__ == '__main__':    
    ftrain = open('trainIdx1.txt' , 'r' )
    fvalidation = open('validationIdx1.txt' , 'r' )
    ftest = open('track1.test.txt' , 'r' )
    #store the clock information of user's overlapping day between validation and test data 
    fo = open( 'overlapuser_clocks.txt', 'w' )
    
    for uid in xrange(0, 1000990):
	clockMap = {}
	line = ftest.readline()
	cnt = int(line.strip().split('|')[1])
	attr = ftest.readline().strip().split(None)
	overlapDay = int(attr[2])
	clock = int(attr[3].split(':')[0])*60 + int(attr[3].split(':')[1])
	clockMap[clock] = 1
	for i in xrange(1, cnt):
	    attr = ftest.readline().strip().split(None)
	    day = int(attr[2])
	    if day != overlapDay:
		continue
	    else:
	        clock = int(attr[3].split(':')[0])*60 + int(attr[3].split(':')[1])
		if not clockMap.has_key(clock):
		    clockMap[clock] = 1
	clockMap, isOverlap = getClock(fvalidation, overlapDay, clockMap) 
	if isOverlap == 1:
	    clockMap, isOverlap = getClock(ftrain, overlapDay, clockMap)
	    clockList = sorted(clockMap.keys())
	    fo.write('%d\t%d' %(uid, overlapDay))
	    for c in clockList:
		fo.write('\t%d' %c)
	    fo.write('\n')
	else:
	    cnt = int(ftrain.readline().strip().split('|')[1])	
	    for i in xrange(0, cnt):
		ftrain.readline()	
    ftrain.close()
    fvalidation.close()
    ftest.close()
    fo.close()
