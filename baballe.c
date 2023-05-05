/////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////                                /////////////////////////////////////
//////////////////////////// Baballe ! par Julien COPPOLANI /////////////////////////////////////
////////////////////////////                                /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>              // fonctions sprintf, fopen, fread, fclose, ftell et fseek
#include <stdlib.h>             // fonction rand
#include <math.h>               // fonctions cos, sin, floor et fabs
#include <GL/glut.h>


// constantes
#define TAILLE_CIRCUIT 1024
#define DIST_VUE 8
#define PI 3.14159265358979
#define COS(X)   cos( (X) * PI/180.0 )
#define SIN(X)   sin( (X) * PI/180.0 )
enum { INVERT, FPS, SCORE, BOUNCING, RESTART };
enum { NOIR, ROUGE, VERT, BLEU, JAUNE_NOIR, CYAN_BLACK, MAGENTA, MULTI };
enum { SHORT_VIEW, FAR_VIEW };
enum { MARB118, MARB053, WOOD022, WOOD008, MULTI024, ROCK007, ROCK008, MULTI095,
MULTI005, MULTI119, WATER008 };
//variables globales
char titre[19] = "Baballe ! par JCOP";
char titre2[] = "Baballe ! par JCOP - Press <p> to begin.";
char buffer[25];
char buffer2[64*64*3];  // pour la texture
//GLboolean debut_partie = GL_TRUE;
GLubyte circuit[TAILLE_CIRCUIT][16][3];
GLfloat zb = 0.0, xb = 0.0, yb = 0.0;
GLfloat vitesse = 0.0;
GLfloat v_lateral = 0.0;
GLfloat v_haut = 0.0;
GLfloat v_haut_init = 5.0;
GLfloat v_haut_init_b = 5.0;
GLfloat v_max = 0.0, v_moy, dist_parcourue = 0.0;
GLint the_sphere;
GLfloat gravitation = 10.0;
GLfloat acceleration = 5.0;
GLshort anticipe_saut = GL_FALSE;
GLbyte couleur_de_fond = 'N';
GLfloat couleur_terrain[3] = {1.0, 1.0, 1.0};
GLfloat couleur_balle0[3] = {1.0, 0.0, 0.0};
GLfloat couleur_balle1[3] = {1.0, 1.0, 1.0};
GLshort score = 0;
GLint playtime = 1;
GLshort debut_aff_z = 0;
GLint retries = 0;
int mouse_x_init, mouse_y_init;                         // pour changer la vue par la souris
GLfloat mouse_theta_ini = 0.0, mouse_phi_ini = PI/12, mouse_theta, mouse_phi;
GLfloat cos_phi, cos_theta, sin_phi, sin_theta;
GLshort vue = 17;
GLshort nb_frames = 0;
GLboolean is_timer = GL_FALSE, aff_score = GL_FALSE, aff_vitesse = GL_FALSE;
GLint lasttime = 0;
int paused = GL_TRUE;
int left = 0, right = 0, up = 0, down = 0, brake_left = 0, brake_right = 0;
int slices = 8, stacks = 16;
GLfloat radius = 0.5;
GLboolean left_button = GL_FALSE, perdu = GL_FALSE, end = GL_FALSE;
GLshort pos_z;
GLboolean bounce = GL_FALSE;
static GLuint texName[11];                                      // pour la texture
GLuint current_texture = 0;
GLshort taille_circuit = TAILLE_CIRCUIT;        // taille du circuit
GLboolean multi = GL_FALSE;                                     // balle multicolore ou non

