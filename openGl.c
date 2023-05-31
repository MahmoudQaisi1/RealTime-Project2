#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <unistd.h>
#include <time.h>
#include "local.h"

int shmid2,sem_id2;
openGL *open_gl;
int shmid_size, sem_size,shmid_thresh,sem_thresh;
int *size,*thresh;
int prev=0,prev1=0,prev2=0,prevT=0,prevD=0;
//gcc -g openGL.c -o openGL -lglut -lGLU -lGL -lm



void clearDotsRight(int n) {
    int m=0;
    float x,i,j;
    for(i = 0.4 ; i < 0.9; i+=0.1){//x
        for(j = -0.8; j < 0.9; j+=0.1){//y
            glPointSize(10.0f);
            glBegin(GL_POINTS);
            glColor3f(0.0f, 0.0f, 0.0f); // Whited
            x=i*-1;
            glVertex2f(x, j); //x,y
            glEnd();
            m++;
            if(m>=n){
                return;
            }

        }
    }
    glFlush();

}
void clearDotsMiddle(int n) {
    int m = 0;
    float x,i,j;
    for (i = -0.2; i <= 0.2; i += 0.1) { // x
        for (j = -0.8; j <= 0.9; j += 0.1) { // y
            glPointSize(10.0f);
            glBegin(GL_POINTS);
            glColor3f(0.0f, 0.0f, 0.0f); // N color
            x = i;
            glVertex2f(x, j); // x, y
            glEnd();
            m++;
            if (m >= n) {
                return;
            }
        }
    }
    glFlush();
}

void clearDotsLeft(int n) {
    int m=0;
    float x,i,j;
    for(i = 0.7 ; i > 0.5; i-=0.1){//x
        for(j = -0.6; j < 0.7; j+=0.1){//y
            glPointSize(10.0f);
            glBegin(GL_POINTS);
            glColor3f(0.0f, 0.0f, 0.0f); // Whited
            //x=j*-1;
            glVertex2f(i, j); //x,y
            glEnd();
            m++;
            if(m>=n){
                return;
            }

        }
    }
    glFlush();

}
void clearDotsTopLeft(int n) {
    int m=0;
    float x,i,j;

    for(i = 0.8 ; i > 0.4; i-=0.1){//x
        for(j = 0.1; j < 0.9; j+=0.1){//y
            glPointSize(10.0f);
            glBegin(GL_POINTS);
            glColor3f(0.0f, 0.0f, 0.0f); // Black
            //x=j*-1;
            glVertex2f(i, j); //x,y
            glEnd();
            m++;
            if(m>=n){
                return;
            }

        }
    }
    
    glFlush();
}
void clearDotsBelowLeft(int n) {
    
    int m=0;
    float x,i,j;

    for(i = 0.8 ; i > 0.4; i-=0.1){//x
        for(j = -0.8; j < 0.0; j+=0.1){//y
            glPointSize(10.0f);
            glBegin(GL_POINTS);
            glColor3f(0.0f, 0.0f, 0.0f); // Black
            //x=j*-1;
            glVertex2f(i, j); //x,y
            glEnd();
            m++;
            if(m>=n){
                return;
            }

        }
    }
    glFlush();
}


void addDotsRight(int n) {
    clearDotsRight(prev);
    int m=0;
    float x,i,j;
    for(i = 0.4 ; i < 0.9; i+=0.1){//x
        for(j = -0.8; j < 0.9; j+=0.1){//y
            glPointSize(10.0f);
            glBegin(GL_POINTS);
            glColor3f(1.0f, 1.0f, 1.0f); // Whited
            x=i*-1;
            glVertex2f(x, j); //x,y
            glEnd();
            m++;
            if(m>=n){
                return;
            }

        }
    }
    prev=n;
 glFlush();
}

