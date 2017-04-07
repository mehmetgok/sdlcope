#include <GLES2/gl2.h>
#include <SDL2/SDL.h>

/*

 g++ -std=c++0x -Wall -pedantic sdlcope.cpp -o sdlcope `sdl2-config --cflags --libs` -lSDL2_image -L /opt/vc/lib -lEGL -lGLESv2 -I /opt/vc/include/
 
*/

// glOrtho
// http://stackoverflow.com/questions/29322913/opengl-es-2-0-ortho-projection-blank-screen

int _position;
int _color;
int _proj;


GLfloat *signal_vertices;
GLfloat *signal_colors;

GLfloat *fft_vertices;
GLfloat *fft_colors;
	
	
// For mesh drawing
GLfloat *mesh_vertices;
GLfloat *mesh_colors;

struct myData {
    SDL_Window *window;
    SDL_GLContext context;
};

#if defined(WIN32) && !defined(UNIX)
/* Do windows stuff */

const char *shader_vertex_source="\n\
        attribute vec2 position; //the position of the point\n\
        attribute vec3 color;  //the color of the point\n\
        \n\
        varying vec3 vColor;\n\
        uniform mat4 projection;\n\
        void main(void) { //pre-built function\n\
        gl_Position =   projection*vec4(position, 0., 1.0); //0. is the z, and 2 is w\n\
        vColor=color;\n\
        }";


const char *shader_fragment_source="\n\
       mediump float;\n\
        \n\
        \n\
        \n\
        varying vec3 vColor;\n\
        void main(void) {\n\
        gl_FragColor = vec4(vColor, .1);\n\
        }";
#else

// Burası Ubuntu üzerinde beglabone black için derleme yapmak üzere ayrıldı. SGX ile NVIDA GPU direktifleri uyusmuyor
const char *shader_vertex_source="\n\
        precision mediump float;\n\
        attribute vec3 color;  //the color of the point\n\
        attribute vec2 position; //the position of the point\n\
        uniform mat4 projection;\n\
        \n\
        varying vec3 vColor;\n\
        void main(void) { //pre-built function\n\
            gl_Position = projection*vec4(position, 0., 1.); //0. is the z, and 1 is w\n\
             vColor=color;\n\
        }";


const char *shader_fragment_source="\n\
       precision mediump float;\n\
        \n\
        \n\
        \n\
         varying vec3 vColor;\n\
        void main(void) {\n\
        gl_FragColor = vec4(vColor, .3);\n\
        }";

#endif

GLfloat resultM[16];



void ortho(float left, float right, float top, float bottom, float near, float far)
{

	resultM[0] = 2.0 / (right - left);
	resultM[1] = 0.0;
	resultM[2] = 0.0;
	resultM[3] = 0.0;
	resultM[4] = 0.0;
	resultM[5] = 2.0 / (top - bottom);
	resultM[6] = 0.0;
	resultM[7] = 0.0;
	resultM[8] = 0.0;
	resultM[9] = 0.0;
	resultM[10] = (1.0f / (near - far));
	resultM[11] = 0.0;
	resultM[12] = ((left + right) / (left - right));
	resultM[13] = ((top + bottom) / (bottom - top));
	resultM[14] = (near / (near - far));
	resultM[15] = 1;
	
	
}




GLuint loadShader(GLuint program, GLenum type, const GLchar *shaderSrc) {
    GLuint shader;
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);
    glAttachShader(program, shader);
    return 0;
}