/////////////////////////////////////////// sphere //////////////////////////////////////////////
/*void sphere(GLint slices, GLint stacks)
{
        //Le nombre de vertexes est égal à : (slices+1)*stacks;
        static GLfloat vertex[544][3]; // ici on limite à slices = 16 et stacks = 32
        GLfloat phi, theta;
        int i=0, j=0, k;
        GLboolean l;
        GLfloat stacks_div = 360.0/stacks;
        GLfloat slices_div = 180.0/slices;
        GLshort nb_points = (slices+1)*stacks;
        GLfloat t, p;

        for(i=0,phi=0.0; i<stacks; phi+=stacks_div,i++) {
                p = phi/180*PI;
                theta=2.0;
                j=0;
                while (j<=slices) {
                        t = theta/180*PI;
                        k=(slices+1)*i+j;
                        vertex[k][0] = sin(t)*cos(p);
                        vertex[k][1] = cos(t);
                        vertex[k][2] = sin(t)*sin(p);
                        if ((j==0)||(j==slices-1))
                                theta+=slices_div-2.0;
                        else
                                theta+=slices_div;
                        j++;
                }
        }
        // Nbre d'appels à glVertex3f : 
        for(i=0;i<stacks;i++) {
                if (i%2)
                        l = GL_TRUE;
                else
                        l = GL_FALSE;
                glBegin(GL_QUAD_STRIP);
                for(j=0;j<=slices;j+=2) {
                        k = (slices+1)*i+j;
                                l = !l;
                                if (l)
                                        glColor3fv(couleur_balle0);
                                else
                                        glColor3fv(couleur_balle1);
                        glVertex3f(vertex[k][0], vertex[k][1], vertex[k][2]);
                        glVertex3f(vertex[(k+slices+1)%nb_points][0], vertex[(k+slices+1)%nb_points][1], vertex[(k+slices+1)%nb_points][2]);
                        if (j!=slices) {
                                l = !l;
                                if (l)
                                        glColor3fv(couleur_balle0);
                                else
                                        glColor3fv(couleur_balle1);
                                glVertex3f(vertex[k+1][0], vertex[k+1][1], vertex[k+1][2]);
                                glVertex3f(vertex[(k+slices+2)%nb_points][0], vertex[(k+slices+2)%nb_points][1], vertex[(k+slices+2)%nb_points][2]);
                        }
                }
                glEnd();
        }
}*/
void create_sphere(GLint slices, GLint stacks, GLfloat radius)
{
        // a = theta, b= phi
        GLfloat slices_div=180.0/slices;
        GLfloat stacks_div = 360.0/stacks;
        GLfloat phi, theta = -88.0;
        GLuint color=0;
        GLfloat x, y, z;
        int i=0, j;

        glNewList(the_sphere, GL_COMPILE);
        //for (theta = -90.0, i = 0; i < slices; theta += slices_div, i++) {
        while (i<slices) {
                glBegin(GL_QUAD_STRIP);
                for (phi = 0.0, j=0; j <= stacks; phi += stacks_div, j++) {
                        if (!multi) {
                                if (color)
                                        glColor3fv(couleur_balle0);
                                else
                                        glColor3fv(couleur_balle1);
                        }
                        else
                                glColor3f(rand()/32768.0, rand()/32768.0, rand()/32768.0);
                        x = radius * COS(phi) * COS(theta);
                        y = radius * SIN(phi) * COS(theta);
                        z = radius * SIN(theta);
                        glVertex3f(x, y, z);
                        x = radius * COS(phi) * COS(theta + (((i==0)||(i==slices-1))?slices_div-2.0:slices_div));
                        y = radius * SIN(phi) * COS(theta + (((i==0)||(i==slices-1))?slices_div-2.0:slices_div));
                        z = radius * SIN(theta + (((i==0)||(i==slices-1))?slices_div-2.0:slices_div));
                        glVertex3f(x, y, z);
                        color = 1 - color;
                }
                glEnd();
                if (i==0)
                        theta+=slices_div-2.0;
                else
                        theta+=slices_div;
                i++;
        }
        glEndList();
}
/////////////////////////////////////////// textString //////////////////////////////////////////
void textString(int x, int y, char *msg, void *font)
{
        glRasterPos2f(x, y);
        while (*msg) {
                glutBitmapCharacter(font, *msg);
                msg++;
        }
}

////////////////////////////////////////// strokeString /////////////////////////////////////////
void strokeString(int x, int y, char *msg, void *font)
{
        glPushMatrix();
        glTranslatef(x, y, 0);
        glScalef(0.03125, 0.03125, 1);
        while (*msg) {
                glutStrokeCharacter(font, *msg);
                msg++;
        }
        glPopMatrix();
}

/////////////////////////////////////////// filesize ////////////////////////////////////////////
long filesize(FILE *stream) {
        long curpos, length;

        curpos = ftell(stream);
        fseek(stream, 0L, SEEK_END);
        length = ftell(stream);
        fseek(stream, curpos, SEEK_SET);
        return length;
}

