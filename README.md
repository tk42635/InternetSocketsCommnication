InternetSocketsCommnication
================
Author: De Huo  
Email: dehuo@usc.edu

## Description of each file:  
__client.c:__ Get 3 arguments(mapID, starting node and file size) from users, query the AWS server for shortest paths to the other nodes in the same map and 3 kinds of delay time by using TCP.  
__aws.c:__ Get 3 parameters from client using TCP, propagate mapID and starting node to server A using UDP, get shortest paths to the other nodes from server A, propagate file size and shortest paths to server B using UDP, get 3 kinds of delay time from server B, return shortest paths and 3 kinds of delay time to client.  
__serverA.c:__ Get mapID and starting node from AWS using UDP, build all maps from map.txt, calculate shortest paths from starting node to the other nodes in specific map by using dijkstra algorithm,	return back the result to AWS.  
__serverB.c:__ Get file size and shortest paths from AWS, calculate 3 kinds of delay time for each path, return back the result to AWS.  
__aws.h, serverA.h, serverB.h:__ Include header files, define constant value, define struct, function prototype

## Format of all the messages exchanged:
   	1. client -> AWS
    	mapID: char* 
	starting node: int
	file size: long long

   	2. AWS -> server A
   	mapID: char* 
	starting node: int 
	
   	3. serverA -> AWS
	dist: int[], shortest paths to every node 
	propSpeed: double
	tranSpeed: double

   	4. AWS -> server B
   	file size: long long
	dist: int[], shortest paths to every node 
	propSpeed: double
	tranSpeed: double
	
	5. server B -> AWS
	tranDelay: double
	propDelay: double[], propagation delay to every node
	end2endDelay: double[], end-to-end delay delay to every node

	6. AWS -> client
	dist: int[], shortest paths to every node
	tranDelay: double
	propDelay: double[], propagation delay to every node
	end2endDelay: double[], end-to-end delay delay to every node

## Disclaimer
Some code about socket transmission are from "Beej's Guide to Network Programming -- Using Internet Sockets". 
