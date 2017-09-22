#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <ostream>
#include <vector>
#include <cstring>
//*************Debugging stage, delete when finished****************//

//*****************************************************************//
#define PI 3.14159265
#define max_bullets 32
#define max_enemies 32
#define max_parts 8
float cols[2][3] = {{0.2,0.2,0.2}, {0.9,0.9,0.9} };
using namespace std;
//***********Bullet Info************//
typedef struct {
	int active;
	int x,y,z,dz;
	int dir;
} Bullet;
static int shoot = 0;
Bullet bullets[max_bullets];//max bullets on screen
Bullet enemyBullets[max_bullets];// max enemy bullets
//***********Enemy Condition*************//
typedef struct {
	int active;
	int x,y,z;
	int hitx,hity;
	int health;

}Enemy;
static int newLevel = 1;
Enemy enemies[max_enemies];//max enemies on screen
//*********Player Condition*************//
typedef struct{
	int active;
	int x,y,z;
	int hitx,hity;
	int health;
}Player;
Player player;
/*********object info*********/
typedef struct
{
    int vertices;
    int positions;
    int texels;
    int normals;
    int faces;
} Model;
/*********particle***********/
typedef struct
{
	int active;
	int n;
	int x,y,z;
	int dx,dy,dz;
} Particle;
Particle particles[max_parts];
static int boom = 0;
/***
 INITIAL CONDITION
***/
int SceneX = 100;
int SceneY = 100;
int SceneZ = 100;
int midX = SceneX/2;
int midY = SceneY/2;
int camOffset = 20;// z axis
int shipOffset = 3;// y axis
float rotx;
float rotz;
bool lightOn = true;
bool pause = false;
bool easyMode = false;
/***
 CAMERA INFORMATION
***/
float eye[] = {midX ,midY+ shipOffset,SceneZ + camOffset};
float angle = 0.0f;
//dot is what the camera looks at
float dot[] = {midX,midY,0};
float rot[] = {0, 0, 0};
/***

LIGHTING LOCATIONS
***/
float lightpos1[] = {0,-0.35,1,0};// normally 0,-3,1,0
float lightpos2[] = {0,20,0,1};
/***
/*** 
TIMER INFORMATION
***/
int c = 0;
int w = 0;
/* Timer - this is main timer handler in the project, it is called in the display function
*/
void Timer(){
	c++; 
	if(c == 2){
	c = 0;
	w ++;
	
	}
}
bool time(int range){
	int i = w;
	while((w-i)%range != 0){
	return true;
	}
	return false;

}
/* r - short for range, sets the repeat range of an object
*/
int r(int range){
	return w%range;	
}
void CreateParticle(int x, int y,int z){
	int i = 0;
	
	for(i =0;i <max_parts; i++){
	if(particles[i].active == 0){
		particles[i].active =1;
		particles[i].x = x;
		particles[i].y = y;
		particles[i].z = z;
		particles[i].dx = 0;
		break;
	
	}
	}

}
void moveParticle(){
	int i = 0;
	for(i = 0; i <max_parts;i++){
		//edge case
		if(particles[i].active ==1 && particles[i].dx>=20){
		particles[i].active = 0;
		}else{
		particles[i].dx++;
		}
	}
}
void DrawParticle(){
	moveParticle();
	int x;
	int y;
	int z;
	for(int i = 0;i<max_parts;i++){
		if(particles[i].active == 1){
		x = particles[i].x;
		y = particles[i].y;
		z = particles[i].z;
		glBegin(GL_POINTS);
		glColor3f(1.0,1.0,0);
		glVertex3f(1*particles[i].dx + x,y,z);
		glVertex3f(1*particles[i].dx + x,1*particles[i].dx+y,z);
		glVertex3f(1*particles[i].dx + x,1*particles[i].dx+y,1*particles[i].dx+z);
		glVertex3f(1*particles[i].dx + x,y,1*particles[i].dx+z);
		glVertex3f(-1*particles[i].dx + x,1*particles[i].dx+y,-1*particles[i].dx+z);
		glVertex3f(1*particles[i].dx + x,y,-1*particles[i].dx+z);
		glVertex3f(-1*particles[i].dx + x,-1*particles[i].dx+y,z);
		glVertex3f(-1*particles[i].dx + x,-1*particles[i].dx+y,-1*particles[i].dx+z);
		glVertex3f(x,1*particles[i].dx+y,1*particles[i].dx+z);
		glVertex3f(x,-1*particles[i].dx+y,1*particles[i].dx+z);

		glEnd();
		}
	}
}
/***TEXTURE INFORMATION
***/
GLubyte* tex;
int width, height, max;
/* LoadPPM -- loads the specified ppm file, and returns the image data as a GLubyte 
 *  (unsigned byte) array. Also returns the width and height of the image, and the
 *  maximum colour value by way of arguments
 *  usage: GLubyte myImg = LoadPPM("myImg.ppm", &width, &height, &max);
 */
