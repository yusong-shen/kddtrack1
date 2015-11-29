#
# this makefile is used to generated data for KDDCup track1
# used by SVDFeature
#
.PHONY: clean all

all:
	rm -rf *~

# abbrv
UserID= features/test.txt.UserID features/train.group.UserID
ItemID= features/test.txt.ItemID features/train.group.ItemID
ItemHir= features/test.txt.ItemHir features/train.group.ItemHir
ItemDayBin  = features/test.txt.ItemDayBin features/train.group.ItemDayBin
UserDayBias = features/test.txt.UserDayBias features/train.group.UserDayBias
UserDayInterpolation = features/test.txt.UserDayInterpolation features/train.group.UserDayInterpolation
TNNArtist= features/test.txt.TNNArtist features/train.group.TNNArtist
UserSessionBias= features/test.txt.UserSessionBias features/train.group.UserSessionBias

#
# this file specifies the generation dependecies of 
# the experiment, to read the experiment procedure
# please read them by orders in the file
# the rule is ordered so that the files in the later part only depends on the previous ones 

# rating matrix 
train.txt: trainIdx1.txt
	python/convertRateMatrix.py trainIdx1.txt train.txt
validation.txt: validationIdx1.txt
	python/convertRateMatrix.py validationIdx1.txt validation.txt
test.txt: track1.test.txt
	python/convertRateMatrix.py track1.test.txt test.txt
# triple validation, importance sampling
train3v.txt: train.txt validation.txt
	cat train.txt validation.txt validation.txt validation.txt > train3v.txt
# user grouped random order file, used to generate user grouped format
train.svdpp.order: train3v.txt
	svd-feature/svdpp_randorder train3v.txt train.svdpp.order	
# training data reordered in user grouped format
train.group: train3v.txt train.svdpp.order 
	svd-feature/line_reorder train3v.txt train.svdpp.order train.group
# implicit feedback file
train.group.imfb: train.group train.txt validation.txt test.txt
	cpp/make_imfb train.group train.group.imfb train.txt validation.txt test.txt
test.txt.imfb: train.txt validation.txt test.txt
	cpp/make_imfb test.txt test.txt.imfb train.txt validation.txt test.txt

#---------------------------
# auxiliary files 
#--------------------------
# time information file, *.day store the day information, *.userdayidx store the consecutive index of the day in the sorted user's day list, 
# *.clock store the clock information in a day, converted into minutes, 
# user_days.txt stores user id, the start day of user's rate, the last day of user's rating, number of different days in users' rating
train.day train.userdayidx train.clock validation.day validation.userdayidx validation.clock test.day test.userdayidx test.clock user_daystats.txt: trainIdx1.txt validationIdx1.txt track1.test.txt
	python/convertTime.py 
#clock information of user's overlapping day between validation and test data 
overlapuser_clocks.txt: trainIdx1.txt validationIdx1.txt track1.test.txt 
	python/makeOverlapClock.py
train.group.day: train.day validation.day train.svdpp.order 
	cat train.day validation.day validation.day validation.day > train3v.day 
	svd-feature/line_reorder train3v.day train.svdpp.order train.group.day
	rm train3v.day
train.group.userdayidx: train.userdayidx validation.userdayidx train.svdpp.order 
	cat train.userdayidx validation.userdayidx validation.userdayidx validation.userdayidx > train3v.userdayidx
	svd-feature/line_reorder train3v.userdayidx train.svdpp.order train.group.userdayidx
	rm train3v.userdayidx
train.group.clock: train.day validation.clock train.svdpp.order 
	cat train.clock validation.clock validation.clock validation.clock > train3v.clock
	svd-feature/line_reorder train3v.clock train.svdpp.order train.group.clock
	rm train3v.clock
item2artist.txt item2album.txt: trackData1.txt albumData1.txt 
	python/makeItemParent.py

