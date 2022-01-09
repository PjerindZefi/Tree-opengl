/*
Dr. Gueorguieva
Student Pjerind Zefi
*/

#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <time.h>
#include <iostream>

using namespace std;
class Point2   //single point w/ floating point coordinates
{
public:
    float x, y;
    Point2() { x = y = 0.0f; } //constructor 1
    Point2(float xx, float yy) { x = xx; y = yy; } //constructor 2
    void set(float xx, float yy) { x = xx; y = yy; }
    float getX() { return x; }
    float getY() { return y; }
    void draw(void)
    {
        glBegin(GL_POINTS); //draw this point
        glVertex2f((GLfloat)x, (GLfloat)y);
        glEnd();
    }

};

int getrandom(int min, int max) {
    srand(time(0));
    return (rand() % (int)(((max)+1) - (min))) + (min);
}

Point2 CP;      //current turtle point
float CD = 90;  //current turtle direction
int Thick;
float trunk[] = { 1.16, 1.13, 1.07 };       //Trunk color
float leafBase[] = { 1.82, 1.54, 1.65 };   //The overall color of leaf
float leafTop[] = { 1.91, 1.8, 1.82 };      //The highlight of the leaf

char Xstr[150] = "XX-[FL+FL+FL]+[+FL-FL-FL]";    //X production rule
char XstrA[150] = "XX-[FN+FNY+FN]+[+FN-FN-FN]";    //X production rule
char YstrA[150] = "YY-[FNW+FN]+[+FNA]"; //Y production of rule
char FstrNL[150] = "FF-[-FX+FX+FX]+[+FX-FX-FX]"; //No leaves F 
char Fstr[150] = "FF-[-FX+FX+FX]+[+FX-FX-FX]";   //F production rule

char atomNL[150] = "F";                      //No leaves starting string
char atom[150] = "F";                            //starting string

char Ystr[150] = "";                             //Y production rule
char XstrNL[150] = "";                       //No leaves X

float angle = getrandom(22, 22);                  //turn angle
int length = 20;                                 //forward length

//array of current turtle locations
//better to use a linked list for these :)
//These arrays will allow a maximum of 100 nested [] pairs.   Should be heaps!!
Point2 saveTut[150];
float saveAng[150];
int curr = 0;

//min and max extremes for the window size
float xmin = 0.0, xmax = 0.0, ymin = 0.0, ymax = 0.0;



void moveTo(float x, float y)
{
    CP.set(x, y);
}

void lineTo(float x, float y)
{
    glBegin(GL_LINES);
    glVertex2f((GLfloat)CP.getX(), (GLfloat)CP.getY());
    glVertex2f((GLfloat)x, (GLfloat)y); //draw the line
    glEnd();
    CP.set(x, y); //update current point to new point
    glFlush();
}

void ngon(int n, float cx, float cy, float radius, float rotAngle) {
    if (n < 3) return;
    double angle = rotAngle * 3.14159265 / 180;
    double angleInc = 2 * 3.14159265 / n;
    moveTo(radius + cx, cy);

    for (int k = 0; k < n; k++)
    {
        angle += angleInc;
        lineTo(radius * cos(angle) + cx, radius * sin(angle) + cy);
    }
}

void turnTo(float angle) { CD = angle; }
void turn(float angle) { CD += angle; }

void forward(float dist, int isVisible)
{
    const float rpd = 0.017453393;
    float x1 = CP.x + dist * cos(rpd * CD);
    float y1 = CP.y + dist * sin(rpd * CD);
    if (isVisible)
        lineTo(x1, y1);
    else
        moveTo(x1, y1);
}

void saveTurtle()
{
    saveTut[curr].set(CP.x, CP.y);
    saveAng[curr] = CD;
    curr++;
}

void restoreTurtle()
{
    curr--;
    moveTo(saveTut[curr].getX(), saveTut[curr].getY());
    turnTo(saveAng[curr]);
}

void drawLeaf() {

    glPointSize(6);
    glBegin(GL_POINTS);
    glColor3fv(leafBase);
    glVertex2f(CP.x, CP.y);
    glEnd();
    glPointSize(5);
    glBegin(GL_POINTS);
    glColor3fv(leafTop);
    glVertex2f(CP.x, CP.y + 1);
    glEnd();
    glColor3fv(trunk);
}

void newLeaf(int t) {
    glBegin(GL_POLYGON);

    glColor3fv(leafBase);
    float theta;
    int r = 10;
    for (int i = 0; i < 360; i++)
    {
        theta = i * 3.14159265 / 180;
        glVertex2f(r * cos(theta) + CP.getX(), r * sin(theta) + CP.getY());
    }
    glEnd();
    glLineWidth(0.5);;

    glBegin(GL_LINE_STRIP);
    glColor3fv(leafTop);
    glVertex2i(cos(3.14159265 / 180) + CP.getX() + 10, sin(3.14159265 / 180) + CP.getY());
    glVertex2i(cos(3.14159265 / 180) + CP.getX() - 11, sin(3.14159265 / 180) + CP.getY());
    glEnd();

    glLineWidth(t);
    glColor3fv(trunk);
}
void produceString(char* st, int order, int draw, float thick)
{
    float sx, sy, sa;
    glLineWidth(thick);

    while (*st != '\0')
    {
        switch (*st)
        {
        case '[': if (thick >= 1) thick /= 3; saveTurtle();
            break;
        case ']': if (thick <= Thick) thick *= 3; restoreTurtle();
            break;
        case '+': turn(-angle);
            break;
        case '-': turn(angle);
            break;
        case 'L': if (order <= 0) {
            drawLeaf();
        }
                break;
        case 'F': if (order > 0)
        {
            produceString(FstrNL, order - 1, draw, thick);
        }
                else
        {
            forward(length, draw);
            if (!draw)
            {
                if (CP.x < xmin)
                    xmin = CP.x;
                if (CP.x > xmax)
                    xmax = CP.x;
                if (CP.y < ymin)
                    ymin = CP.y;
                if (CP.y > ymax)
                    ymax = CP.y;
            }
        }
                break;
        case 'X': if (order > 0)
        {
            produceString(XstrNL, order - 1, draw, thick);
            produceString(XstrA, order - 1, draw, thick);
            produceString(Xstr, order - 1, draw, thick);
        }

        }
        st++;
    }

}

void MyDisplay(void)
{
    CP.set(0, 0);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT);

    //Change colors to basic colors
    trunk[0] = 1;       trunk[1] = 0;       trunk[2] = 0;
    leafBase[0] = 0;    leafBase[1] = 1;    leafBase[2] = 0;
    leafTop[0] = 0.0;  leafTop[1] = 1;  leafTop[2] = 0.0;


    glColor3fv(trunk);
    int thick = 10;
    Thick = thick;

    //setup initial turtle position
    //run through once to determine window coordinates
    moveTo(0.0, 0.0);
    turnTo(90);
    produceString(atom, 4, 0, thick);
    gluOrtho2D(xmin + 10, xmax - 10, ymin + 10, ymax - 10);

    moveTo(0.0, 0.0);
    turnTo(90);
    produceString(atom, 4, 1, thick);
    glFlush();
}

void main(int argc, char** argv)
{
    glutInit(&argc, argv);          // initialize the toolkit
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); // set display mode
    glutInitWindowSize(600, 600);     // set window size
    glutInitWindowPosition(100, 150); // set window position on screen
    glutCreateWindow("L-System Tree"); // open the screen window and set the name
    glutDisplayFunc(MyDisplay);
    glutMainLoop();
}
