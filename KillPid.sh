exec $fd< "${ProcessPID.txt}"
while read LINE 
do 
	echo $LINE
        kill -9 $LINE
done < "ProcessPID.txt"
#exec $fd<&-
echo "finished script"