int sdlInit(myData *data) {
    SDL_Init(SDL_INIT_VIDEO);
	
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    data->window = SDL_CreateWindow("Demo", 0, 0, 1024, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    data->context = SDL_GL_CreateContext(data->window);

    return 0;
}

int glInit(myData *data) {
    
	GLuint programObject;
    
	programObject = glCreateProgram();
    
	loadShader(programObject, GL_VERTEX_SHADER, shader_vertex_source);
    loadShader(programObject, GL_FRAGMENT_SHADER, shader_fragment_source);
    
	glLinkProgram(programObject);
    glUseProgram(programObject);
	
	_position = glGetAttribLocation(programObject, "position");
    _color = glGetAttribLocation(programObject, "color");
	//_proj = glGetAttribLocation(programObject, "projection");
	
	 _proj = glGetUniformLocation(programObject, "projection");
	
	
	
	
	
	ortho(-50, 8050, -11, 11, -1, 1);
	
	glUniformMatrix4fv(_proj, 1, false, (GLfloat*)resultM);
		
	
	glDisable(GL_DEPTH_TEST);
	
	

    // glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
    glViewport(0, 0, 1024, 600);

    return 0;
}

int loopFunc(myData *data) {
    SDL_Event event;
	
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return 1;
        }
    }

    glClear(GL_COLOR_BUFFER_BIT);
    
	
	 // Load the vertex data
     glVertexAttribPointer(_position, 3, GL_FLOAT, GL_FALSE, 0, mesh_vertices);
     glEnableVertexAttribArray(_position);

     glVertexAttribPointer(_color, 3, GL_FLOAT, GL_FALSE, 0, mesh_colors);
     glEnableVertexAttribArray(_color);


     glDrawArrays(GL_LINES, 0, 64);

    glDisableVertexAttribArray(_position);

     glDisableVertexAttribArray(_color);
	 
	 
	     // Load the vertex data
              glVertexAttribPointer(_position, 3, GL_FLOAT, GL_FALSE, 0, signal_vertices);
              glEnableVertexAttribArray(_position);

             glVertexAttribPointer(_color, 3, GL_FLOAT, GL_FALSE, 0, signal_colors);
             glEnableVertexAttribArray(_color);


             glDrawArrays(GL_LINE_STRIP, 0, 8000);


             glDisableVertexAttribArray(_position);

             glDisableVertexAttribArray(_color);


           
	
	
	
	
	
    SDL_GL_SwapWindow(data->window);

    return 0;
}

void sdlQuit(myData *data) {
    SDL_GL_DeleteContext(data->context);
    SDL_DestroyWindow(data->window);
    SDL_Quit();
    return;
}

void genMesh()
{

 /*
     * Design scope mesh
    */

    int ind = 0;
    int starty = -10;
    int startx = 0;



    mesh_vertices = new GLfloat[64*3];
    mesh_colors = new GLfloat[64*3];

     for (int i=0;i<11;i++)
    {
        mesh_vertices[ind++] = 0.0f;
        mesh_vertices[ind++] = starty;
        mesh_vertices[ind++] = 0.0f;

        mesh_vertices[ind++] = 8000.0f;
        mesh_vertices[ind++] = starty;
        mesh_vertices[ind++] = 0.0f;

        starty+=2;
    }

    for (int i=0;i<21;i++)
    {
        mesh_vertices[ind++] = startx;
        mesh_vertices[ind++] = 10.0;
        mesh_vertices[ind++] = 0.0f;

        mesh_vertices[ind++] = startx;
        mesh_vertices[ind++] = -10;
        mesh_vertices[ind++] = 0.0f;

        startx+=1000;

    }


     for (int i=0;i<64;i++)
     {
         mesh_colors[i*3+0] = 0.1;
         mesh_colors[i*3+1] = 0.2;
         mesh_colors[i*3+2] = 0.05;

     }



     signal_vertices = new GLfloat[3*8000];
     signal_colors = new GLfloat[3*8000];

     ind = 0;


     for (int i=0;i<8000;i++)
     {
         signal_vertices[ind++] = i/1.0;
         signal_vertices[ind++] = 5*sin(i*3.14/180.0);
         signal_vertices[ind++] = 0.0;

     }


     /*
      *    x, y, z
     */


/*
     ind = 0;
     for (int i=0;i<20000;i++)
     {
         signal_vertices[ind++] = i/1.0;
         signal_vertices[ind++] = 8.0*sin(signal_vertices[ind-1]*2.0*3.14/180.0/30);
         signal_vertices[ind++] = 0.0f;

     }
*/

     /*
      * signal color is red
    */

     ind = 0;


     for (int i=0;i<8000;i++)
     {
         signal_colors[ind++] = 0.8;
         signal_colors[ind++] = 0.05;
         signal_colors[ind++] = 0.05;

     }



     fft_vertices = new GLfloat[3*8000];
     fft_colors = new GLfloat[3*8000];

     ind = 0;


     for (int i=0;i<8000;i++)
     {
         fft_vertices[ind++] = i/1.0;
         fft_vertices[ind++] = i/1.0;
         fft_vertices[ind++] = 0.0;

     }


     ind = 0;


     for (int i=0;i<8000;i++)
     {
         fft_colors[ind++] = 0.05;
         fft_colors[ind++] = 0.8;
         fft_colors[ind++] = 0.05;

     }


}

void clearMem()
{
  
  
  delete [] signal_vertices;
  delete [] signal_colors;
  
  delete [] mesh_vertices;
  delete [] mesh_colors;
  
  
  delete [] fft_vertices;
  delete [] fft_colors;

}

int main() {
    myData data;
	
	genMesh();

    sdlInit(&data);

    glInit(&data);

    while (!loopFunc(&data));

    sdlQuit(&data);
	
	clearMem();

    return 0;
}