///////////////////////////////////////// display_score /////////////////////////////////////////
void display_score(void)
{
        static char buff[6];
        GLint time;
        if ((aff_score)||(perdu)||(end)||(aff_vitesse)) {
                glMatrixMode(GL_PROJECTION);
                glPushMatrix();
                glLoadIdentity();
                gluOrtho2D(0, 100, 0, 100);
                glMatrixMode(GL_MODELVIEW);
                glPushMatrix();
                glLoadIdentity();
                if (aff_vitesse) {
                        glColor3fv(couleur_terrain);
                        sprintf(buff, "%5.2f", vitesse);
                        strokeString(82, 96, buff, GLUT_STROKE_MONO_ROMAN);
                }
                if ((aff_score)||(perdu)||(end)) {
                        //glDisable(GL_DEPTH_TEST);
                        //glDisable(GL_LIGHTING);
                        glEnable(GL_BLEND);
                        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                        glColor4f(1.0, 1.0, 0.0, 0.75);
                        glRecti(18, 33, 83, 75);
                        glDisable(GL_BLEND);
                        glLineWidth(2.0);
                        glColor3f(1.00, 0.5, 0.0);
                        glBegin(GL_LINE_LOOP);
                        glVertex2i(18, 33);
                        glVertex2i(83, 33);
                        glVertex2i(83, 75);
                        glVertex2i(18, 75);
                        glEnd();
                        glLineWidth(1.0);

                        //glLineWidth(lineWidth);
                        glColor3f(0.0, 0.0, 1.0);
                        strokeString(43, 70, "SCORE", GLUT_STROKE_ROMAN);
                        strokeString(20, 60, "Position", GLUT_STROKE_ROMAN);
                        strokeString(20, 55, "Best Position", GLUT_STROKE_ROMAN);
                        strokeString(20, 50, "Average Speed", GLUT_STROKE_ROMAN);
                        strokeString(20, 45, "Max Speed", GLUT_STROKE_ROMAN);
                        strokeString(20, 40, "Time", GLUT_STROKE_ROMAN);
                        strokeString(20, 35, "Retries", GLUT_STROKE_ROMAN);

                        time = playtime / 1000;
                        v_moy = dist_parcourue * 1000 / playtime;

                        sprintf(buff, "%5i", debut_aff_z);
                        strokeString(65, 60, buff, GLUT_STROKE_MONO_ROMAN);
                        sprintf(buff, "%5i", score);
                        strokeString(65, 55, buff, GLUT_STROKE_MONO_ROMAN);
                        sprintf(buff, "%5.2f", v_moy);
                        strokeString(65, 50, buff, GLUT_STROKE_MONO_ROMAN);
                        sprintf(buff, "%5.2f", v_max);
                        strokeString(65, 45, buff, GLUT_STROKE_MONO_ROMAN);
                        sprintf(buff, "%5i", time);
                        strokeString(65, 40, buff, GLUT_STROKE_MONO_ROMAN);
                        sprintf(buff, "%5i", retries);
                        strokeString(65, 35, buff, GLUT_STROKE_MONO_ROMAN);
                        if (end) {
                                glColor3f(1.0, 0.0, 0.0);
                                strokeString(30, 80, "CONGRATULATIONS !", GLUT_STROKE_ROMAN);
                        }
                        //glEnable(GL_DEPTH_TEST);
                        //glEnable(GL_LIGHTING);
                }
                glMatrixMode(GL_PROJECTION);
                glPopMatrix();
                glMatrixMode(GL_MODELVIEW);
                glPopMatrix();
        }
}

/////////////////////////////////////// display_the_end /////////////////////////////////////////
void display_the_end(void)
{
        GLfloat i = (GLfloat)taille_circuit;

        // !! glPushMatrix();
        // !! glTranslatef(-xb, 0.0, -zb-DIST_VUE);
        glPushMatrix();
        glTranslatef(-3.0, 0.0, 0.0);
        // Affichage du 'E'
        glBegin(GL_LINE_LOOP);
        glVertex3f(5.5, 4.0, i); glVertex3f(5.5, 2.0, i);
        glVertex3f(4.0, 2.0, i); glVertex3f(4.0, 2.4, i);
        glVertex3f(5.0, 2.4, i); glVertex3f(5.0, 2.8, i);
        glVertex3f(4.0, 2.8, i); glVertex3f(4.0, 3.2, i);
        glVertex3f(5.0, 3.2, i); glVertex3f(5.0, 3.6, i);
        glVertex3f(4.0, 3.6, i); glVertex3f(4.0, 4.0, i);
        glEnd();
        // Affichage du 'N'
        glBegin(GL_LINE_LOOP);
        glVertex3f(3.5, 2.0, i); glVertex3f(3.0, 2.0, i);
        glVertex3f(3.0, 3.0, i); glVertex3f(2.5, 2.0, i);
        glVertex3f(2.0, 2.0, i); glVertex3f(2.0, 4.0, i);
        glVertex3f(2.5, 4.0, i); glVertex3f(2.5, 3.0, i);
        glVertex3f(3.0, 4.0, i); glVertex3f(3.5, 4.0, i);
        glEnd();
        // Affichage du 'D'
        glBegin(GL_LINE_LOOP);
        glVertex3f(1.5, 2.0, i); glVertex3f(0.5, 2.0, i);
        glVertex3f(0.0, 2.5, i); glVertex3f(0.0, 3.5, i);
        glVertex3f(0.5, 4.0, i); glVertex3f(1.5, 4.0, i);
        glEnd();
        glPopMatrix();
}

