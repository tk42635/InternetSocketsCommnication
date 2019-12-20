
all:
		g++ -o client client.cpp  -lnsl -lresolv
		g++ -o aws aws.cpp aws.h  -lnsl -lresolv
		g++ -o serverA serverA.cpp serverA.h  -lnsl -lresolv
		g++ -o serverB serverB.cpp serverB.h  -lnsl -lresolv

.PHONY: serverA
serverA:
		./serverA

.PHONY: serverB
serverB:
		./serverB

.PHONY: aws
aws:
		./aws
