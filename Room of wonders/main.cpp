#define FREEGLUT_STATIC

#include<iostream>
#include<GLTools.h>
#include<GL/glut.h>
#include<GLMatrixStack.h>
#include<GLGeometryTransform.h>
#include<stopWatch.h>

#include <GLFrustum.h>
#include "header.h"

using namespace std;

// vertices
GLBatch floor_batch, sofa_batch, cushion_batch, dine_batch, tea_batch,
plant_batch, heron_batch, chair_batch, seat_batch, urn_batch,
wall_batch, candle_batch, candle_stand_batch, frame_batch,
photo_batch, legs_batch, top_batch, lamp_batch, shade_batch;

// shaders
GLuint shader;

// vertex shader uniforms
GLuint locNM, locMVP, locMV, locLight, locSign;    

// pixel shader uniforms
GLuint locTexture, locAmb, locDiff, 
locMaterial, locShine, locNoShine;

GLGeometryTransform transformPipeline;
GLMatrixStack modelViewMatrix, projectionMatrix;

GLuint Tex_bark, Tex_sofa, Tex_cush, Tex_dine, Tex_china, Tex_plant, Tex_heron, 
Tex_black, Tex_mud, Tex_wall, Tex_frame, Tex_photo, Tex_painting, Tex_marble, Tex_lamp;

GLFrame worldFrame;

float len=1280, wide=1024;


// vertex arrays read from obj file (containing only distinct vertices)
component *v, *vn, *vt;

// the final vertics given to openGL
M3DVector3f *Ver;
M3DVector3f *Normals;
M3DVector2f *vTexCoords; 	

// light position 
// in which space? normally this is given in in world space, but the vertex shader expects this in view space :(
M3DVector3f lightPosition = {-450, 240, 1335};  // world space
M3DVector3f viewSpaceLightPos;  // view space

class object
{	
public:

    GLfloat ambColor;
    GLfloat diffColor;
    GLfloat material;
    GLfloat translation1[3];
    GLfloat translation2[3];
    GLfloat translation3[3];
    GLfloat translation4[3];

    GLfloat rotation_x[4];
    GLfloat rotation_y[4];
    GLfloat rotation_z[4];
    GLfloat scale[3];
    GLfloat translateO;
    int noShine;
    float shininess;


    object()
    {

        ambColor = 0.01;
        diffColor = 1;
        material = 1;


        rotation_x[0]=0;
        rotation_x[1]=1;
        rotation_x[2]=0;
        rotation_x[3]=0;


        rotation_y[0]=0;
        rotation_y[1]=0;
        rotation_y[2]=1;
        rotation_y[3]=0;


        rotation_z[0]=0;
        rotation_z[1]=0;
        rotation_z[2]=0;
        rotation_z[3]=1;


        translation1[0]=0;
        translation1[1]=0;
        translation1[2]=0;			 

        translation2[0]=0;
        translation2[1]=0;
        translation2[2]=0;



        translation3[0]=0;
        translation3[1]=0;
        translation3[2]=0;			 

        translation4[0]=0;
        translation4[1]=0;
        translation4[2]=0;


        scale[0]=1;
        scale[1]=1;
        scale[2]=1;

        shininess=50;
        noShine=0;
        translateO=0;
    }
};

bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
    GLbyte *pBits;
    int nWidth, nHeight, nComponents;
    GLenum eFormat;

    // Read the texture bits
    pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
    if(pBits == NULL) 
        return false;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0,
        eFormat, GL_UNSIGNED_BYTE, pBits);

    free(pBits);

    if(minFilter == GL_LINEAR_MIPMAP_LINEAR || 
        minFilter == GL_LINEAR_MIPMAP_NEAREST ||
        minFilter == GL_NEAREST_MIPMAP_LINEAR ||
        minFilter == GL_NEAREST_MIPMAP_NEAREST)
        glGenerateMipmap(GL_TEXTURE_2D);    
    return true;
}

void freePointers()
{
    free(v);
    free(vn);
    free(vt);
    free(Ver);
    free(Normals);
    free(vTexCoords);

}


void fillBuffer(char fname[40],GLBatch *batch)
{

    FILE *fp;
    fp=fopen(fname,"r+");
    int total_ver=loadMesh(fp);	
    fclose(fp);
    GLBatch b;


    batch->Begin(GL_TRIANGLES,total_ver,1);
    batch->CopyVertexData3f(Ver);
    batch->CopyTexCoordData2f(vTexCoords,0);
    batch->CopyNormalDataf(Normals);	
    batch->End();

    freePointers();
}

