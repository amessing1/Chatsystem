all:
#
	#Compiling chat program...
	#######################
	gcc client.c -lSDL2 -lSDL2_ttf -lpthread -o client
	#######################
	gcc server.c -o server
	#######################

clean:
	@rm client
	@rm server