void addDotsLeft(int n) {
    clearDotsLeft(prev1);
    int m=0;
    float x,i,j;

    for(i = 0.7 ; i > 0.5; i-=0.1){//x
        for(j = -0.6; j < 0.7; j+=0.1){//y
            glPointSize(10.0f);
            glBegin(GL_POINTS);
            glColor3f(1.0f, 1.0f, 1.0f); // Whited
            //x=j*-1;
            glVertex2f(i, j); //x,y
            glEnd();
            m++;
            if(m>=n){
                return;
            }

        }
    }
    prev1=n;
    glFlush();
}
void addDotsTopLeft(int n) {
    clearDotsTopLeft(prevT);
    int m=0;
    float x,i,j;

    for(i = 0.8 ; i > 0.4; i-=0.1){//x
        for(j = 0.1; j < 0.9; j+=0.1){//y
            glPointSize(10.0f);
            glBegin(GL_POINTS);
            glColor3f(1.0f, 0.0f, 0.0f); // Whited
            //x=j*-1;
            glVertex2f(i, j); //x,y
            glEnd();
            m++;
            if(m>=n){
                return;
            }

        }
    }
    prevT=0;
    glFlush();
}
void addDotsBelowLeft(int n) {
    clearDotsBelowLeft(prevD);
    int m=0;
    float x,i,j;

    for(i = 0.8 ; i > 0.4; i-=0.1){//x
        for(j = -0.8; j < 0.0; j+=0.1){//y
            glPointSize(10.0f);
            glBegin(GL_POINTS);
            glColor3f(0.0f, 1.0f, 0.0f); // Green
            //x=j*-1;
            glVertex2f(i, j); //x,y
            glEnd();
            m++;
            if(m>=n){
                return;
            }

        }
    }
    prevD=n;
    glFlush();
}
void addDotsMiddle(int n) {
    clearDotsMiddle(prev2);
    int m = 0;
    float x,i,j;

    for (i = -0.2; i <= 0.2; i += 0.1) { // x
        for (j = -0.8; j <= 0.9; j += 0.1) { // y
            glPointSize(10.0f);
            glBegin(GL_POINTS);
            glColor3f(0.0f, 0.0f, 1.0f); // White color
            x = i;
            glVertex2f(x, j); // x, y
            glEnd();
            m++;
            if (m >= n) {
                return;
            }
        }
    }
    prev2 = n;
    glFlush();
}


void progress_bar1(float p){
    // Draw the first progress bar
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_QUADS);
        glVertex2f(-0.2, 0.425);
        glVertex2f(-0.2 + p, 0.425);
        glVertex2f(-0.2 + p, 0.325);
        glVertex2f(-0.2, 0.325);
    glEnd();
}


void progress_bar2(float p){
    // Set the color of the second progress bar to blue
    glColor3f(0.0, 0.0, 1.0);

    // Draw the second progress bar
    glBegin(GL_QUADS);
        glVertex2f(-0.2, -0.425);
        glVertex2f(-0.2 + p, -0.425);
        glVertex2f(-0.2 + p, -0.325);
        glVertex2f(-0.2, -0.325);
    glEnd();
}

void progress_bar3(float p){
        // Set the color of the second progress bar to black
    glColor3f(0.0, 0.0, 0.0);


            // Draw the third rectangle
    glBegin(GL_QUADS);
        glVertex2f(-0.8+0.05,0.975);
        glVertex2f(-0.8+0.05+p, 0.975);
        glVertex2f(-0.8+0.05+p, 0.925);
        glVertex2f(-0.8+0.05, 0.925);
    glEnd();
}

void progress_bar4(float p){
    // Set the color of the second progress bar to purple
    glColor3f(1.0, 0.0, 1.0);


    // Draw the second progress bar
    glBegin(GL_QUADS);
        glVertex2f(-0.2+0.05,0.975);
        glVertex2f(-0.2+0.05+p, 0.975);
        glVertex2f(-0.2+0.05+p, 0.925);
        glVertex2f(-0.2+0.05, 0.925);
    glEnd();
}

void progress_bar5(float p){
    // Set the color of the second progress bar to yellow
    glColor3f(1.0, 1.0, 0.0);


    // Draw the second progress bar
    glBegin(GL_QUADS);
        glVertex2f(0.4+0.05,0.975);
        glVertex2f(0.4+0.05+p, 0.975);
        glVertex2f(0.4+0.05+p, 0.925);
        glVertex2f(0.4+0.05, 0.925);
    glEnd();
}


