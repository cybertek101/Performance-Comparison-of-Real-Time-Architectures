#!/bin/bash

#check input correctness
if [ "$#" -ne 1 ]; then
	echo "usage ./gnuplotscript.sh file";
	exit
fi

#modify input to suit processing
sed 's/(//g' < $1 > temp
sed 's/)//g' < temp > temp2
sed 's/,/, /g' < temp2 > temp
sed 's/,//g' < temp > temp2
sed -n '1,2 !p' < temp2 >temp
#mv temp2 temp

#get number of files to create
size=`cat temp | wc -l`

#write to each file
for ((i=1;i<=$size;++i));do
	awk '
		NR == '$i'{
				print $1,$3
				print $1,$4	
		}' < temp > output.$i
done

cp temp original
plotStr="plot 'original' t 'mean' w lines 0"

for ((i=1;i<=$size;++i));do
	plotStr=$plotStr",'output.$i' notitle w lines $i"
done

#file plotStr
echo $plotStr > plotStr

echo "to plot, run the following in gnuplot"
echo "*************************************"
echo $plotStr
echo "*************************************"

#initiate gnuplot
gnuplot
#clean up
#rm output.*
#rm temp*
