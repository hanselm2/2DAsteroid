#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>//need to change this include to #include <GL/GLUT.h> for xcode. and relevant glut library for linux
#include <iostream>
#include <vector>
#include <stdio.h>
#include <cmath>
#include <ctime>
#include <cstdlib>
using std::vector;
using namespace::std;

#define PI 3.14159265

//i messed up and wrote a bunch without realizing i couldnt just have one position coordinate or not that i know of yet, so 
//ship1 will be the ships x position, and ship 5 will be its y position
float ship [5] = {0,0,0,0,0}; // its position, angle, speed in the direction its facing, and its rotation velocity
int count3=0;
int count4=0;

float bullRad = .01;
float bullvel = .05;
int nFPS = 60;
int level=0;
bool startLevel = false;

/* **** HELPER FUNCTIONS DECLARED HERE **** */

bool randomBool()
{
    return ((rand() % 2) == 1);
}

float randomFloat()
{
    if ( randomBool() )
        return (float)rand() / ((float)RAND_MAX);
    else return -( (float)rand() / ((float)RAND_MAX) );
}

float distance2D(float firstx, float firsty, float secondx, float secondy)
{
    float distance = sqrt( (firstx-secondx)*(firstx-secondx) + (firsty-secondy)*(firsty-secondy) );
    return distance;
}

bool getStart()
{   cout<<"Would you like to play? : Y/N"<<endl;
    char ans;
    cin>>ans;
    if(ans != 'Y' && ans != 'y' && ans != 'N' && ans != 'n'){
        cout<<"incorrect selection please try again"<<endl;
        getStart();
    }
    if( ans =='N' || ans =='n' ) exit(0);
    return (ans == 'Y' || ans == 'y');
}

void init(void)
{
    // init your data, setup OpenGL environment here
    glClearColor(0.3,0.3,0.3,1.0); // clear color is gray       
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // uncomment this function if you only want to draw wireframe model

}

/* ****ANY STRUCTS OR CLASSES USED WILL BE DECLARED HERE*** */

//the variable type that will be used to store the position of the bullets
//x and y pos are the position of the bullet: ray is the line the bullet will follow: and time is a counter of how many 
//frames it has been on the screen for
struct bullet {
    float xpos;
    float ypos;
    float ray;
    int time;
};

struct asteroid{
    float xpos;
    float ypos;
    float ray;
    int health;
    float velocity;
    float radius;   // base case radius
};

vector<bullet*> bullets;
vector<asteroid*> asteroids;




/* **** ANY DRAW FUNCTIONS OR ANIMATIONS WILL BE DECLARED HERE **** */

void drawship()
{
        //draw our own ship using triangles
        glColor3f(1.0, 0.0, 0.0);    //draw our ship to be red
        glBegin(GL_TRIANGLE_STRIP);
            glVertex2f(-.0625,-.125);
            glVertex2f(0.f,-.0625);
            glVertex2f(-0.f,.125);
            glVertex2f(.0625,-.125);
        glEnd();
}

//NOTE: color is NOT drawn:  must declare own color
void drawCircle(float radius, float xposit, float yposit) 
{
    //internet says 12 polygons is maximum number of polygons needed
    glBegin( GL_POLYGON);
        for(double i=0; i < 2*PI; i+= PI/12.0)
            glVertex3f( (cos(i)*radius)+xposit, (sin(i)*radius)+yposit, 0.0);
    glEnd();
}

//Our drawbullet function is self updating: IE the "Physics" of the bullets are contained in here
void drawBullets()
{
    glColor3f(1.0, 1.0, 1.0);    //our  bullets will be solid white
    for(unsigned int i=0; i<bullets.size(); i++)
    {
        bullet* _bullet = bullets[i];
        drawCircle(bullRad, _bullet->xpos, _bullet->ypos);
        //Update the cordinate : "Physics"
        _bullet->xpos += cos( (_bullet->ray + 90)*PI/180.0 ) * bullvel;
        _bullet->ypos += sin( (_bullet->ray + 90)*PI/180.0 ) * bullvel;
        _bullet->time++;
        if(_bullet->time>180) //been on the screen for 3 seconds
        {   bullets.erase(bullets.begin() + i);
            delete _bullet;
        }
        else{
            if(_bullet->xpos > 2.1 || _bullet->xpos < -2.1) _bullet->xpos = -_bullet->xpos;
            if(_bullet->ypos > 2.1 || _bullet->ypos < -2.1) _bullet->ypos = -_bullet->ypos;
        }
    }
}