void drawLine(float x1, float y1,float z1)
{
    glLineWidth(10);
    glBegin(GL_LINES);
    glVertex3f(x1, y1,z1);
    glVertex3f(0,0,0);
    glEnd();
}


void setup()
{
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glClearColor(0.4,0.4,0.9,1);
    worldFrame.RotateWorld(-0.3,1,0,0);	
    worldFrame.MoveForward(-3000);
    worldFrame.MoveUp(60);

    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);

    GLFrustum viewFrustum;	        
    viewFrustum.SetPerspective(35.0f, float(len)/float(wide),1, 5000);
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());

    int total_ver;

    fillBuffer("ground3.obj",&floor_batch);
    fillBuffer("11.obj",&sofa_batch);
    fillBuffer("pillow2.obj",&cushion_batch);
    fillBuffer("dine2.obj",&dine_batch);
    fillBuffer("tea.obj",&tea_batch);
    fillBuffer("plant.obj",&plant_batch);
    fillBuffer("heron.obj",&heron_batch);	
    fillBuffer("chairFrame.obj",&chair_batch);	
    fillBuffer("chairBase.obj",&seat_batch);	
    fillBuffer("urn.obj",&urn_batch);	
    fillBuffer("wall2.obj",&wall_batch);		
    fillBuffer("candles.obj",&candle_batch);	
    fillBuffer("candlestand.obj",&candle_stand_batch);		
    fillBuffer("frame.obj",&frame_batch);	
    fillBuffer("photo.obj",&photo_batch);	
    fillBuffer("legs.obj",&legs_batch);
    fillBuffer("top.obj",&top_batch);
    fillBuffer("lamp.obj",&lamp_batch);
    fillBuffer("shade.obj",&shade_batch);


    shader=gltLoadShaderPairWithAttributes("ver.vp", "frag.fp", 3, 
        GLT_ATTRIBUTE_VERTEX, "vPosition", 
        GLT_ATTRIBUTE_NORMAL, "vNormal", 
        GLT_ATTRIBUTE_TEXTURE0, "vTexture");

    // vertex shader's uniforms
    locNM=glGetUniformLocation(shader,"normalMatrix");
    locMVP=glGetUniformLocation(shader,"mvpMatrix");
    locMV=glGetUniformLocation(shader,"mvMatrix");
    locLight=glGetUniformLocation(shader,"lightPosition");
    locSign=glGetUniformLocation(shader,"sign");

    // fragment shader uniforms    
    locTexture=glGetUniformLocation(shader,"colorMap2D");
    locDiff=glGetUniformLocation(shader,"diffLight");
    locAmb=glGetUniformLocation(shader,"ambLight");
    locShine=glGetUniformLocation(shader,"shininess");
    locMaterial=glGetUniformLocation(shader,"material");
    locNoShine=glGetUniformLocation(shader,"noShine");


    glGenTextures(1,&Tex_bark);
    glBindTexture(GL_TEXTURE_2D,Tex_bark);	
    LoadTGATexture("floor.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);


    glGenTextures(1,&Tex_sofa);
    glBindTexture(GL_TEXTURE_2D,Tex_sofa);	
    LoadTGATexture("b_sofa.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);


    glGenTextures(1,&Tex_cush);
    glBindTexture(GL_TEXTURE_2D,Tex_cush);	
    LoadTGATexture("cush.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);



    glGenTextures(1,&Tex_dine);
    glBindTexture(GL_TEXTURE_2D,Tex_dine);	
    LoadTGATexture("wood.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);



    glGenTextures(1,&Tex_china);
    glBindTexture(GL_TEXTURE_2D,Tex_china);	
    LoadTGATexture("white.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);

    glGenTextures(1,&Tex_frame);
    glBindTexture(GL_TEXTURE_2D,Tex_frame);	
    LoadTGATexture("frame.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);


    glGenTextures(1,&Tex_photo);
    glBindTexture(GL_TEXTURE_2D,Tex_photo);	
    LoadTGATexture("photo.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);


    glGenTextures(1,&Tex_painting);
    glBindTexture(GL_TEXTURE_2D,Tex_painting);	
    LoadTGATexture("painting.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);	



    glGenTextures(1,&Tex_plant);
    glBindTexture(GL_TEXTURE_2D,Tex_plant);	
    LoadTGATexture("green.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);



    glGenTextures(1,&Tex_heron);
    glBindTexture(GL_TEXTURE_2D,Tex_heron);	
    LoadTGATexture("pink.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);



    glGenTextures(1,&Tex_mud);
    glBindTexture(GL_TEXTURE_2D,Tex_mud);	
    LoadTGATexture("gold.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);


    glGenTextures(1,&Tex_wall);
    glBindTexture(GL_TEXTURE_2D,Tex_wall);	
    LoadTGATexture("wallpaper1.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);



    glGenTextures(1,&Tex_marble);
    glBindTexture(GL_TEXTURE_2D,Tex_marble);	
    LoadTGATexture("marble.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);


    glGenTextures(1,&Tex_lamp);
    glBindTexture(GL_TEXTURE_2D,Tex_lamp);	
    LoadTGATexture("lamp.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);




}

void updateMatrices()
{
    glUniformMatrix3fv(locNM,1,GL_FALSE,transformPipeline.GetNormalMatrix());
    glUniformMatrix4fv(locMV,1,GL_FALSE,transformPipeline.GetModelViewMatrix());
    glUniformMatrix4fv(locMVP,1,GL_FALSE,transformPipeline.GetModelViewProjectionMatrix());
}

void bindDraw(GLBatch *batch,GLuint texName)
{
    glBindTexture(GL_TEXTURE_2D,texName);	
    batch->Draw();
}


void setUniforms(object obj)
{
    modelViewMatrix.PushMatrix();

    //glUniform3fv(locLight, 1, lightPosition);
    glUniform3fv(locLight, 1, viewSpaceLightPos);
    glUniform1f(locAmb, obj.ambColor);
    glUniform1f(locDiff, obj.diffColor);
    glUniform1f(locShine, obj.shininess);
    glUniform1f(locMaterial, obj.material);
    glUniform1i(locTexture, 0);
    glUniform1i(locSign, 1);	
    glUniform1f(locNoShine, obj.noShine);

    modelViewMatrix.Scale(obj.scale[0],obj.scale[1],obj.scale[2]);

    modelViewMatrix.Translate(obj.translation1[0],obj.translation1[1],obj.translation1[2]);	


    modelViewMatrix.Rotate(obj.rotation_y[0],obj.rotation_y[1],obj.rotation_y[2],obj.rotation_y[3]);		
    modelViewMatrix.Translate(obj.translation2[0],obj.translation2[1],obj.translation2[2]);


    modelViewMatrix.Rotate(obj.rotation_z[0],obj.rotation_z[1],obj.rotation_z[2],obj.rotation_z[3]);
    modelViewMatrix.Translate(obj.translation3[0],obj.translation3[1],obj.translation3[2]);

    modelViewMatrix.Rotate(obj.rotation_x[0],obj.rotation_x[1],obj.rotation_x[2],obj.rotation_x[3]);	
    modelViewMatrix.Translate(obj.translation4[0],obj.translation4[1],obj.translation4[2]);

    updateMatrices();
    modelViewMatrix.PopMatrix();
}

void drawPoint(float x, float y,float z)
{

    glPointSize(10);
    glBegin(GL_POINTS);
    glVertex3f(x, y,z);
    glEnd();
    glFlush(); 
}

void render()
{

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


    // This puts the view matrix (camera matrix) into the stack
    M3DMatrix44f  mViewMatrix;     // the view matrix
    worldFrame.GetCameraMatrix(mViewMatrix);    
    modelViewMatrix.PushMatrix();
    modelViewMatrix.MultMatrix(mViewMatrix);

    m3dTransformVector3(viewSpaceLightPos, lightPosition, mViewMatrix);

    glUseProgram(shader);


    // the light
    updateMatrices();
    drawPoint(lightPosition[0],lightPosition[1],lightPosition[2]);


    object floor;
    floor.scale[0]=2;
    floor.rotation_x[0]=-90;
    //floor.scale[1]=1;
    floor.scale[2]=1.1;

    floor.material=0.8; 

    floor.noShine=1;
    setUniforms(floor);
    bindDraw(&floor_batch,Tex_bark);

    //end of ground


    //the sofa
    object sofa;

    sofa.rotation_x[0]=180;	
    sofa.translation1[1]=120;


    sofa.noShine=1;
    setUniforms(sofa);
    bindDraw(&sofa_batch,Tex_sofa);

    //end of sofa

    //the cushion
    object cushion;


    cushion.rotation_x[0]=180;	
    cushion.translation1[1]=120;

    cushion.noShine=1;
    setUniforms(cushion);
    bindDraw(&cushion_batch,Tex_cush);

    //end of cushion

    //the dining table
    object dine;
    dine.translation1[0]=600;
    dine.translation1[1]=-70;
    dine.translation1[2]=1000;


    dine.rotation_x[0]=-90;
    dine.rotation_y[0]=-90;

    //dine.translation1[0]=20;

    dine.noShine=1;
    setUniforms(dine);
    bindDraw(&dine_batch,Tex_dine);

    //end of dining table


    //the tea
    object teaset;

    teaset.translation1[0]=600;
    teaset.translation1[1]=170;
    teaset.translation1[2]=1000;

    teaset.rotation_x[0]=-180;
    teaset.rotation_y[0]=90;

    teaset.noShine=0;
    setUniforms(teaset);
    bindDraw(&tea_batch,Tex_china);

    //end of tea

    //the plant
    object plant;

    plant.translation1[0]=-400;

    plant.rotation_x[0]=-90;

    plant.noShine=1;
    setUniforms(plant);
    bindDraw(&plant_batch,Tex_plant);

    //end of plant

    //the urn
    object urn;

    urn.translation1[0]=740;
    urn.translation1[2]=-400;

    urn.rotation_x[0]=-90;

    urn.noShine=0;
    setUniforms(urn);
    bindDraw(&urn_batch,Tex_mud);

    //end of urn

    //the herons
    object heron;

    heron.translation1[0]=400;

    heron.rotation_x[0]=-90;
    heron.rotation_y[0]=-90;

    heron.noShine=0;	
    heron.scale[0]=0.9;
    heron.scale[1]=0.9;
    heron.scale[2]=0.9;
    setUniforms(heron);

    bindDraw(&heron_batch,Tex_heron);


    heron.scale[0]=1.1;
    heron.scale[1]=1.1;
    heron.scale[2]=1.1;

    heron.translation1[0]=410;
    setUniforms(heron);
    bindDraw(&heron_batch,Tex_heron);


    //end of herons

    //the chair
    object chair;

    chair.scale[0]=3;
    chair.scale[1]=3;
    chair.scale[2]=3;

    chair.translation1[0]=-150;
    chair.translation1[2]=200;
    chair.rotation_x[0]=-90;
    chair.rotation_y[0]=150;

    chair.noShine=0;	
    setUniforms(chair);

    bindDraw(&chair_batch,Tex_black);

    chair.translation1[2]=300;
    setUniforms(chair);	
    bindDraw(&chair_batch,Tex_black);

    object seat;

    seat.scale[0]=3;
    seat.scale[1]=3;
    seat.scale[2]=3;

    seat.translation1[0]=-150;
    seat.translation1[2]=200;

    seat.rotation_x[0]=-90;
    seat.rotation_y[0]=150;

    seat.noShine=0;	
    setUniforms(seat);

    bindDraw(&seat_batch,Tex_cush);

    seat.translation1[2]=300;
    setUniforms(seat);	
    bindDraw(&seat_batch,Tex_cush);

    //end of chair


    //the wall
    object wall;

    wall.scale[0]=2;

    wall.noShine=1;
    setUniforms(wall);	
    bindDraw(&wall_batch,Tex_wall);	

    //end of wall

    //the candlestand
    object stand;

    stand.rotation_x[0]=-90;
    stand.rotation_y[0]=-30;
    stand.translation1[2]=-750;	
    stand.translation1[1]=500;	
    stand.translation1[0]=-570;	
    setUniforms(stand);	
    bindDraw(&candle_stand_batch,Tex_black);	

    //end of candlestand

    //end of wall

    //the candles
    object candle;
    candle.rotation_x[0]=-90;
    candle.rotation_y[0]=-30;
    candle.translation1[2]=-750;	
    candle.translation1[1]=500;	
    candle.translation1[0]=-570;
    setUniforms(candle);	
    bindDraw(&candle_batch,Tex_wall);	
    //end of candles


    //the frame
    object frame;

    frame.rotation_x[0]=-90;
    frame.translation1[2]=-750;	
    frame.translation1[1]=500;	

    setUniforms(frame);	
    bindDraw(&frame_batch,Tex_frame);	


    frame.rotation_y[0]=-90;
    frame.scale[2]=1.2;


    frame.translation1[2]=210;	
    frame.translation1[1]=500;	
    frame.translation1[0]=940;	

    setUniforms(frame);	
    bindDraw(&frame_batch,Tex_frame);	
    //end of frame

    //the pic
    object pic;

    pic.rotation_x[0]=-90;
    pic.translation1[2]=-750;	
    pic.translation1[1]=500;	

    setUniforms(pic);	
    bindDraw(&photo_batch,Tex_photo);	

    pic.rotation_y[0]=-90;
    pic.scale[2]=1.2;


    pic.translation1[2]=210;	
    pic.translation1[1]=500;	
    pic.translation1[0]=940;	

    setUniforms(pic);	
    bindDraw(&photo_batch,Tex_painting);
    //end of pic 


    //the legs
    object legs;

    legs.translation1[0]=-460;
    legs.translation1[2]=1300;
    legs.rotation_x[0]=-90;


    setUniforms(legs);
    bindDraw(&legs_batch,Tex_frame);

    //end of legs

    //the top
    object top;

    top.translation1[0]=-460;
    top.translation1[2]=1300;
    top.rotation_x[0]=-90;

    setUniforms(top);
    bindDraw(&top_batch,Tex_marble);

    //end of top


    //the lamp
    object lamp;


    lamp.translation1[0]=-460;
    lamp.translation1[1]=120;
    lamp.translation1[2]=1300;
    lamp.rotation_x[0]=-90;

    setUniforms(lamp);
    bindDraw(&lamp_batch,Tex_sofa);

    //end of lamp

    //the shade
    object shade;


    shade.translation1[0]=-460;
    shade.translation1[1]=40;
    shade.translation1[2]=1300;
    shade.rotation_x[0]=-90;

    shade.noShine=0;

    setUniforms(shade);
    glUniform1i(locSign,-1);	
    bindDraw(&shade_batch,Tex_lamp);

    //end of shade	

    // pops out the view matrix
    modelViewMatrix.PopMatrix();

    glutSwapBuffers();
    glutPostRedisplay();
}

int time=0;
float rotn=0.01,forw=0;
void keys(int key,int x,int y)
{

    worldFrame.RotateWorld(+0.3,1,0,0);	
    if(key==GLUT_KEY_UP)
    {
        worldFrame.MoveForward(forw);		
        forw=forw+0.8;

    }


    if(key==GLUT_KEY_DOWN)
    {
        worldFrame.MoveForward(-forw);
        forw=forw+0.8;

    }
    worldFrame.RotateWorld(-0.3,1,0,0);	


    if(key==GLUT_KEY_RIGHT)
    {
        {
            worldFrame.RotateWorld(-rotn,0,1,0);		
        }
    }
    if(key==GLUT_KEY_LEFT)
    {
        worldFrame.RotateWorld(rotn,0,1,0);		
    }

}



void keyPressed (unsigned char key, int x, int y)
{
    int li=15;
    if(key=='w')
    {
        lightPosition[1]=lightPosition[1]+li;
    }

    if(key=='s')
    {
        lightPosition[1]=lightPosition[1]-li;
    }

    if(key=='a')
    {
        lightPosition[0]=lightPosition[0]-li;
    }
    if(key=='d')
    {

        lightPosition[0]=lightPosition[0]+li;
    }

    if(key=='r')
    {
        lightPosition[2]=lightPosition[2]-li;
    }

    if(key=='f')
    {
        lightPosition[2]=lightPosition[2]+li;
    }
    //cout<<lightPosition[0]<<"  "<<lightPosition[1]<<" "<<lightPosition[2];

}

void keyReleased(unsigned char key, int x, int y)
{


}

void main(int argc,char **argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(len,wide);
    glutInitWindowPosition(0,0);
    glutCreateWindow("table");
    glutDisplayFunc(render);
    glutSpecialFunc(keys);
    glutKeyboardFunc(keyPressed);
    glutKeyboardUpFunc(keyReleased);


    GLenum err=glewInit();
    if(GLEW_OK!=  err)
    {
        fprintf(stderr,"glew errot: %s\n",glewGetErrorString(err));		

    }

    setup();
    glutMainLoop();

}
