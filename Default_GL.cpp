#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include "CommonInclude.h" 
#include "Time.h"
#include "Texture.h"
#include "Ground.h"
#include "FBXModel.h"
#include "ShaderManager.h"
#include "Camera.h"
#include "Helicopter.h"

// 함수 선언
void InitBuffers();
GLvoid DrawScene();
GLvoid Reshape(int w, int h);
void SpecialKeyboard(int key, int x, int y);
void Mouse(int button, int state, int x, int y);
int main(int argc, char** argv);
void WhellFunc(int whell, int dir, int x, int y);
void Motion(int x, int y);
void Timer(int value);

// 전역 변수
GLint width = 1280, height = 720;
GLuint shaderProgramID;

// 모델별 VAO/VBO/EBO 
GLuint VAO_Body, VBO_Body, EBO_Body;
GLuint VAO_Blade, VBO_Blade, EBO_Blade;
GLuint VAO_Tail, VBO_Tail, EBO_Tail;

//스카이박스 관련
GLuint skyboxVAO, skyboxVBO;
GLuint skyboxShaderProgramID;
GLuint cubemapTexture;

// 객체
Camera* camera = nullptr;
Helicopter* helicopter = nullptr;
Ground* mGround = nullptr;

// 마우스 입력
bool rightClickDown = false; 
int lastMouseX = 0;         
int lastMouseY = 0;

//imgui 관련 변수
float modelScale = 0.1f;
int targetFrameDelay = 1;
bool wireframeMode = false;
float glassAlpha = 0.5f;

//디버그 회전 
float xModelRotation = 0.0f;
float yModelRotation = 0.0f;
float zModelRotation = 0.0f;
float currentScale = 1.0f;
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("HeliProj");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW 초기화 실패" << std::endl;
        return -1;
    }

    Input::Initialize();
    
    // ImGui 초기화
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL3_Init("#version 330");

    // 셰이더 생성
    shaderProgramID = ShaderManager::CreateShaderProgram("vertex3D.glsl", "fragment.glsl");
    if (shaderProgramID == 0) {
        std::cerr << "셰이더 프로그램 생성 실패" << std::endl;
        return -1;
    }

    skyboxShaderProgramID = ShaderManager::CreateSkyboxShaderProgram();
    if (skyboxShaderProgramID == 0) {
        std::cerr << "스카이박스 셰이더 프로그램 생성 실패" << std::endl;
        return -1;
    }

    InitBuffers();
    
    // 카메라 초기화
    camera = new Camera();
    camera->Initialize(glm::vec3(75.0f, 75.0f, 75.0f), 150.0f, 50.0f);
    
    // 헬리콥터 초기화
    helicopter = new Helicopter();
    helicopter->Initialize();
    helicopter->LoadModels(VAO_Body, VBO_Body, EBO_Body,
                          VAO_Blade, VBO_Blade, EBO_Blade,
                          VAO_Tail, VBO_Tail, EBO_Tail);
    
    // 땅 초기화
    mGround = new Ground();
    mGround->Initialize();

    // 스카이박스 큐브맵 로드
    std::vector<std::string> faces = {
          "SkyBox-Texture/px.png",
          "SkyBox-Texture/nx.png",
          "SkyBox-Texture/py.png",
          "SkyBox-Texture/ny.png",
          "SkyBox-Texture/pz.png",
          "SkyBox-Texture/nz.png"
    };
    cubemapTexture = ShaderManager::LoadCubemap(faces);
    if (cubemapTexture == 0) {
        std::cerr << "스카이박스 큐브맵 로드 실패" << std::endl;
    }
    
    Time::Initialize();

    glutDisplayFunc(DrawScene);
    glutReshapeFunc(Reshape);
    glutTimerFunc(targetFrameDelay, Timer, 0);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutSpecialFunc(SpecialKeyboard);
    glutMouseWheelFunc(WhellFunc);

    glutMainLoop();

    delete camera;
    delete helicopter;
    delete mGround;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
    return 0;
}

void WhellFunc(int whell, int dir, int x, int y)
{
    if (camera) {
        camera->Zoom(dir);
    }
}

