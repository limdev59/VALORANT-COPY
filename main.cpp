#include "pch.h"
#include "CCore.h"
#include "MouseMgr.h"
#include "Camera.h"

#include "Model.h"


static GLvoid Reshape(int w, int h) {
    MouseMgr::Instance()->SetWindowSize(w, h);
    CCore::Instance()->width = w; 
    CCore::Instance()->height = (h == 0) ? 1 : h;
    glViewport(0, 0, w, (h == 0) ? 1 : h);
    Camera::Instance()->SetAspectRatio((float)CCore::Instance()->width, (float)CCore::Instance()->height);
}
static GLvoid Render() {
    CCore::Instance()->Render();
}
static GLvoid Update() {
    CCore::Instance()->Update();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(300, 100);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("OpenGL Application");
    glewExperimental = GL_TRUE;
    glewInit();

    CCore::Instance()->Init();

    
    static GLuint vertexShader, fragmentShader;
    make_shaderProgram(
        CCore::Instance()->shaderProgramID, 
        vertexShader, 
        fragmentShader
    );

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glutDisplayFunc(Render);
    glutIdleFunc(Update);
    glutReshapeFunc(Reshape);

    glutMainLoop();
}