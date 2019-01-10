#/bin/bash
PIDARRAY=()
for file in file1 file2
do
	md5sum $file &
	PIDARRAY+=("$!")
done
wait ${PIDARRAY[@]}