void DrawScene()
{
    Time::Update();
    
    // 헬리콥터 업데이트
    if (helicopter) {
        helicopter->Update(Time::DeltaTime());
    }
    
    // 카메라 업데이트
    if (camera && helicopter) {
        camera->Update(Time::DeltaTime(), 
                      helicopter->GetPosition(), 
                      helicopter->GetUp(), 
                      helicopter->GetForward());
    }

    // 씬 클리어
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);

    glUseProgram(shaderProgramID);

    // Uniform 설정
    GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLint projLoc = glGetUniformLocation(shaderProgramID, "proj");
    GLint useTextureLoc = glGetUniformLocation(shaderProgramID, "useTexture");

    // 조명 유니폼
    GLint eyePosLoc = glGetUniformLocation(shaderProgramID, "eyePos");
    GLint lightDirLoc = glGetUniformLocation(shaderProgramID, "lightDir");
    GLint lightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");
    GLint ambientStrengthLoc = glGetUniformLocation(shaderProgramID, "ambientStrength");
    GLint specularStrengthLoc = glGetUniformLocation(shaderProgramID, "specularStrength");
    GLint shininessLoc = glGetUniformLocation(shaderProgramID, "shininess");

    // View/Projection 행렬
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 proj = camera->GetProjectionMatrix((float)width / (float)height);

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

    // 조명 설정
    glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f));
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    glUniform3fv(eyePosLoc, 1, glm::value_ptr(camera->GetPosition()));
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    glUniform1f(ambientStrengthLoc, 0.5f);
    glUniform1f(specularStrengthLoc, 0.5f);
    glUniform1f(shininessLoc, 32.0f);
    glUniform1f(useTextureLoc, 1.0f);

    // 헬리콥터 렌더링
    if (helicopter) {
      // 텍스처 사용 설정
        glUniform1f(useTextureLoc, 1.0f);
  
        helicopter->Render(shaderProgramID, wireframeMode, glassAlpha, modelScale);
        
 // 미사일 렌더링 (별도 상태로 처리)
        helicopter->RenderMissiles(shaderProgramID, view, proj);
        
        // 텍스처 사용 상태 복원
  glUniform1f(useTextureLoc, 1.0f);
    }

    // 땅 렌더링
    if (mGround)
    {
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        
        // 텍스처 사용 명시적으로 설정
        glUniform1f(useTextureLoc, 1.0f);
     
        mGround->Render(shaderProgramID, view, proj);
        glEnable(GL_BLEND);
    }

    // 스카이박스 그리기
    glDepthFunc(GL_LEQUAL);
    glUseProgram(skyboxShaderProgramID);

    GLint skyViewLoc = glGetUniformLocation(skyboxShaderProgramID, "view");
    GLint skyProjLoc = glGetUniformLocation(skyboxShaderProgramID, "projection");
    glUniformMatrix4fv(skyViewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(skyProjLoc, 1, GL_FALSE, glm::value_ptr(proj));

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glUniform1i(glGetUniformLocation(skyboxShaderProgramID, "skybox"), 0);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

    // ImGui 렌더링 
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(
        ImVec2((float)width, 0.0f),
        ImGuiCond_FirstUseEver,
        ImVec2(1.0f, 0.0f)
    );
    ImGui::SetNextWindowSize(ImVec2(250, 150), ImGuiCond_FirstUseEver);

    ImGui::Begin("Debug Controls");

    float fps = 1.0f / Time::DeltaTime();
    ImGui::Text("FPS: %.1f", fps);

    // Ground HeightMap Control
    if (mGround) {
        ImGui::Separator();
        ImGui::Text("Terrain Controls");
        
        if (ImGui::SliderFloat("Height Scale", &currentScale, 0.0f, 1000.0f)) {
			mGround->ControlHeightmap(currentScale);
        }
        ImGui::Separator();
    }

    if (helicopter) {
        ImGui::Separator();
        glm::vec3 pos = helicopter->GetPosition();
        glm::vec3 vel = helicopter->GetVelocity();
        ImGui::Text("Position: (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
        ImGui::Text("Velocity: (%.1f, %.1f, %.1f)", vel.x, vel.y, vel.z);
        ImGui::Text("Speed: %.1f", glm::length(vel));
        ImGui::Separator();
        ImGui::Text("Pitch: %.1f", helicopter->GetPitch());
        ImGui::Text("Roll: %.1f", helicopter->GetRoll());
        
        ImGui::Separator();

        ImGui::SliderFloat("Model RotationX", &xModelRotation, -180.0f, 180.0f);
        ImGui::SliderFloat("Model RotationY", &yModelRotation, -180.0f, 180.0f);
        ImGui::SliderFloat("Model RotationZ", &zModelRotation, -180.0f, 180.0f);
        helicopter->SetDebugRotation(xModelRotation, yModelRotation, zModelRotation);
        ImGui::Separator();

        ImGui::SliderFloat("Max Speed", &helicopter->GetMaxSpeed(), 10.0f, 200.0f);
        ImGui::SliderFloat("Acceleration", &helicopter->GetAccelerationRate(), 10.0f, 100.0f);
        ImGui::SliderFloat("Max Tilt", &helicopter->GetMaxTiltAngle(), 10.0f, 60.0f);
        ImGui::Separator();
    }

    if (ImGui::Button("wired frame"))
    {
        wireframeMode = !wireframeMode;
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glutSwapBuffers();
}

void Reshape(int w, int h) {
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    ImGui_ImplGLUT_ReshapeFunc(w, h);
}

void SpecialKeyboard(int key, int x, int y)
{
    if (!helicopter) return;
    
    glm::vec3 pos = helicopter->GetPosition();
    switch (key)
    {
    case GLUT_KEY_RIGHT:
        pos.x += 10.0f;
        break;
    case GLUT_KEY_LEFT:
        pos.x -= 10.0f;
        break;
    case GLUT_KEY_UP:
        pos.z += 1.0f;
        break;
    case GLUT_KEY_DOWN:
        pos.z -= 1.0f;
        break;
    }
}

void Timer(int value) {
    Input::Update();
    
    if (Input::GetKey(eKeyCode::ESC))
    {
        exit(0);
    }

    glutPostRedisplay();
    glutTimerFunc(targetFrameDelay, Timer, 0);
}

void Mouse(int button, int state, int x, int y) {
    ImGui_ImplGLUT_MouseFunc(button, state, x, y);

    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse)
    {
        if (button == GLUT_LEFT_BUTTON) {
            if (state == GLUT_DOWN) {
                rightClickDown = true;
                lastMouseX = x;
                lastMouseY = y;
            }
            else if (state == GLUT_UP) {
                rightClickDown = false;
            }
        }
        else if (button == GLUT_RIGHT_BUTTON) {
            if (state == GLUT_DOWN) {
                lastMouseX = x;
                lastMouseY = y;
            }
        }
    }
}

void Motion(int x, int y) {
    ImGui_ImplGLUT_MotionFunc(x, y);
    
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse && camera && helicopter)
    {
        if (rightClickDown) {
            int deltaX = x - lastMouseX;
            int deltaY = y - lastMouseY;

            float yaw = helicopter->GetYaw();
            camera->ProcessMouseDrag(deltaX, deltaY, yaw);
            helicopter->SetYaw(yaw);

            lastMouseX = x;
            lastMouseY = y;

            glutPostRedisplay();
        }
    }
}

void InitBuffers() {
    // 스카이박스 버텍스
    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,   1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    const GLsizei stride = 11 * sizeof(GLfloat);
    
    // Body 버퍼
    glGenVertexArrays(1, &VAO_Body);
    glBindVertexArray(VAO_Body);

    glGenBuffers(1, &VBO_Body);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Body);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO_Body);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Body);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    // Blade 버퍼
    glGenVertexArrays(1, &VAO_Blade);
    glBindVertexArray(VAO_Blade);

    glGenBuffers(1, &VBO_Blade);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Blade);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO_Blade);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Blade);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    // Tail 버퍼
    glGenVertexArrays(1, &VAO_Tail);
    glBindVertexArray(VAO_Tail);

    glGenBuffers(1, &VBO_Tail);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Tail);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO_Tail);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Tail);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}