GLubyte* LoadPPM(char* file, int* width, int* height, int* max)
{
	GLubyte* img;
	FILE *fd;
	int n, m;
	int  k, nm;
	char c;
	int i;
	char b[100];
	float s;
	int red, green, blue;
	
	fd = fopen(file, "r");
	fscanf(fd,"%[^\n] ",b);
	if(b[0]!='P'|| b[1] != '3')
	{
		printf("%s is not a PPM file!\n",file); 
		exit(0);
	}
	printf("%s is a PPM file\n", file);
	fscanf(fd, "%c",&c);
	while(c == '#') 
	{
		fscanf(fd, "%[^\n] ", b);
		printf("%s\n",b);
		fscanf(fd, "%c",&c);
	}
	ungetc(c,fd); 
	fscanf(fd, "%d %d %d", &n, &m, &k);

	printf("%d rows  %d columns  max value= %d\n",n,m,k);

	nm = n*m;

	img = (GLubyte*)malloc(3*sizeof(GLuint)*nm);


	s=255.0/k;


	for(i=0;i<nm;i++) 
	{
		fscanf(fd,"%d %d %d",&red, &green, &blue );
		img[3*nm-3*i-3]=red*s;
		img[3*nm-3*i-2]=green*s;
		img[3*nm-3*i-1]=blue*s;
	}

	*width = n;
	*height = m;
	*max = k;

	return img;
}
void displayLife(){
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int width,height;
	unsigned int imageSize;
	unsigned char * data;
	FILE * file = fopen("shitty_heart.bmp","rb");
	if(!file){printf("ERROR NO IMAGE FOUND");}
	fclose(file);
	GLuint image;// ("shitty_heart.bmp");
	glGenTextures(1,&image);
	glBindTexture(GL_TEXTURE_2D,image);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_BGR,GL_UNSIGNED_BYTE,data);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
}
/*** Model Info
***/
Model getOBJinfo(char* fp){
	Model model = {0};
	ifstream inOBJ (fp);
	if(!inOBJ.good()){
	printf("ERROR OPENING FILE");
	}
	while(!inOBJ.eof()){	
		string line;
		getline(inOBJ,line);
		string type = line.substr(0,2);
		if(type.compare("v ") == 0)
			model.positions++;
		else if(type.compare("vt") == 0)
          		model.texels++;
       		else if(type.compare("vn") == 0)
           		model.normals++;
       		else if(type.compare("f ") == 0)
           		model.faces++;
    	}
	model.vertices = model.faces*3;
	// Close OBJ file
	inOBJ.close();
 	return model;
}
void extractOBJdata(char* fp, float positions[][3], float texels[][2],
			 float normals[][3], int faces[][6])
{
    // Counters
    int p = 0;
    int t = 0;
    int n = 0;
    int f = 0;
 
    // Open OBJ file
    ifstream inOBJ(fp);
    if(!inOBJ.good())
    {
       printf("ERROR READING FILE");
    }
 
    // Read OBJ file
    while(!inOBJ.eof())
    {
        string line;
        getline(inOBJ, line);
        string type = line.substr(0,2);
	
        if(type.compare("v ") == 0)
	{
	char* l = new char[line.size()+1];
	memcpy (l, line.c_str(), line.size()+1);
	// Extract tokens
	strtok(l, " ");
	for(int i=0; i<3; i++)
        positions[p][i] = atof(strtok(NULL, " "));
	//dealocate
	delete[] l;
	p++;
	}// Texels
else if(type.compare("vt") == 0)
{
    char* l = new char[line.size()+1];
    memcpy(l, line.c_str(), line.size()+1);
 
    strtok(l, " ");
    for(int i=0; i<2; i++)
        texels[t][i] = atof(strtok(NULL, " "));
 
    delete[] l;
    t++;
}
 
// Normals
else if(type.compare("vn") == 0)
{
    char* l = new char[line.size()+1];
    memcpy(l, line.c_str(), line.size()+1);
 
    strtok(l, " ");
    for(int i=0; i<3; i++)
        normals[n][i] = atof(strtok(NULL, " "));
 
    delete[] l;
    n++;
}
 
// Faces
else if(type.compare("f ") == 0)
{
    char* l = new char[line.size()+1];
    memcpy(l, line.c_str(), line.size()+1);
 
    strtok(l, " ");
    for(int i=0; i<6; i++)
        faces[f][i] = atof(strtok(NULL, " /"));
 
    delete[] l;
    f++;
}
 	}
    // Close OBJ file
    inOBJ.close();
}