void bulletAsterColis(asteroid* & test)
{
    for(unsigned int i=0; i<bullets.size(); i++)
    {      
        bullet * _bullet = bullets[i];
        float dist = distance2D(test->xpos, test->ypos, bullets[i]->xpos, bullets[i]->ypos);
        if (dist < (bullRad + test->radius * (test->health+1)/2) )    //collision occurs
        {
            test->health --;
            
            //now the asteroid slows down and its direction may change
            
            //used to calculate the new ray's angle 
            float newx = ( (bullvel * cos((_bullet->ray + 90)*PI/180.0))*.3 + (test->velocity * cos((test->ray)*PI/180.0)) );
            float newy = ( (bullvel * sin((_bullet->ray + 90)*PI/180.0))*.3 + (test->velocity * sin((test->ray)*PI/180.0)) );
            
            float theta = atan2(newy,newx) * 180/PI;
            if( theta < 0) theta += 360;
            test->ray = theta;

            float veloc = sqrt( (newx*newx) + (newy*newy) );
            if(veloc > .5) test->velocity = .5;
            else test->velocity = veloc;

            //i dont think this will cause a segfault in the bullets array, im maintaining it by using erase first
            bullets.erase(bullets.begin() + i);
            delete _bullet;
        }
    }
}

void drawAsteroids()
{
    glColor3f(.5, .35, .5);
    for(unsigned int i=0; i<asteroids.size(); i++)
    {
        //check for colisions with bullets if there is one resolve it
        asteroid* _asteroid = asteroids[i];
        bulletAsterColis(_asteroid);
        
        if(_asteroid->health > 0)
        {   drawCircle( (_asteroid->radius * (_asteroid->health+1)/2), _asteroid->xpos, _asteroid->ypos);
            //update the cordinates
            _asteroid->xpos += cos( (_asteroid->ray)*PI/180.0 ) * _asteroid->velocity;
            _asteroid->ypos += sin( (_asteroid->ray)*PI/180.0 ) * _asteroid->velocity;
            
            if(_asteroid->xpos>2.1 || _asteroid->xpos<-2.1) _asteroid->xpos = -_asteroid->xpos/1.005;
            if(_asteroid->ypos>2.1 || _asteroid->ypos<-2.1) _asteroid->ypos = -_asteroid->ypos/1.005;
        }

        else if(_asteroid->health == 0)
        {   asteroids.erase(asteroids.begin()+i);
            delete _asteroid;
        }

        else cout<<"error in the health of the asteroids after colision"<<endl;
    }


}



/* **** ALL PHYSICS AND ACTUAL BUILDING OF THE GRAPHICS WILL BE BELOW **** */

//inneffecient algorithm and only applicibable to simple polygons, could add a helper function to find vertices but 
//i have already worked out most of the algorithm for the ships vertices and thus this function cannot port to other
//applictations
bool shipAsterColis()
{
    float distance, colisionDist;
    for( unsigned int i=0; i<asteroids.size(); i++)
    {   
        asteroid* test= asteroids[i];
        distance = distance2D(test->xpos, test->ypos, ship[1], ship[5]);
        
        
        /* OLD TRY failed because it would blow up at edge cases
        //find the vector between the center of the stroid and the ship
        float vectx = (test->xpos - ship[1]);
        float vecty = (test->ypos - ship[5]);
        theta = atan2(vecty, vectx) * 180/PI; // will give the angle of the vector up to 180
        theta -= ship[2];   //correct for the ships orientation

        //cout<<"vectx = "<<vectx<<"\t vecty = "<<vecty<<"\t theta = "<<theta<<endl;

        if(theta < 0) theta += 360;

        if(theta <= 90 && theta > -63.5)
            colisionDist = abs( (.125/( sin(theta*PI/180) + 4*cos(theta*PI/180))) )+(test->radius * (test->health+1)/2);
        else if(theta <= 243.4 && theta >90)
            colisionDist = abs(.125/( sin(theta*PI/180) - 4*cos(theta*PI/180) ))+(test->radius * (test->health+1)/2);
        else 
            colisionDist = abs(.125/sin(theta*PI/180))+(test->radius * (test->health+1)/2) ;
        */
    
        colisionDist = (test->radius * (test->health+1)/2) + .09; // giving the ship a .1 radius circle hitbox

        if(distance <= colisionDist)
            return true;
    }
    
    return false;
}


