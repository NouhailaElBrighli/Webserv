while [ 1 ]
do
	leaks webserv | grep bytes
	sleep 2
	clear
done