/*drawPanels() - this draws the left and right plans for the scene, USED FOR TESTING
*/
void drawPanels(float* c, float w,float l){
	glPushMatrix();	
	glBegin(GL_QUADS);
	//right panel along x
	float v2[4][3] = { {c[0],c[1], c[2]-1},
			{c[0]+l,c[1], c[2]-1},
			{c[0]+l,c[1]+w, c[2-1]},
			{c[0],c[1]+w, c[2]-1}};
	//offset by one to match up with grid
	glColor3fv(cols[0]);
	glVertex3fv(v2[0]);
	glVertex3fv(v2[1]);
	glVertex3fv(v2[2]);
	glVertex3fv(v2[3]);
	glEnd();
	glPopMatrix();//grid
	
}
/* DrawReticule - draws two small boxes as reticule
*/
void DrawReticule(){
	//z axis
	//*********straightLine, remove later**************//
	glColor3f(0.0,0.0,1.0);
	if(easyMode){
	glBegin(GL_LINES);
	glVertex3f(dot[0],dot[1],dot[2]);
	glVertex3f(dot[0],dot[1]+1,dot[2]+SceneZ);
	glEnd();
	}
	//*********first square**********************//
	glBegin(GL_QUADS);
	int off1 = 3; //x and y axis
	int square1 = 50; //z axis and is 50
	//right panel along x
	float v[4][3] = { {dot[0]-off1,dot[1]-off1,dot[2]+square1},
			{dot[0]+off1,dot[1]-off1, dot[2]+square1},
			{dot[0]+off1,dot[1]+off1, dot[2]+square1},
			{dot[0]-off1,dot[1]+off1, dot[2]+square1}};
	//offset by one to match up with grid
	glColor3f(0.0,0.0,1.0);
	glVertex3fv(v[0]);
	glVertex3fv(v[1]);
	glVertex3fv(v[2]);
	glVertex3fv(v[3]);
	glEnd();
	//*********second square********************//
	glBegin(GL_QUADS);
	int off2 = 2; //x and y axis
	int square2 = 25; //z axis and is 50
	//right panel along x
	float v2[4][3] = { {dot[0]-off2,dot[1]-off2,dot[2]+square2},
			{dot[0]+off2,dot[1]-off2, dot[2]+square2},
			{dot[0]+off2,dot[1]+off2, dot[2]+square2},
			{dot[0]-off2,dot[1]+off2, dot[2]+square2}};
	//offset by one to match up with grid
	glColor3f(0.0,0.0,1.0);
	glVertex3fv(v2[0]);
	glVertex3fv(v2[1]);
	glVertex3fv(v2[2]);
	glVertex3fv(v2[3]);
	glEnd();
	glutPostRedisplay();

}

