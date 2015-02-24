while [ true ]
do
	clang -g -Wall -Wpedantic freq.c && cat file.txt | ./a.out
	sleep 2
done
