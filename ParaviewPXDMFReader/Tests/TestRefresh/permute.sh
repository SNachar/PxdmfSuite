#!/bin/bash

testfile=testdata.pxdmf
file1=data1fieldsfewModes.pxdmf
file2=data2fieldsManyModes.pxdmf

if [ -a $testfile ]
then
 if [ -h $testfile ]
 then
 	 if [ $file1 -ef $testfile ]
 	 then 
	 	rm $testfile
		ln -s $file2 $testfile
		cp $file2 hard_$testfile
		echo "changing to $file2 "
     else
	 	rm $testfile
		ln -s $file1 $testfile
		cp $file1 hard_$testfile
		echo "changing to $file1 "
			 
	 fi
 
 else
    echo "file $testfile is not a symbolic link"
 fi
else 
 echo "no file $testfile, creating  file"
 ln -s $file1 $testfile 
fi
 