///////////////////////////////////////////// init //////////////////////////////////////////////
void init(void)
{
        int i;
        GLboolean error = GL_FALSE;
        char *names[11] = { "marb118.raw", "marb053.raw", "wood022.raw", "wood008.raw",
                "multi024.raw", "rock007.raw", "rock008.raw", "multi095.raw",
                "multi005.raw", "multi119.raw", "water008.raw" };
        char textname[22];
        FILE *f;
        FILE *f_circuit;

        if ((f_circuit = fopen("circuits/circuit0.raw", "r"))!=NULL) {
                taille_circuit = filesize(f_circuit);  // taille en bytes
                if (taille_circuit%48 == 0) {
                        fread(circuit, taille_circuit, 1, f_circuit);
                        fclose(f_circuit);
                        taille_circuit /= 48;   // taille en longueur
                }
                else {
                        error = GL_TRUE;
                        printf("Taille du fichier incorrecte !!");
                }
        }
        else
                error = GL_TRUE;
        if (error) {
                f_circuit = fopen("circuits/circuit.raw", "r");
                if (f_circuit)
                {
                        fread(circuit,TAILLE_CIRCUIT*16*3, 1, f_circuit);
                        fclose(f_circuit);
                }
        }
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glShadeModel(GL_FLAT);
        the_sphere = glGenLists(1);
        create_sphere(slices, stacks, radius);
        // Initialisation des angles de vue
        sin_phi = sin(mouse_phi_ini);
        sin_theta = sin(mouse_theta_ini);
        cos_phi = cos(mouse_phi_ini);
        cos_theta = cos(mouse_theta_ini);

        // chargement des textures
        glGenTextures(11, texName);
        for (i=0; i<11; i++) {
                sprintf(textname, "textures/%s", names[i]);
                f = fopen(textname, "r");
                if (f) {
                        fread(buffer2, 64*64*3, 1, f);
                        fclose(f);
                }
                glBindTexture(GL_TEXTURE_2D, texName[i]);
                //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
                glTexImage2D(GL_TEXTURE_2D, 0, 3, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer2);
        }
}

///////////////////////////////// routine d'affichage principal /////////////////////////////////
void display(void)
{
        static GLshort pos_x;
        GLint time = glutGet(GLUT_ELAPSED_TIME);
        //static DWORD temps_0 = temps;
        GLfloat delta = (time - lasttime)/1000.0;
        GLshort i, j;

        nb_frames++;    // pour le timer
        if ((!perdu)&&(!paused)&&(!end)) {
                playtime += time - lasttime;
                lasttime = time;
                if (left) {
                        //brake_right = 0;
                        v_lateral += 2*acceleration*delta;
                }
                else if (brake_left) {
                        v_lateral -= acceleration*delta;
                        if (v_lateral <= 0) {
                                v_lateral = 0.0;
                                brake_left = 0;
                        }
                }
                if (right) {
                        //brake_left = 0;
                        v_lateral -= 2*acceleration*delta;
                }
                else if (brake_right) {
                        v_lateral += acceleration*delta;
                        if (v_lateral >= 0) {
                                v_lateral = 0.0;
                                brake_right = 0;
                        }
                }
                if (up) {
                        vitesse += acceleration*delta;
                        if (vitesse > v_max)
                                v_max = vitesse;
                }
                else if (down) {
                        if (vitesse < -v_max)
                                v_max = -vitesse;
                        if (vitesse>=0)
                                vitesse -= 2*acceleration*delta;
                        else
                                vitesse -= acceleration*delta;
                }
                dist_parcourue += fabs(vitesse*delta);  // totalise la distance parcourue totale
                zb += vitesse*delta;
                xb += v_lateral*delta;
                yb += v_haut*delta;
                if (v_haut!=0.0)
                        v_haut -= gravitation*delta;
                if (yb<0.0) {
                        yb=0.0;
                        v_haut = 0.0;
                        if (anticipe_saut) {
                                v_haut = v_haut_init;
                                anticipe_saut = GL_FALSE;
                        }
                        else if (bounce) {
                                if (v_haut_init_b > 0.421875*v_haut_init) {
                                        v_haut_init_b *= 0.75;
                                        v_haut = v_haut_init_b;
                                }
                                else
                                        v_haut_init_b = v_haut_init;
                        }
                }
                if (zb < 0.0) {
                        zb = 0.0;
                        vitesse = 0.0;
                }
                debut_aff_z = (GLshort) zb;
        }// endif((!perdu)&&(!paused)&&(!end))

        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texName[current_texture]);
        ////// affichage du terrain //////
        glColor3fv(couleur_terrain);
        glPushMatrix();
        glTranslatef(-xb, 0.0, -zb-DIST_VUE);
        glBegin(GL_QUADS);
        for(j=debut_aff_z; j < debut_aff_z+vue; j++) {
                if (j < taille_circuit) {
                        for(i=0; i<16; i++) {
                                if ((circuit[j][i][1]==0)){//&&(circuit[j][i][1]==0)&&(circuit[j][i][2]==0)) {
                                        glTexCoord2s(0, 0); glVertex3i(i-8, 0, j);
                                        glTexCoord2s(0, 1); glVertex3i(i-7, 0, j);
                                        glTexCoord2s(1, 1); glVertex3i(i-7, 0, j+1);
                                        glTexCoord2s(1, 0); glVertex3i(i-8, 0, j+1);
                                }
                        }
                }
        }
        glEnd();
        glDisable(GL_TEXTURE_2D);
        if (debut_aff_z+vue > taille_circuit)
                display_the_end();
        glPopMatrix();
        //////////////////////////////////
        glEnable(GL_CULL_FACE);
        //glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
        glPushMatrix();
        glTranslatef(0.0, radius+yb, -6.0);
        glRotatef(zb*180/(PI*radius), 1.0, 0.0, 0.0);
        //glRotatef(-xb*180/(PI*radius), 0.0, 0.0, 1.0);
        //glScalef(0.5,0.5,0.5);
        glCallList(the_sphere);
        glPopMatrix();
        glDisable(GL_CULL_FACE);
        //glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
        if (debut_aff_z+2 >= taille_circuit) {
                end = GL_TRUE;
                glutIdleFunc(NULL);
        }
        // teste si la balle ne sort pas du circuit //
        else if (yb==0.0) {
                pos_z = (GLshort)zb+2;  // la balle est décalée sur l'axe z de -6 et le circuit de -8
                pos_x = floor(xb);
                if ((circuit[pos_z][pos_x+8][1]!=0)) {//||(circuit[pos_z][pos_x+7][1]!=0)||(circuit[pos_z][pos_x+7][2]!=0)) {
                        perdu = GL_TRUE;
                        glutIdleFunc(NULL);
                }
                else if (debut_aff_z > score)
                        score = debut_aff_z;
        }
        display_score();
        glutSwapBuffers();
}