# Feature files, we use cpp to faster the generation
# UserID
features/test.txt.UserID: test.txt 
	cpp/mkUserID test.txt 
features/train.group.UserID: train.group 
	cpp/mkUserID train.group
# ItemID
features/test.txt.ItemID: test.txt 
	cpp/mkItemID test.txt
features/train.group.ItemID: train.group 
	cpp/mkItemID train.group
# ItemHir
features/test.txt.ItemHir: test.txt trackData1.txt albumData1.txt
	python/mkItemHir.py test.txt
features/train.group.ItemHir: train.group trackData1.txt albumData1.txt
	python/mkItemHir.py train.group
# ItemDayBin
features/test.txt.ItemDayBin: test.txt test.day
	cpp/mkItemDayBin test.txt test.day
features/train.group.ItemDayBin: train.group train.group.day
	cpp/mkItemDayBin train.group train.group.day
# UserDayBias
features/test.txt.UserDayBias: test.txt test.userdayidx user_daystats.txt
	cpp/mkUserDayBias test.txt test.userdayidx
features/train.group.UserDayBias: train.group train.group.userdayidx user_daystats.txt
	cpp/mkUserDayBias train.group train.group.userdayidx
# UserDayInterpolation
features/test.txt.UserDayInterpolation: test.txt test.day
	cpp/mkUserDayInterpolation test.txt test.day
features/train.group.UserDayInterpolation: train.group train.group.day
	cpp/mkUserDayInterpolation train.group train.group.day
# Taxonomy parent History rating, use artist to help track/album
features/test.txt.TNNArtist: test.txt train.txt validation.txt item2artist.txt 
	cpp/mkTaxonomyNN test.txt item2artist.txt TNNArtist train.txt validation.txt
features/train.group.TNNArtist: train.group item2artist.txt 
	cpp/mkTaxonomyNN train.group item2artist.txt TNNArtist train.group
# UserSessionBias
features/test.txt.UserSessionBias: test.txt test.day test.clock overlapuser_clocks.txt
	python/mkUserSessionBias.py test.txt test.day test.clock
features/train.group.UserSessionBias: train.group train.group.day train.group.clock overlapuser_clocks.txt
	python/mkUserSessionBias.py train.group train.group.day train.group.clock

# basic svd++
buffer.train.svdpp: train.group train.group.imfb $(UserID) $(ItemID) 
	utils/kddcup_combine_ugroup train.group buffer.train.svdpp -u UserID -i ItemID -max_block 4000
buffer.test.svdpp: test.txt test.txt.imfb $(UserID) $(ItemID) 
	utils/kddcup_combine_ugroup test.txt buffer.test.svdpp -u UserID -i ItemID -max_block 4000

# svd++ with item time bin
buffer.train.svdpp.itime: train.group train.group.imfb $(UserID) $(ItemID) $(ItemDayBin)
	utils/kddcup_combine_ugroup train.group buffer.train.svdpp.itime -g ItemDayBin -u UserID -i ItemID -max_block 4000
buffer.test.svdpp.itime: test.txt test.txt.imfb $(UserID) $(ItemID) $(ItemDayBin)
	utils/kddcup_combine_ugroup test.txt buffer.test.svdpp.itime -g ItemDayBin -u UserID -i ItemID -max_block 4000

# svd++ with item time bin, user day bias
buffer.train.svdpp.iutime: train.group train.group.imfb $(UserID) $(ItemID) $(ItemDayBin) $(UserDayBias)
	utils/kddcup_combine_ugroup train.group buffer.train.svdpp.iutime -g ItemDayBin UserDayBias -u UserID -i ItemID -max_block 4000
buffer.test.svdpp.iutime: test.txt test.txt.imfb $(UserID) $(ItemID) $(ItemDayBin) $(UserDayBias)
	utils/kddcup_combine_ugroup test.txt buffer.test.svdpp.iutime -g ItemDayBin UserDayBias -u UserID -i ItemID -max_block 4000

