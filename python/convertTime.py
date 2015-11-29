#!/usr/bin/python

def getTime(fi,fclock, dayList):
    line = fi.readline()
    cnt = int(line.strip().split('|')[1])
    days = []
    for i in xrange(0, cnt):
	line = fi.readline()
	attr = line.strip().split(None)
	day = int( attr[2] )
	clockstr = attr[3]
	days.append(day)
	#convert to minute
	clock = int(clockstr.split(':')[0])*60 + int(clockstr.split(':')[1])
	fclock.write(str(clock)+'\n')
	if not dayList.has_key(day):
	    dayList[day] = 1
    return dayList,  days

if __name__ == '__main__':    
    ftrain = open( 'trainIdx1.txt', 'r' )
    fvalidation= open( 'validationIdx1.txt', 'r' )
    ftest = open( 'track1.test.txt', 'r' )
    #write the day information to a separate file
    fdayTrain = open('train.day','w')
    fdayValidation = open('validation.day','w')
    fdayTest = open('test.day','w')
    #get the index of day in user's day list and write them to a separate file
    fdayIdxTrain = open('train.userdayidx','w')
    fdayIdxValidation = open('validation.userdayidx','w')
    fdayIdxTest = open('test.userdayidx','w')
    #write the clock information of a day into a separate file, converted into minutes
    fclockTrain = open('train.clock','w')
    fclockValidation = open('validation.clock','w')
    fclockTest = open('test.clock','w')
    #store user's day list
    fuserday = open('user_daystats.txt','w')

    for uid in xrange( 0, 1000990 ):
	dayList = {}
	dayList, trainDays = getTime(ftrain, fclockTrain, dayList)   
	dayList, validationDays = getTime(fvalidation, fclockValidation, dayList)   
	dayList, testDays = getTime(ftest, fclockTest, dayList) 
        
	l = sorted( dayList.keys() )    
	fuserday.write( '%d\t%d\t%d\t%d\n' % ( uid, l[0], l[-1], len(l) ) )
	for idx in xrange( len(l) ):
            dayList[ l[idx] ] = idx
            
	for day in trainDays:
	    dayIdx = dayList[day] 
	    fdayIdxTrain.write(str(dayIdx)+'\n')
	    fdayTrain.write(str(day)+'\n')
	for day in validationDays:
	    dayIdx = dayList[day] 
	    fdayIdxValidation.write(str(dayIdx)+'\n')
	    fdayValidation.write(str(day)+'\n')
	for day in testDays:
	    dayIdx = dayList[day]
	    fdayIdxTest.write(str(dayIdx)+'\n')
	    fdayTest.write(str(day)+'\n')
    ftrain.close()
    fvalidation.close()
    ftest.close()
    fdayTrain.close()
    fdayValidation.close()
    fdayTest.close()
    fclockTrain.close()
    fclockValidation.close()
    fclockTest.close()
    fdayIdxTrain.close()
    fdayIdxValidation.close()
    fdayIdxTest.close()
    fuserday.close()