void DrawStar(int x, int y,int z){
	
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glColor4f(0.8,0.8,0.2,1.0);
	glPointSize(2.0f);
	glBegin(GL_POINTS);
	glVertex3f(x,y,z);
	glEnd();
	glPointSize(5.0f);
	glColor4f(0.8,0.8,0.2,0.2);
	glBegin(GL_POINTS);
	glVertex3f(x,y,z);
	glEnd();
	glutPostRedisplay();
	
}
void DrawStars(){
	//draw 16 stars all along the edges of the map
	//top 4
	DrawStar(14,92,r(243)-143);
	DrawStar(35,97,r(212)-112);
	DrawStar(67,94,r(283)-183);
	DrawStar(87,98,r(224)-124);
	//right 4
	DrawStar(91,84,r(226)-126);
	DrawStar(93,59,r(274)-174);
	DrawStar(97,41,r(225)-125);
	DrawStar(94,23,r(283)-183);
	//left 4
	DrawStar(7,83,r(252)-152);
	DrawStar(3,64,r(262)-162);
	DrawStar(2,49,r(283)-183);
	DrawStar(6,17,r(225)-125);
	//bottom 4
	DrawStar(14,8,r(214)-114);
	DrawStar(31,7,r(264)-164);
	DrawStar(51,2,r(272)-172);
	DrawStar(73,6,r(225)-125);
	// top 4
	DrawStar(7,101,r(243)-143);
	DrawStar(46,90,r(212)-112);
	DrawStar(81,107,r(283)-183);
	DrawStar(97,92,r(224)-124);
	//right 4
	DrawStar(102,91,r(226)-126);
	DrawStar(94,52,r(274)-174);
	DrawStar(97,29,r(225)-125);
	DrawStar(102,03,r(283)-183);
	//left 4
	DrawStar(-1,92,r(252)-152);
	DrawStar(-5,76,r(262)-162);
	DrawStar(2,59,r(283)-183);
	DrawStar(6,27,r(225)-125);
	//bottom 4
	DrawStar(12,-1,r(214)-114);
	DrawStar(38,9,r(264)-164);
	DrawStar(61,0,r(272)-172);
	DrawStar(98,6,r(225)-125);

}
void DrawPlanet(int x, int y, int z){
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glColor4f(0,0.9,0.4,1.0);
	glPushMatrix();
	glTranslatef(x,y,z);
	glRotatef(90,1,0,0.5);
	glutSolidSphere(15,16,16);
	glPopMatrix();
	glutPostRedisplay();

}
void DrawSaturn(int x, int y, int z){
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glColor4f(1.0,0,1.0,1.0);
	glPushMatrix();
	glTranslatef(x,y,z);
	glRotatef(90,1,0,0.5);
	glutSolidSphere(8,16,16);
	glColor4f(1.0,0,1.0,0.7);
	glutSolidTorus(5,18,2,16);
	glPopMatrix();
	glutPostRedisplay();

}
void DrawSun(int x,int y,int z){
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glColor4f(1.0,0.8,0,1.0);
	glPushMatrix();
	glTranslatef(x,y,z);
	glRotatef(90,1,0,0);
	glScalef(1,1,1);
	glutSolidSphere(150,16,16);
	glPopMatrix();
	glutPostRedisplay();
}
void DrawPlanets(){
	//purple star
	glPolygonMode(GL_FRONT, GL_FILL);
	DrawPlanet(-3,46,r(3000)-213);
	DrawSaturn(115,32,r(2690)-239);
	DrawSun(50,-150,r(3200)-700);
	//green star

}
/*DrawScene() - based on the current state of the viewModel,
this function desides which mesh type to draw
*/
void DrawScene(){
	float origin[3] = {0,0,0};
	{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//drawPanels(origin,SceneY,SceneX);
	DrawStars();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	DrawReticule();
	}
	glutPostRedisplay();

}
void moveBullet(){
	int i = 0;
	if(shoot ==1){		//if space bar is pressed, draw new bullet
	for(i =0;i <max_bullets; i++){
	if(bullets[i].active == 0){
		bullets[i].active =1;
		bullets[i].x = eye[0];
		bullets[i].y = eye[1]-shipOffset;
		bullets[i].z = SceneZ;
		bullets[i].dz = 1;
		bullets[i].dir = 0;
		break;
	}
	}
	shoot = 0; //prevents rapid firing
	}
	for(i = 0; i <max_bullets;i++){
		if(bullets[i].active == 1){
		bullets[i].z = bullets[i].z - bullets[i].dz;
		}
		//edge case
		if(bullets[i].active ==1 && bullets[i].z <= -20){
		bullets[i].active = 0;
		}
	}
	//end of player shooting
}
void moveEnemyBullet(){
	//***************Enemies Shooting
	int i = 0;
	int r = rand()% 10; // one in 5 chances
	for(i =0;i <max_enemies; i++){
	if(enemies[i].active == 1){
		if(enemyBullets[i].active == 0 && r ==0){
			enemyBullets[i].active =1;
			enemyBullets[i].x = enemies[i].x;
			enemyBullets[i].y = enemies[i].y;
			enemyBullets[i].z = enemies[i].z+2;;
			enemyBullets[i].dz = 1;
			enemyBullets[i].dir = 1;
			break;
		}
		}
	}
	for(i = 0; i <max_bullets;i++){
		if(enemyBullets[i].active == 1){
		enemyBullets[i].z = enemyBullets[i].z + enemyBullets[i].dz;
		}
		//edge case
		if(enemyBullets[i].active ==1 && enemyBullets[i].z >= 140){
		enemyBullets[i].active = 0;
		}

	}

}
void DrawBullet(Bullet *b){
	glPushMatrix();
	
	//player bullets
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPointSize(10.0f);
	if(b->dir == 0){
	glTranslatef(b->x,b->y,b->z);
	glColor4f(0,1.0,0,1.0);
	}else if(b->dir == 1){
	//enemy bullet
	glTranslatef(b->x,b->y,b->z+5);
	glColor4f(1.0,0,0,1.0);
	}
	glRotatef(r(360)*8,0,0,1);
	glutSolidCone(1,2,5,3);
	glPopMatrix();
}
void DrawBullets(){
	moveBullet();
	moveEnemyBullet();
	for(int i = 0;i< max_bullets;i++){
		if(bullets[i].active){
		DrawBullet(&bullets[i]);
		}
		if(enemyBullets[i].active){
		DrawBullet(&enemyBullets[i]);
		}
	}

}
bool enemyHit(Enemy *e, Bullet *b){
	for(int i= -4;i<4;i++){
		for(int j = -4;j<7;j++){
		
	if(e->x+i == b->x && e->y+j == b->y && e->z == b->z){
	//e->active = 0;
	return true;
	}
	}
	}
	return false;
}
void DrawEnemy(Enemy *e){
	glPushMatrix();
	//player bullets
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glTranslatef(e->x,e->y,e->z);
	glColor4f(0.7,0,0.3,1.0);
	//draw the enemy
	glRotatef(25*sin(r(100)/1.75),-1,0,0);
	Model enemy = getOBJinfo("enemy3.obj");
	float positions[enemy.positions][3];
	float texels[enemy.texels][2];
	float normals[enemy.normals][3];
	int faces[enemy.faces][6];
	extractOBJdata("enemy3.obj",positions, texels,
			 normals, faces);
	for(int i = 0;i<enemy.faces;i++){
		glBegin(GL_TRIANGLES);
		int vA = faces[i][0] - 1;
		int vB = faces[i][2] - 1;
		int vC = faces[i][4] - 1;
		int f = 4;
		int vnA = faces[i][1] - 1;
		int vnB = faces[i][3] - 1;
		int vnC = faces[i][5] - 1;
		glNormal3f(normals[vnA][0],normals[vnA][1] ,normals[vnA][2]);
		glVertex3f(positions[vA][0]*f,positions[vA][1]*f ,positions[vA][2]*f );
		glNormal3f(normals[vnB][0],normals[vnB][1] ,normals[vnB][2]);
		glVertex3f(positions[vB][0]*f,positions[vB][1] *f,positions[vB][2]*f );
		glNormal3f(normals[vnB][0],normals[vnB][1] ,normals[vnB][2]);
		glVertex3f(positions[vC][0]*f,positions[vC][1]*f ,positions[vC][2]*f );
		glEnd();
	}
	glPopMatrix();
}
void createEnemy(){
int i = 0;
	int j = 0;
	int fx = SceneX/5;
	int fy = SceneY/5;
	for(i =0;i <max_enemies; i++){
	if(i%4 +1 == 4)
		j++;
	
	if(enemies[i].active == 0 && newLevel == 1){
		enemies[i].active =1;
		enemies[i].x = fx* (i%4+1)+(j%2 * -1)*5;
		enemies[i].y = fx* (j%4+1);
		enemies[i].z = 0;		//will randomize z
		enemies[i].hitx = enemies[i].x;
		enemies[i].hity = enemies[i].y;
		enemies[i].health = 10;
	}
	}

}
void moveEnemy(){
	int i = 0;
	int j = 0;
	int fx = SceneX/5;
	int fy = SceneY/5;
	newLevel =0; // stops creation of enemies
	for(i = 0; i <max_enemies;i++){
		//damage
		if(enemies[i].active ==1){
		for(int k =0;k<max_bullets;k++){
		if(enemyHit(&enemies[i],&bullets[k])){
		enemies[i].health = enemies[i].health -5;
		CreateParticle(enemies[i].x,enemies[i].y,enemies[i].z);
		//hit animation
			if(enemies[i].health == 0){
				//particle effect
				enemies[i].active = 0;
			}
		}
		}
		}

	}
}
void DrawEnemies(){
	createEnemy();
	moveEnemy();
	for(int i = 0; i<max_enemies;i++){
	if(enemies[i].active){
	DrawEnemy(&enemies[i]);
	}
	}
}
void createPlayer(){

	if(player.active == 0 && newLevel == 1){
		player.active =1;
		player.x = eye[0];
		player.y = eye[1]-5;
		player.z = eye[2]-20;		
		player.hitx = player.x;
		player.hity = player.y;
		rotx = 0;
		rotz = 0;
		player.health = 3;
	}
	

}
bool Hit(Bullet *b){
	
	for(int i = 0;i<6;i++){
		for(int j= 0; j<6;j++){

		if(player.x-3 +i == b->x && player.y-3 +j == b->y && player.z == b->z){
		printf("You've been hit\n");
		
		return true;
		}
		}
	}
	return false;
}
void movePlayer(){	
	//newLevel =0; // stops creation of enemies
	if(player.active == 0){
		printf("you are dead, press n to continue");
		shoot = 0;
	}
	if(player.active == 1){
		//update hitboxes
		player.x = eye[0];
		player.y = eye[1]-5;
	}
	//damage
	if(player.active ==1){
	for(int k =0;k<max_bullets;k++){
	if(Hit(&enemyBullets[k])){
	player.health = player.health -1;
	CreateParticle(eye[0],eye[1]-5,player.z);
	//hit animation
		if(player.health== 0){
			//particle effect
			player.active = 0;
		}
	}
	}
	}
}
void DrawPlayer(){
	createPlayer();
	movePlayer();
	if(player.active==1){
	glPushMatrix();
	//player bullets
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glTranslatef(eye[0],eye[1]-5,player.z);
	glColor4f(0.3,0.3,0.3,1.0);
	glScalef(1,1,1);
	if(rotx != 0 ||rotz !=0){
	glRotatef(rotz*1,1,0,0);
	glRotatef(rotx*1,0,0,1);
	}
	Model ship = getOBJinfo("player_ship.obj");
	float positions[ship.positions][3];
	float texels[ship.texels][2];
	float normals[ship.normals][3];
	int faces[ship.faces][6];
	extractOBJdata("player_ship.obj",positions, texels,
			 normals, faces);
	for(int i = 0;i<ship.faces;i++){
		glBegin(GL_TRIANGLES);
		int vA = faces[i][0] - 1;
		int vB = faces[i][2] - 1;
		int vC = faces[i][4] - 1;
		int vnA = faces[i][1] - 1;
		int vnB = faces[i][3] - 1;
		int vnC = faces[i][5] - 1;
		glNormal3f(normals[vnA][0],normals[vnA][1] ,normals[vnA][2]);
		glVertex3f(positions[vA][0],positions[vA][1] ,positions[vA][2] );
		glNormal3f(normals[vnB][0],normals[vnB][1] ,normals[vnB][2]);
		glVertex3f(positions[vB][0],positions[vB][1] ,positions[vB][2] );
		glNormal3f(normals[vnC][0],normals[vnC][1] ,normals[vnC][2]);
		glVertex3f(positions[vC][0],positions[vC][1] ,positions[vC][2] );
		glEnd();
	}
	glPopMatrix();
	//pause to prevent flickering
	if(rotz >0){
	rotz -= 0.5;
	}
	if(rotz <0){
	rotz +=0.5;
	}
	if(rotx >0){
	rotx -= 0.5;
	}
	if(rotx <0){
	rotx +=0.5;
	}
	}
}