# svd++ with item time bin, user day bias, user temporal factor
buffer.train.svdpp.iutime.put: train.group train.group.imfb $(UserID) $(ItemID) $(ItemDayBin) $(UserDayBias) $(UserDayInterpolation)
	utils/kddcup_combine_ugroup train.group buffer.train.svdpp.iutime.put -g ItemDayBin UserDayBias -u UserID UserDayInterpolation -i ItemID -max_block 4000
buffer.test.svdpp.iutime.put: test.txt test.txt.imfb $(UserID) $(ItemID) $(ItemDayBin) $(UserDayBias) $(UserDayInterpolation)
	utils/kddcup_combine_ugroup test.txt buffer.test.svdpp.iutime.put -g ItemDayBin UserDayBias -u UserID UserDayInterpolation -i ItemID -max_block 4000

# svd++ with item time bin, user day bias, user temporal factor, taxonomy parent neighborhood
buffer.train.svdpp.iutime.put.tnn: train.group train.group.imfb $(UserID) $(ItemID) $(ItemDayBin) $(UserDayBias) $(UserDayInterpolation) $(TNNArtist)
	utils/kddcup_combine_ugroup train.group buffer.train.svdpp.iutime.put.tnn -g ItemDayBin UserDayBias TNNArtist -u UserID UserDayInterpolation -i ItemID -max_block 4000
buffer.test.svdpp.iutime.put.tnn: test.txt test.txt.imfb $(UserID) $(ItemID) $(ItemDayBin) $(UserDayBias) $(UserDayInterpolation) $(TNNArtist)
	utils/kddcup_combine_ugroup test.txt buffer.test.svdpp.iutime.put.tnn -g ItemDayBin UserDayBias TNNArtist -u UserID UserDayInterpolation -i ItemID -max_block 4000

# svd++ with item time bin, user day bias, user temporal factor, taxonomy parent neighborhood, taxonomy factorization
buffer.train.svdpph.iutime.put.tnn: train.group train.group.imfb $(UserID) $(ItemHir) $(ItemDayBin) $(UserDayBias) $(UserDayInterpolation) $(TNNArtist)
	utils/kddcup_combine_ugroup train.group buffer.train.svdpph.iutime.put.tnn -g ItemDayBin UserDayBias TNNArtist -u UserID UserDayInterpolation -i ItemHir -max_block 4000
buffer.test.svdpph.iutime.put.tnn: test.txt test.txt.imfb $(UserID) $(ItemHir) $(ItemDayBin) $(UserDayBias) $(UserDayInterpolation) $(TNNArtist)
	utils/kddcup_combine_ugroup test.txt buffer.test.svdpph.iutime.put.tnn -g ItemDayBin UserDayBias TNNArtist -u UserID UserDayInterpolation -i ItemHir -max_block 4000


# svd++ with item time bin, user day bias, user temporal factor, taxonomy parent neighborhood, taxonomy fator user session bias
buffer.train.svdpph.iutime.put.tnn.us: train.group train.group.imfb $(UserID) $(ItemHir) $(ItemDayBin) $(UserDayBias) $(UserDayInterpolation) $(TNNArtist) $(UserSessionBias)
	utils/kddcup_combine_ugroup train.group buffer.train.svdpph.iutime.put.tnn.us -g ItemDayBin UserDayBias TNNArtist UserSessionBias -u UserID UserDayInterpolation -i ItemHir -max_block 4000
buffer.test.svdpph.iutime.put.tnn.us: test.txt test.txt.imfb $(UserID) $(ItemHir) $(ItemDayBin) $(UserDayBias) $(UserDayInterpolation) $(TNNArtist) $(UserSessionBias)
	utils/kddcup_combine_ugroup test.txt buffer.test.svdpph.iutime.put.tnn.us -g ItemDayBin UserDayBias TNNArtist UserSessionBias -u UserID UserDayInterpolation -i ItemHir -max_block 4000