//level/2 at 5 health
void makeAsteroids(int level)
{
    for(int i=0; i<4+level; i++)
    {   asteroid * newAsteroid = new asteroid;
        newAsteroid->ray = ( randomFloat() * 360 );     //well use the ray to determine a circle outside of a safe zone
        
        newAsteroid->xpos = ( randomFloat() * 1.5 );
        if(newAsteroid->xpos < 0) newAsteroid->xpos -= 1;
        else newAsteroid->xpos += 1;

        newAsteroid->ypos = ( randomFloat() * 1.5 ) + 1;
        if(newAsteroid->ypos < 0) newAsteroid->ypos -= 1;
        else newAsteroid->ypos += 1;              

        newAsteroid->velocity = ( randomFloat() * .05 ) + .02;  //asteroids will move at least .02 units per frame
        newAsteroid->health = 3;
        newAsteroid->radius = 0.1;
        asteroids.push_back(newAsteroid);
    }
    
    if( level > 1)
    {   for(int i=0; i<level-1; i++)
        {   asteroid * newAsteroid = new asteroid;
            newAsteroid->ray = ( randomFloat() * 360 );     //well use the ray to determine a circle outside of a safe zone
            
            newAsteroid->xpos = ( randomFloat() * 1.5 );
            if(newAsteroid->xpos < 0) newAsteroid->xpos -= 1;
            else newAsteroid->xpos += 1;

            newAsteroid->ypos = ( randomFloat() * 1.5 ) + 1;
            if(newAsteroid->ypos < 0) newAsteroid->ypos -= 1;
            else newAsteroid->ypos += 1;

            newAsteroid->velocity = ( randomFloat() * .05 ) + .0175;  //asteroids will move at least .02 units per frame
            newAsteroid->health = 4;
            newAsteroid->radius = 0.1;
            asteroids.push_back(newAsteroid);
        }
    }
    
    if( (level-1)/2 != 0)
    {   for(int i=0; i<level-1; i++)
        {   asteroid * newAsteroid = new asteroid;
            newAsteroid->ray = ( randomFloat() * 360 );     //well use the ray to determine a circle outside of a safe zone
            
             newAsteroid->xpos = ( randomFloat() * 1.5 );
             if(newAsteroid->xpos < 0) newAsteroid->xpos -= 1;
             else newAsteroid->xpos += 1;                        

             newAsteroid->ypos = ( randomFloat() * 1.5 ) + 1;
             if(newAsteroid->ypos < 0) newAsteroid->ypos -= 1;
             else newAsteroid->ypos += 1;       
            
            newAsteroid->velocity = ( randomFloat() * .05 ) + .015;  //asteroids will move at least .02 units per frame
            newAsteroid->health = 5;
            newAsteroid->radius = 0.1;
            asteroids.push_back(newAsteroid);
        }
    }
}

bool levelclear()
{
    return asteroids.empty();
}

void beginGame()
{
    for(int i=0; i<5; i++)
        ship[i] = 0;
    for(unsigned int i=0; i<asteroids.size();i++)
    {   //garbage management for heap
        asteroid * _asteroid = asteroids[i];
        asteroids.erase(asteroids.begin()+i);
        delete _asteroid;
    }
    level = 1;
    startLevel = true;
}

void physics()
{
    ship[2] = ship[2] + ship[4]; //The angle from normal is equal to where it was plus its velocity times the frame rate
    if(ship[2] > 360) ship[2] -=360;
    else if(ship[2] < -360) ship[2] -=360;

    ship[1] = ship[1] + cos( (ship[2] + 90)*PI /180.0 ) * ship[3];
    ship[5] = ship[5] + sin( (ship[2] + 90)*PI /180.0 ) * ship[3];

    if(ship[1] > 2.2 || ship[1] < -2.2) ship[1] = -ship[1]/1.01; //wrap around the screen  
    if(ship[5] > 2.2 || ship[5] < -2.2) ship[5] = -ship[5]/1.01; //wrap around the screen  
    
    //retarding forces
    if (ship[4] > -.05 && ship[4] < .05) ship[4] -= ship[4]/40;
    else ship[4] -= ship[4]/30;

    if(ship[3] > -.002 && ship[3] < .002) ship[3] -= ship[3]/10;
    else ship[3] -= (ship[3]/60);
}


