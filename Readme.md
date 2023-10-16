# Tunneler

---

## Introduction
Tunneler is a game for 2-8 players. Each player controls one tank with which they can dig tunnels,
shoot and move around the game map.The aim of the game is to find and eliminate all opponents.

## Concept
The program is divided into server part and client part. Communication takes place over TCP sockets.
The server's task is to establish a connection with a predetermined number of clients (max. 8) and start the game.
During the game, the server acts as a hub for distributing changes generated by individual clients.

Each of the clients controls one tank (in the case of splitscreen mode two) and at each impulse the timer sends its current status
to the server and receives a reply that also contains the current states of other players.
Based on this response, it changes the state of the game and displays the result.

## Input and output
The program uses the SDL library for displaying graphics, timer and keypress processing.
Tank control:
normal mode: <kbd>&#8592;</kbd> <kbd>&#8593;</kbd> <kbd>&#8594;</kbd> <kbd>&#8595;</kbd> - move, <kbd>Left CTRL</kbd> - shoot
splitscreen mode: 1st player - same as normal mode, 2nd player - <kbd>A</kbd> <kbd>S</kbd> <kbd>D</kbd> <kbd>W</kbd> - move, <kbd>Right CTRL</kbd> - shoot

## Compiling and Starting
To compile and run the client part of the program, the SDL library (freely available at www.libsdl.org) is required. 
Compiling should be possible in a reasonable Linux environment using the attached Makefile.

The game is executable in normal mode or in splitscreen mode, which allows two players game on one PC.
A test map (``new_map file``) is included.

First, you need to start the server, e.g.:

```
./server -m new_map -l 1 -c 3
```

(see server --help for details)

Then, you can start the client; the client requires the following parameters:
- ip server address
- map file name (it is assumed that all clients have the same map available)

```
./tunneler -a ip_address -m map_file
```

(see tunneler --help for details)

Example of Starting:
In one terminal, start:

```
#./server -m new_map
```

In another terminal, start:

```
#./tunneler -a 127.0.0.1 -m new_map -s
```


![screenshot][image]

[image]: screenshot.png
