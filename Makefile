SDL_FLAGS =`sdl-config --cflags --libs` 

tunneler : tunneler.cc game.cc tmap.cc camera.cc collisions.cc tank.cc structure.cc player.cc bullet.cc commondef.cc statusbar.cc painting.cc
	g++ -w -g -o tunneler tunneler.cc game.cc tmap.cc camera.cc collisions.cc tank.cc structure.cc player.cc bullet.cc commondef.cc statusbar.cc painting.cc ${SDL_FLAGS} 

server : server.cc commondef.cc mirror.cc initvect.cc 
	g++ -w -g -o server server.cc commondef.cc mirror.cc initvect.cc 
clean : 
	rm -f tunneler
	rm -f server