//note our ship implementation we are transforming it every fram, our bullet and asteroid implementation we are changing their
//member values and then redrawing it every frame. This is a mistake and in the future i should implement a ship the same way
//i implement the bullets and asteroids
void display(void)
{
        //put your OpenGL display commands here
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        
        // reset OpenGL transformation matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity(); // reset transformation matrix to identity
        
        // setup look at transformation so that 
        // eye is at : (0,0,3)
        // look at center is at : (0,0,0)
        // up direction is +y axis
        gluLookAt(0.f,0.f,3.f,0.f,0.f,0.f,0.f,1.f,0.f);
        //glRotatef(fRotateAngle,0.f,1.f,0.f);
        
        /*
        // Test drawing a solid teapot
        glColor3f(1.0,0.0,0.0); // set current color to Red
        glutSolidTeapot(1.f); // call glut utility to draw a solid teapot 
        */
        
        
        if(startLevel)
        {   makeAsteroids(level);
            startLevel = false;
        }

        physics();
     
        if(shipAsterColis())
        {   cout<<"collision! you die!!!" <<endl;
            level = 0;
            startLevel = false;
        }

        //draw the ship and transform it
        
        glPushMatrix();
        glTranslatef(ship[1], ship[5], 0.f);
        glRotatef(ship[2],0.f,0.f,1);
        drawship();
        glPopMatrix();  
        

        drawAsteroids();
        drawBullets();
        
        if( levelclear() )
        {   level ++;
            startLevel = true;
        }

        if(level==0 && startLevel == false)
        {   if(getStart())
                beginGame();
        }
        
        glFlush();
        glutSwapBuffers();  // swap front/back framebuffer to avoid flickering 
}


void reshape (int w, int h)
{
        // reset viewport ( drawing screen ) size
        glViewport(0, 0, w, h);
        float fAspect = ((float)w)/h; 
        // reset OpenGL projection matrix
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(70.f,fAspect,0.001f,30.f); 
}

void keyboard(unsigned char key, int x, int y)
{
    //pressed w move forwards
    if(key == 119)
    {
        if(ship[3] == 0) ship[3] +=.025;
        else ship[3] += .025 - (ship[3]/.1)*.025; //want it to stop accelerating at 2   
    }
    
    if(key == 97)
    {
        if(ship[4] == 0) ship[4] +=2;
        else ship[4] += 2 - (ship[4]/10)*2; //want it to stop accelerating at 2   
    }
    
    if(key == 100)
    {
        if(ship[4] == 0) ship[4] -=2;
        else ship[4] -= 2 + (ship[4]/10)*2; //want it to stop accelerating at 2   
    }

    if(key == 115)
    {
        if(ship[3] == 0) ship[3] -=.025;
        else ship[3] -= .025 + (ship[3]/.5)*.025; //want it to stop accelerating at 2   
    }

    if(key == 32)
    {   bullet*  newbullet = new bullet; //creates a new bullet on the heap so we can pass it out of this funct
        newbullet->xpos = ship[1] + ( cos( (ship[2] + 90)*PI/180.0) * .125 ); //move the bullet to the tip of the ship
        newbullet->ypos = ship[5] + ( sin( (ship[2] + 90)*PI/180.0) * .125);
        newbullet->ray = ship[2];   //using 2 d polar cordinates all we need is the angle to make the ray
        newbullet->time = 0;
        bullets.push_back(newbullet);
//        cout<<ship[2]<<"\t"<< (cos(ship[2] +90) * .25)<<endl;
    }

        if (key == 27) 
    {   printf("demonstration finished.\n");
        exit(0);
    }                                  
}

void timer(int v)
{

        glutPostRedisplay(); // trigger display function by sending redraw into message queue
        glutTimerFunc(1000/nFPS,timer,v); // restart timer again
}

int main(int argc, char* argv[])
{
        srand((unsigned int) time(0));  // seeding RNG

        glutInit(&argc, (char**)argv);
        //set up for double-buffering & RGB color buffer & depth test
        glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
        glutInitWindowSize (500, 500); 
        glutInitWindowPosition (100, 100);
        glutCreateWindow ((const char*)"2D Asteroid");
        
        init(); // setting up user data & OpenGL environment
                                
        // set up the call-back functions 
        glutDisplayFunc(display);  // called when drawing 
        glutReshapeFunc(reshape);  // called when change window size
        glutKeyboardFunc(keyboard); // called when received keyboard interaction
        glutTimerFunc(100,timer,nFPS); // a periodic timer. Usually used for updating animation
                                                              
        glutMainLoop(); // start the main message-callback loop
        
        return 0;
}
    