/* createdNormalVectors - creates the normal vectors for each
vertice by getting the cross product*/
void createNormalVectors(){
	float v1[3];
	float v2[3];
	float vfinal[3];
	for(int i =0;i<SceneX; i++){
		for(int j = 0;j<SceneY;j++){
		v1[0] = i+1;
		v1[1] = 0;
		v1[2] = j;
		
		v2[0] = i+1;
		v2[1] = 0;
		v2[2] = j+1;
		
		//perform cross product calculation
		vfinal[0] = v1[1]*v2[2] - v1[2]*v2[1];
		vfinal[1] = v1[2]*v2[0] - v1[0]*v2[2];
		vfinal[2] = v1[0]*v2[2] - v1[2]*v2[0];
		float length = sqrtf(vfinal[0]*vfinal[0] + vfinal[1]*vfinal[1] 
						+ vfinal[2]*vfinal[2]);


		}
	}


}

void keyboard(unsigned char key, int x, int y)
{

	/* key presses move the cube, if it isn't at the extents (hard-coded here) */
	switch (key)
	{	glMatrixMode(GL_MODELVIEW);
		case 'q':
		case 27:
			exit (0);
			break;

		case 'r':
		case 'R':
			easyMode = !easyMode;
			break;

		case 'w':
		case 'W':
			
			
			break;

		case 'e':
		case 'E':
			if(lightOn){
			lightOn = !lightOn;
			glDisable(GL_LIGHT0);
			glDisable(GL_LIGHTING);
			}else{
			lightOn = !lightOn;
			glEnable(GL_LIGHT0);
			glEnable(GL_LIGHTING);
			}
			break;
		case 't':
		case 'T':
			
			break;
		case 'i':
		case 'I':
			lightpos1[2] +=1;
			lightpos2[2] -=1;
			break;
		case 'k':
		case 'K':
			lightpos1[2] -=1;
			lightpos2[2] +=1;
			break;
		case 'j':
		case 'J':
			lightpos1[0] +=1;
			lightpos2[0] -=1;
			break;
		case 'l':
		case 'L':
			lightpos1[0] -=1;
			lightpos2[0] +=1;
			break;
		case 'o':
		case 'O':
			lightpos1[1] +=1;
			lightpos2[1] -=1;
			break;
		case 'u':
		case 'U':
			lightpos1[0] +=1;
			lightpos2[0] -=1;
			break;
		case ' ':
			shoot = 1;
			break;
		case 'n':
			newLevel = 1;
			break;
		
			
	}
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glLightfv(GL_LIGHT0,GL_POSITION,lightpos1);
	glLightfv(GL_LIGHT1,GL_POSITION,lightpos2);
	glPopMatrix();
	glutPostRedisplay();
}
/*reset - used to reset the heightmap and clears the current mesh*/
void reset(){
	eye[0] = 0;
	eye[1] = 5;
	eye[2] = 0;
}
void special(int key, int x, int y)
{
	/* arrow key presses move the camera */
    /************************************************************************
     
                        CAMERA CONTROLS
     
     ************************************************************************/
	switch(key)
	{
	case GLUT_KEY_LEFT: 
		if(rotx <=75)
		rotx +=3;
		if(eye[0] != 10){
		eye[0] -=1;
		dot[0] -=1;
		}
	break; 
	case GLUT_KEY_RIGHT: 
		if(rotx >=-75)
		rotx -=3;
		if(eye[0] != 90){
		eye[0] +=1;
		dot[0] +=1;
		}
	break;
	case GLUT_KEY_DOWN:
		if(rotz  >=-75) 
		rotz -=3;
		if(eye[1] != 10){
		eye[1] -=1;
		dot[1] -=1;
		}
		 
	break; 
	case GLUT_KEY_UP:
		//stop moving up if it is larger than max height 
		if(rotz <=75)
		rotz +=3;
		if(eye[1] != 90){
		eye[1] +=1;
		dot[1] +=1;
		}
	break;
	case GLUT_KEY_PAGE_UP:
		eye[2] -=1;//into the screen
		

	break;
	case GLUT_KEY_PAGE_DOWN:
		eye[2] +=1;//out of the screen
	break;		
    }
	
	glutPostRedisplay();
}
/*menu - used to change the size of the grid by using right clicks*/
void menu(int key){
	//these keysfunction will handle all creations of the objects
	
	glFlush();
	glutPostRedisplay();


}
void drawFace(void){
	glBindTexture(GL_TEXTURE_2D,13);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex3f(0.0,0.0,0.0);
	glTexCoord2f(1,0);
	glVertex3f(10.0,0.0,0.0);
	glTexCoord2f(1,1);
	glVertex3f(10.0,10.0,0.0);
	glTexCoord2f(0,1);
	glVertex3f(0.0,10.0,0.0);
	glEnd();

}
void init(void)
{	
	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);
    	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);
    	glLightfv(GL_LIGHT0,GL_POSITION,lightpos1);
	glLightfv(GL_LIGHT1,GL_POSITION,lightpos2);
       	glMatrixMode(GL_PROJECTION); 
	glFrustum(-10,10,-1000,1000,0,1000);
	gluPerspective(45, 1, 1, 600); 
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	/**
	tex = LoadPPM("snail_a.ppm", &width, &height, &max);
	glEnable(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,0);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	**/
}
void createMenu(){
	glutCreateMenu(menu);
	glutAddMenuEntry("New Game", '1');
	glutAddMenuEntry("Load Game", '2');
	glutAddMenuEntry("Exit", '3');
	}
