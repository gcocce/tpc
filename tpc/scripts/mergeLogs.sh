cat *.log | sort > unique.log

for i in $(ls | grep "^[0-9]*.log.*$");
do
	unlink $i
done
