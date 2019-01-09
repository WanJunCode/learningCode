#/bin/bash
fpath=$1
if [ -e $fpath ];
then
	echo file exists
else
	echo file not exists
fi	
