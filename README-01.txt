2D Asteroid
Author: Caleb Hanselman
Overview: My first attempt at making a coherent game in openGL. It resembles the classic 2d Asteroid game in most ways, but has some errors
such as resizing the screen will slightly change the bounds on where the game takes place. Further the codeing for this project is admitadely very sloppy. I have no class dependance and only one file it depends on, but this was more a small project for me to get the
the feeling of how to program a game in openGL. 
Features: All physics is done without using a physics library. Movement is smooth and feels like the classic game. 
Collision detection is mostly accurate, as well. Speed decays in a fashion similar to the original game
Controls: 
		w: moves the ship forward
		a: rotates the ship counter clockwise
		d: rotates the ship clock wise
		s: moves the ship backwards
		space: fires a bullet in the direction of the ship

Compile instructions: To compile on a unix based system make sure you have glut installed and then run the makefile. If it complains
about a lack of glut.h or glu.h on your system, check to make sure it is installed. Also some systems (FREEGLUT for archlinux) have to 
change #include <GL/glut.h> to #include<GL/glu.h>. I have no experience compiling this on windows, but it should work as long as the
libraries are included and can be reached.