/* display function - GLUT display callback function
 *		clears the screen, sets the camera position, draws the ground plane and movable box
 */
void display(void)
{
	
	//***************Material setup*****************//
	float amb[] = {0.72,0.43,0.71,1.0};
	float diff[] = {0.28,0.70,0.12,1.0};
	float spec[] = {0.98,0.90,0.79,1.0};
	float shiny = 23;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
	glColorMaterial(GL_AMBIENT,GL_DIFFUSE);
	//************** Light setup******************//
	GLfloat DiffuseLight1[] = {0.8,0.9,0.0};//red light
	GLfloat AmbientLight1[] = {0.1,0.02,0.3};
	GLfloat DiffuseLight2[] = {0.0,1.0,0.0};//green light
	GLfloat AmbientLight2[] = {0.0,1.0,0.0};
	glLightfv(GL_LIGHT0,GL_DIFFUSE,DiffuseLight1);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,DiffuseLight2);
	glLightfv(GL_LIGHT0,GL_AMBIENT,AmbientLight1);
	glLightfv(GL_LIGHT1,GL_AMBIENT,AmbientLight2);



	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//******************** Camera Controls**********************//
	float origin[3] = {0,0,0};
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2], dot[0], dot[1], dot[2], 0, 1, 0); // have it look at the centre of mesh, z axis is flipped
	//******************** Scene and Timer Setup****************//
	if(pause){//while game IS Paused
	w = 0;
	}
	Timer();
	DrawScene();
	DrawPlanets();
	DrawPlayer();//THIS HAS TO COME BEFORE ENEMIES TO ENSURE THE SCENE IS LOADED.
	DrawEnemies();
	DrawParticle();
	DrawBullets();
	
	//drawFace();
	glutSwapBuffers();
}

/* intstructions - text that is displayed in the terminal to let
the user know how to use the program*/
void instructions(void){
	printf("Here are the instructions for the program:\n");
	


}
/* main function - program entry point */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);		//starts up GLUT
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	
	
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("Modeller");	//creates the window

	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	
	glEnable(GL_DEPTH_TEST);
	instructions();
	init();
	glutMainLoop();				//starts the event loop
	
	return(0);					//return may not be necessary on all compilers
}