//////////////////////////////////////////// reshape ////////////////////////////////////////////
void reshape(int w, int h)
{
        glViewport(0, 0, (GLsizei) w, (GLsizei) h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective (90.0, ((GLfloat) w) / ((GLfloat) h),0.5,64.0);
        glMatrixMode (GL_MODELVIEW);
        glLoadIdentity ();
        gluLookAt (DIST_VUE*sin_theta*cos_phi, DIST_VUE*sin_phi, -DIST_VUE*cos_phi*cos_theta,
                0.0, 0.0, 0.0,
                -DIST_VUE*sin_theta*sin_phi, DIST_VUE*cos_phi, DIST_VUE*sin_phi*cos_theta);
        glCullFace(GL_FRONT);
        //glEnable(GL_POLYGON_SMOOTH);
}

///////////////////////////////////////////// timer /////////////////////////////////////////////
void timer(int value)
{
        // value = 0
        if (is_timer) {
                if (value==0) {
                        glutTimerFunc(1000, timer, 0);
                        sprintf(buffer, "%s - %i", titre, nb_frames);
                        glutSetWindowTitle(buffer);
                        nb_frames = 0;
                }
        }
        else
                glutSetWindowTitle(titre);
}

//////////////////////////////////////////// special ////////////////////////////////////////////
void special (int key, int x, int y)
{
        switch (key) {
        case GLUT_KEY_UP:
                up = 1;
                break;
        case GLUT_KEY_DOWN:
                down = 1;
                break;
        case GLUT_KEY_LEFT:
                left = 1;
                brake_left = 0;
                break;
        case GLUT_KEY_RIGHT:
                right = 1;
                brake_right = 0;
                break;
        default:
                break;
        }
}

/////////////////////////////////////////// specialup ///////////////////////////////////////////
void specialup(int key, int x, int y)
{
        switch (key) {
        case GLUT_KEY_UP:
                up = 0;
                break;
        case GLUT_KEY_DOWN:
                down = 0;
                break;
        case GLUT_KEY_LEFT:
                left = 0;
                brake_left = 1;
                break;
        case GLUT_KEY_RIGHT:
                right = 0;
                brake_right = 1;
                break;
        }
}

//////////////////////////////////////////// keyboard ///////////////////////////////////////////
void keyboard (unsigned char key, int x, int y)
{
        static char buff_pause[] = "pause";
        int i = 0;
        int j = (score < pos_z) ? score : pos_z;
        switch (key) {
        case ' ':
                if (!paused) {
                        if (v_haut==0)
                                v_haut=v_haut_init;
                        else
                                anticipe_saut=1;
                }
                break;
        case 13:
                if (end) {
                        xb = zb = yb = 0.0;
                        v_haut = vitesse = v_lateral = v_max = 0.0;
                        score = 0; retries = 0; dist_parcourue = 0.0; playtime = 1;
                        debut_aff_z = 0;
                        end = GL_FALSE;
                        glutIdleFunc(display);
                        lasttime = glutGet(GLUT_ELAPSED_TIME);
                }
                else if (perdu) {
                        retries++;
                        perdu = GL_FALSE;
                        v_haut = vitesse = v_lateral = 0.0;
                        // Recherche du point de départ antérieur //
                        xb = zb = 0.0;
                        debut_aff_z = 0;
                        j -= 20;
                        while (j > 2) {
                                if ((circuit[j][i][0]==0xFF)&&(circuit[j][i][1]==0)) {
                                        zb = j - 1.5;
                                        debut_aff_z = (GLshort) zb;
                                        xb = i - 7.5;
                                        break;
                                }
                                i++;
                                if (i==16) {
                                        i = 0;
                                        j--;
                                }
                        }
                        ////////////////////////////////////////////
                        glutIdleFunc(display);
                        lasttime = glutGet(GLUT_ELAPSED_TIME);
                }
                break;
        case 'i':
                if (couleur_de_fond=='N') {
                        glClearColor(1.0, 1.0, 1.0, 0.0);
                        couleur_terrain[0] = couleur_terrain[1] = couleur_terrain[2] = 0.0;
                        couleur_de_fond = 'B';
                }
                else {
                        glClearColor(0.0, 0.0, 0.0, 0.0);
                        couleur_terrain[0] = couleur_terrain[1] = couleur_terrain[2] = 1.0;
                        couleur_de_fond = 'N';
                }
                glutPostRedisplay();
                break;
        case 'p':
        paused = !paused;
        if (paused) {
                glutIdleFunc(NULL);
                sprintf(buffer, "%s - %s", titre, buff_pause);
                glutSetWindowTitle(buffer);
        }
        else {
                glutIdleFunc(display);
                glutSetWindowTitle(titre);
                lasttime = glutGet(GLUT_ELAPSED_TIME);
        }
                break;
        case 'v':
                aff_vitesse = !aff_vitesse;
                break;
        /*case 't':
                if (!is_timer) {
                        is_timer = 1;
                        nb_frames = 0;
                        //glutChangeToMenuEntry(2, "Hide FPS", FPS);
                        glutTimerFunc(1000, timer, 0);
                }
                else {
                        is_timer = 0;
                        //glutChangeToMenuEntry(2, "Display FPS", FPS);
                }
                break;*/
        /*case 'v':
                if (vue==17)
                        vue = 43;
                else
                        vue = 17;
                break;*/
        default:
                break;
        }
}

//////////////////////////////////////////// mouse //////////////////////////////////////////////
void mouse(int button, int state, int x, int y)
{
        static GLboolean press_down = GL_FALSE;

        switch(button) {
        case GLUT_LEFT_BUTTON:
                if (state==GLUT_UP) {
                        left_button = GL_FALSE;
                        press_down = GL_FALSE;
                        mouse_phi_ini = mouse_phi;
                        mouse_theta_ini = mouse_theta;
                }
                else if (state==GLUT_DOWN) {
                        left_button = GL_TRUE;
                        if (press_down==GL_FALSE) {
                                press_down = GL_TRUE;
                                mouse_x_init = x;
                                mouse_y_init = y;
                        }
                }
                break;
        default:
                break;
        }
}

//////////////////////////////////////////// motion /////////////////////////////////////////////
void motion(int x, int y)
{

        if (left_button) {

                mouse_theta = PI/360*(x-mouse_x_init) + mouse_theta_ini;
                mouse_phi = PI/360*(y-mouse_y_init) + mouse_phi_ini;
                if (mouse_theta>PI/2)
                        mouse_theta = PI/2;
                else if (mouse_theta<-PI/2)
                        mouse_theta = -PI/2;
                if (mouse_phi>PI/2)
                        mouse_phi = PI/2;
                else if (mouse_phi<PI/36)
                        mouse_phi = PI/36;
                sin_phi = sin(mouse_phi);
                sin_theta = sin(mouse_theta);
                cos_phi = cos(mouse_phi);
                cos_theta = cos(mouse_theta);
                glLoadIdentity ();
                gluLookAt (DIST_VUE*sin_theta*cos_phi, DIST_VUE*sin_phi, -DIST_VUE*cos_phi*cos_theta,
                        0.0, 0.0, 0.0,
                        -DIST_VUE*sin_theta*sin_phi, DIST_VUE*cos_phi, DIST_VUE*sin_phi*cos_theta);
                glutPostRedisplay();
        }
}

///////////////////////////////////////// visibility ////////////////////////////////////////////
void visibility(int state)
{
        switch(state) {
        case GLUT_NOT_VISIBLE:
                glutIdleFunc(NULL);
                break;
        case GLUT_VISIBLE:
                glutIdleFunc(display);
                break;
        }
}

//////////////////////////////////////////// menu ///////////////////////////////////////////////
void menu(int value)
{
        switch (value) {
        case INVERT:
                if (couleur_de_fond=='N') {
                        glClearColor(1.0, 1.0, 1.0, 0.0);
                        couleur_terrain[0] = couleur_terrain[1] = couleur_terrain[2] = 0.0;
                        couleur_de_fond = 'B';
                }
                else {
                        glClearColor(0.0, 0.0, 0.0, 0.0);
                        couleur_terrain[0] = couleur_terrain[1] = couleur_terrain[2] = 1.0;
                        couleur_de_fond = 'N';
                }
                break;
                //glutPostRedisplay();
        case FPS:
                if (!is_timer) {
                        is_timer = GL_TRUE;
                        nb_frames = 0;
                        glutChangeToMenuEntry(2, "Hide FPS", FPS);
                        glutTimerFunc(1000, timer, 0);
                }
                else {
                        is_timer = GL_FALSE;
                        glutChangeToMenuEntry(2, "Display FPS", FPS);
                }
                break;
        case SCORE:
                if (!aff_score)
                        glutChangeToMenuEntry(3, "Hide Score", SCORE);
                else
                        glutChangeToMenuEntry(3, "Display Score", SCORE);
                aff_score = !aff_score;
                break;
        case BOUNCING:
                if (!bounce)
                        glutChangeToMenuEntry(4, "Bounce off", BOUNCING);
                else
                        glutChangeToMenuEntry(4, "Bounce on", BOUNCING);
                bounce = !bounce;
                break;
        case RESTART:
                xb = zb = yb = 0.0;
                debut_aff_z = 0;
                v_haut = vitesse = v_lateral = v_max = 0.0;
                score = 0; retries = 0; dist_parcourue = 0.0; playtime = 1;
                perdu = GL_FALSE;
                break;
        }
        lasttime = glutGet(GLUT_ELAPSED_TIME);
        glutPostRedisplay();
}

///////////////////////////////////////// menu_slices ///////////////////////////////////////////
void menu_slices(int value)
{
        slices = value;
        create_sphere(slices,stacks, radius);
        lasttime = glutGet(GLUT_ELAPSED_TIME);
        glutPostRedisplay();
}

///////////////////////////////////////// menu_stacks ///////////////////////////////////////////
void menu_stacks(int value)
{
        stacks = value;
        create_sphere(slices,stacks, radius);
        lasttime = glutGet(GLUT_ELAPSED_TIME);
        glutPostRedisplay();
}

///////////////////////////////////////// menu_radius ///////////////////////////////////////////
void menu_radius(int value)
{
        radius = (GLfloat)value/8;
        create_sphere(slices,stacks, radius);
        lasttime = glutGet(GLUT_ELAPSED_TIME);
        glutPostRedisplay();
}

///////////////////////////////////////// menu_colors ///////////////////////////////////////////
void menu_colors(int value)
{
        multi = GL_FALSE;
        couleur_balle1[0] = couleur_balle1[1] = couleur_balle1[2] = 1.0;
        switch (value) {
        case NOIR:
                couleur_balle0[0] = couleur_balle0[1] = couleur_balle0[2] = 0.0;
                break;
        case ROUGE:
                couleur_balle0[0] = 1.0;
                couleur_balle0[1] = couleur_balle0[2] = 0.0;
                break;
        case VERT:
                couleur_balle0[1] = 0.75;
                couleur_balle0[0] = couleur_balle0[2] = 0.0;
                break;
        case BLEU:
                couleur_balle0[2] = 1.0;
                couleur_balle0[0] = couleur_balle0[1] = 0.0;
                break;
        case JAUNE_NOIR:
                couleur_balle0[0] = couleur_balle0[1] = 1.0;
                couleur_balle0[2] = 0.0;
                couleur_balle1[0] = couleur_balle1[1] = couleur_balle1[2] = 0.0;
                break;
        case CYAN_BLACK:
                couleur_balle0[1] = couleur_balle0[2] = 1.0;
                couleur_balle0[0] = 0.0;
                couleur_balle1[0] = couleur_balle1[1] = couleur_balle1[2] = 0.0;
                break;
        case MAGENTA:
                couleur_balle0[0] = couleur_balle0[2] = 1.0;
                couleur_balle0[1] = 0.0;
                break;
        case MULTI:
                multi = GL_TRUE;
                break;
        }
        create_sphere(slices,stacks, radius);
        lasttime = glutGet(GLUT_ELAPSED_TIME);
        glutPostRedisplay();
}

////////////////////////////////////////// menu_vue /////////////////////////////////////////////
void menu_vue(int value)
{
        switch(value) {
        case SHORT_VIEW:
                vue = 17;
                break;
        case FAR_VIEW:
                vue = 25;
                break;
        }
        lasttime = glutGet(GLUT_ELAPSED_TIME);
        glutPostRedisplay();
}

///////////////////////////////////// menu_acceleration /////////////////////////////////////////
void menu_acceleration(int value)
{
        // t = 2 * v_haut_init / gravitation 
        //<=> pour t = 1 seconde, g = 2*v
        v_haut_init = v_haut_init_b = (GLfloat)value;
        acceleration = v_haut_init;
        gravitation = 2*v_haut_init;
        lasttime = glutGet(GLUT_ELAPSED_TIME);
}

//////////////////////////////////////// menu_sphere ////////////////////////////////////////////
void menu_sphere(int value)
{
        return;
        //lasttime = glutGet(GLUT_ELAPSED_TIME);
}

//////////////////////////////////////// menu_texture ////////////////////////////////////////////
void menu_texture(int value)
{
        current_texture = value;
        lasttime = glutGet(GLUT_ELAPSED_TIME);
        glutPostRedisplay();
}

//////////////////////////////////////// createmenus ////////////////////////////////////////////
void createmenus(void)
{
        int mainmenu, submenu_slices, submenu_stacks, submenu_radius, submenu_acceleration,
                submenu_sphere, submenu_vue, submenu_colors, submenu_texture;
        char buff[2];
        int i;

        submenu_slices = glutCreateMenu(menu_slices);
        for (i=3;i<=14;i++) {
                sprintf(buff, "%i", i);
                glutAddMenuEntry(buff, i);
        }
        submenu_stacks = glutCreateMenu(menu_stacks);
        for (i=6;i<=28;i+=2) {
                sprintf(buff, "%i", i);
                glutAddMenuEntry(buff, i);
        }
        submenu_radius = glutCreateMenu(menu_radius);
        glutAddMenuEntry("0.25", 2);
        glutAddMenuEntry("0.375", 3);
        glutAddMenuEntry("0.5", 4);
        glutAddMenuEntry("0.75", 6);
        glutAddMenuEntry("1.0", 8);

        submenu_colors = glutCreateMenu(menu_colors);
        glutAddMenuEntry("Black", NOIR);
        glutAddMenuEntry("Red", ROUGE);
        glutAddMenuEntry("Green", VERT);
        glutAddMenuEntry("Blue", BLEU);
        glutAddMenuEntry("Yellow-black", JAUNE_NOIR);
        glutAddMenuEntry("Cyan-black", CYAN_BLACK);
        glutAddMenuEntry("Magenta", MAGENTA);
        glutAddMenuEntry("Multicolor", MULTI);

        submenu_acceleration = glutCreateMenu(menu_acceleration);
        for (i=2; i<11; i++) {
                sprintf(buff, "%i", i);
                glutAddMenuEntry(buff, i);
        }

        submenu_sphere = glutCreateMenu(menu_sphere);
        glutAddSubMenu("Radius", submenu_radius);
        glutAddSubMenu("Colors", submenu_colors);
        glutAddSubMenu("Slices", submenu_slices);
        glutAddSubMenu("Stacks", submenu_stacks);

        submenu_vue = glutCreateMenu(menu_vue);
        glutAddMenuEntry("Short", SHORT_VIEW);
        glutAddMenuEntry("Far", FAR_VIEW);

        submenu_texture = glutCreateMenu(menu_texture);
        glutAddMenuEntry("Marble1", MARB118);
        glutAddMenuEntry("Marble2", MARB053);
        glutAddMenuEntry("Wood1", WOOD022);
        glutAddMenuEntry("Wood2", WOOD008);
        glutAddMenuEntry("Rock1", MULTI024);
        glutAddMenuEntry("Rock2", ROCK007);
        glutAddMenuEntry("Rock3", ROCK008);
        glutAddMenuEntry("Multi1", MULTI095);
        glutAddMenuEntry("Multi2", MULTI005);
        glutAddMenuEntry("Multi3", MULTI119);
        glutAddMenuEntry("Water", WATER008);

        mainmenu = glutCreateMenu(menu);
        glutAddMenuEntry("Invert colors", INVERT);
        glutAddMenuEntry("Display FPS", FPS);
        glutAddMenuEntry("Display Score", SCORE);
        glutAddMenuEntry("Bouncing on", BOUNCING);
        glutAddMenuEntry("Restart", RESTART);

        glutAddSubMenu("Acceleration", submenu_acceleration);
        glutAddSubMenu("View", submenu_vue);
        glutAddSubMenu("Sphere", submenu_sphere);
        glutAddSubMenu("Texture", submenu_texture);
        glutAttachMenu(GLUT_RIGHT_BUTTON);
        glutSetMenu(mainmenu);
}

//////////////////////////////////// Entrée du programme ///////////////////////////////////////
int main(int argc, char **argv)
{
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
        glutInitWindowSize(512, 512);
        glutInitWindowPosition(100, 100);
        glutCreateWindow(titre2);
        glutSetCursor(GLUT_CURSOR_CROSSHAIR);
        glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
        createmenus();
        init();
        glutDisplayFunc(display);
        //glutIdleFunc(display);
        glutReshapeFunc(reshape);
        glutKeyboardFunc(keyboard);
        glutSpecialFunc(special);
        glutSpecialUpFunc(specialup);
        glutMouseFunc(mouse);
        glutMotionFunc(motion);
        glutVisibilityFunc(visibility);
        glutMainLoop();
        return 0;
}