void drawPageFrame() {  //here  put these inside the drawPageFrame(float progress1, float progress2){
	//and insted of 0.4 write the variables progress1 and the second 0.4 replace it with progress2 
    // Set the color of the frame to blue
    glColor3f(0.0, 0.0, 1.0);

    // Draw the top and bottom sides of the frame
    glBegin(GL_QUADS);
        glVertex2f(-1.0, 1.0);
        glVertex2f(1.0, 1.0);
        glVertex2f(1.0, 0.9);
        glVertex2f(-1.0, 0.9);

        glVertex2f(-1.0, -1.0);
        glVertex2f(1.0, -1.0);
        glVertex2f(1.0, -0.9);
        glVertex2f(-1.0, -0.9);
    glEnd();

    // Draw the left and right sides of the frame
    glBegin(GL_QUADS);
        glVertex2f(-1.0, 1.0);
        glVertex2f(-0.9, 1.0);
        glVertex2f(-0.9, -1.0);
       glVertex2f(-1.0, -1.0);

        glVertex2f(1.0, 1.0);
        glVertex2f(0.9, 1.0);
        glVertex2f(0.9, -1.0);
        glVertex2f(1.0, -1.0);
    glEnd();
    glColor3f(0.0, 1.0, 0.0);

    glColor3f(1.0, 0.0, 0.0);

    // Draw the first line
   // glBegin(GL_LINES);
        //glVertex2f(-0.8, 0.8);
      //  glVertex2f(0.5, 0.8);
    //glEnd();

    // Draw the second line
   // glBegin(GL_LINES);
        //glVertex2f(-0.8, 0.7);
      //  glVertex2f(0.5, 0.7);
    //glEnd();
    //
    //Draw the horizntal line between the progress bars
    glColor3f(1.0, 0.0, 0.0);

    // Draw the line
    glBegin(GL_LINES);
        glVertex2f(0.9, 0.0);
        glVertex2f(0.3, 0.0);
    glEnd();
//draw the vertical lines 
    glColor3f(1.0, 0.0, 0.0);

    // Draw the first line
    glBegin(GL_LINES);
        glVertex2f(-0.3, 0.8);
        glVertex2f(-0.3, -0.8);
    glEnd();

    // Draw the second line
  glBegin(GL_LINES);
        glVertex2f(0.3, 0.8);
        glVertex2f(0.3, -0.8);
    glEnd();

    //*******************Draw two rectangels**********************
    // Set the color of the lines to red
    glColor3f(1.0, 0.0, 0.0);


    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.8+0.05,0.975);
        glVertex2f(-0.5+0.05, 0.975);
        glVertex2f(-0.5+0.05, 0.925);
        glVertex2f(-0.8+0.05, 0.925);
    glEnd();

            // Draw the fifth rectangle
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.4+0.05,0.975);
        glVertex2f(0.7+0.05, 0.975);
        glVertex2f(0.7+0.05, 0.925);
        glVertex2f(0.4+0.05, 0.925);
    glEnd();

    //*******************draw table***********************
       glColor3f(0.6, 0.4, 0.2);

   
    //draw the two horizantal lines at the most right
     // Set the color of the lines to red
    glColor3f(1.0, 0.0, 0.0);

    srand(time(NULL));
int k = (int) (rand()%86);
    addDotsRight((open_gl->messages)%86);

int z = (int) (rand()%26);
    //addDotsLeft(z);
    addDotsMiddle(open_gl->encoded%86);
    addDotsTopLeft(open_gl->spy%26);
    addDotsBelowLeft(open_gl->receiver%26);

 int p3,p5 = (int) (rand()%10);

    progress_bar3((open_gl->thresh1/open_gl->th1)*0.3);
    progress_bar5((open_gl->thresh2/open_gl->th2)*0.3);
    glFlush();


}



//**********************add dots to the most right*********************




//************************* remove dots********************************

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
   // Compute aspect ratio of the new window
   if (height == 0) height = 1;                // To prevent divide by 0
   GLfloat aspect = (GLfloat)width / (GLfloat)height;
 
   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);
 
   // Set the aspect ratio of the clipping area to match the viewport
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();
   if (width >= height) {
     // aspect >= 1, set the height from -1 to 1, with larger width
      gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
   } else {
      // aspect < 1, set the width to -1 to 1, with larger height
     gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
   }
}

//end the remove dots **************************************************
void display() {

    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the page frame
    drawPageFrame();

 

    // Swap the buffers to update the display
    glutSwapBuffers();
    sleep(1);
    glutPostRedisplay();
    // Clear the window

}
//*************************************remove function********************



//***********************end of remove function*********************
int main(int argc, char** argv) {

    //read_param();
    //Accessing the shared memory and semaphores for openGL//////////////////////////////////////////////////////////
    shmid2 = shmget(1, sizeof(openGL), 0);
    if (shmid2 < 0)
    {
        perror("shmget receiver... openGL");
        exit(1);
    }

    open_gl = (openGL *)shmat(shmid2, NULL, 0);
    if (open_gl == (openGL *)-1)
    {
        perror("shmat receiver...");
        exit(1);
    }

    sem_id2 = semget(1, 2, 0);
    if (sem_id2 < 0)
    {
        perror("semget receiver");
        exit(1);
    }

    //printf("officer semid_size=%d\n",semid_size);
    // Initialize GLUT
    glutInit(&argc, argv);

    // Set the window size and position
   
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
  

    // Set the display mode to double buffering
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Create the window
    glutCreateWindow("OIM");
    // Set the display callback function
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    // Enter the GLUT event loop
    glutMainLoop();
    sleep(2);
    addDotsRight(40);
    sleep(2);
    addDotsRight(10);
    sleep(2);


    return 0;
}