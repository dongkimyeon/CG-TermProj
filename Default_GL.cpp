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
#include "AA.h"
#include <random>

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
void InitializeAAUnits();

// 전역 변수
GLint width = 1280, height = 720;
GLuint shaderProgramID;

//스카이박스 관련
GLuint skyboxVAO, skyboxVBO;
GLuint skyboxShaderProgramID;
GLuint cubemapTexture;

//크로스헤어 관련
GLuint crosshairVAO, crosshairVBO;
GLuint crosshairShaderProgramID;

// 후처리 관련
GLuint postprocessShaderID;
GLuint framebuffer;
GLuint textureColorbuffer;
GLuint rbo;
GLuint quadVAO, quadVBO;
bool enableNightVision = false;

// 객체
Camera* camera = nullptr;
Helicopter* helicopter = nullptr;
AA** aaUnits = nullptr;
const int NUM_AA_UNITS = 10;

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
float crosshairSize = 1.0f;
float crosshairDistance = 50.0f;
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

    // 후처리 셰이더 생성
    postprocessShaderID = ShaderManager::CreateShaderProgram("postprocess_vertex.glsl", "postprocess_fragment.glsl");
    if (postprocessShaderID == 0) {
        std::cerr << "후처리 셰이더 프로그램 생성 실패" << std::endl;
        return -1;
    }

    crosshairShaderProgramID = ShaderManager::CreateShaderProgram("crosshairVertex.glsl", "crosshairFrag.glsl");
    if (crosshairShaderProgramID == 0) {
        std::cerr << "크로스헤어 셰이더 프로그램 생성 실패" << std::endl;
        return -1;
    }

    InitBuffers();
    
    // 카메라 초기화
    camera = new Camera();
    camera->Initialize(glm::vec3(75.0f, 75.0f, 75.0f), 150.0f, 50.0f);
    
    // 헬리콥터 초기화
    helicopter = new Helicopter();
    helicopter->Initialize();
    helicopter->LoadModels();

    // 땅 초기화
    mGround = new Ground();
    mGround->Initialize();

    // AA 유닛 초기화 (10대)
    InitializeAAUnits();

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

    // 정리
    delete camera;
    delete helicopter;
    delete mGround;
    
    if (aaUnits) {
        for (int i = 0; i < NUM_AA_UNITS; ++i) {
            delete aaUnits[i];
        }
        delete[] aaUnits;
        
        // 공유 모델 정리
        AA::CleanupSharedModel();
    }
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
    return 0;
}

void WhellFunc(int wheel, int dir, int x, int y)
{
    if (camera) {
        camera->Zoom(dir);
        glutPostRedisplay();  // 화면 갱신
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
            helicopter->GetForward(),
            helicopter->GetPitch(),    // 피치 추가
            helicopter->GetRoll(),     // 롤 추가
            helicopter->GetCannonWorldPosition());  // 기관포 월드 위치 추가
    }

    // ===== 1단계: 프레임버퍼에 씬 렌더링 =====
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
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
        glUniform1f(useTextureLoc, 1.0f);
  
        helicopter->Render(shaderProgramID, wireframeMode, glassAlpha, modelScale);
        

        helicopter->RenderMissiles(shaderProgramID, view, proj);
        
      
        glUniform1f(useTextureLoc, 1.0f);
    }

    // AA 유닛 렌더링 (10대 모두)
    if (aaUnits) {
        glUniform1f(useTextureLoc, 1.0f);
        
        for (int i = 0; i < NUM_AA_UNITS; ++i) {
            if (aaUnits[i]) {
                aaUnits[i]->Render(shaderProgramID, wireframeMode, glassAlpha, modelScale);
            }
        }
        
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

    // ===== 크로스헤어 렌더링 (3인칭 뷰일 때만, 프레임버퍼 내부에서) =====
    if (camera && (camera->GetCameraMode() == 0 || camera->GetCameraMode() == 1) && helicopter) {
        // 깊이 테스트 끄기
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // 기본 3D 셰이더 사용 (vertex3D.glsl + fragment.glsl)
        glUseProgram(shaderProgramID);
        
        // 헬리콥터의 현재 상태 가져오기
        glm::vec3 heliPos = helicopter->GetPosition();
        float yaw = helicopter->GetYaw();
        float pitch = helicopter->GetPitch();
        float roll = helicopter->GetRoll();
        
        // 헬리콥터 변환 매트릭스 직접 생성
        glm::mat4 heliTransform = glm::mat4(1.0f);
        heliTransform = glm::translate(heliTransform, heliPos);
        heliTransform = glm::rotate(heliTransform, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        heliTransform = glm::rotate(heliTransform, glm::radians(roll), glm::vec3(1.0f, 0.0f, 0.0f));
        heliTransform = glm::rotate(heliTransform, glm::radians(pitch), glm::vec3(0.0f, 0.0f, 1.0f));
        
        // 헬리콥터의 로컬 전방 방향 벡터 (-1, 0, 0)를 월드 공간으로 변환
        glm::vec3 localForward = glm::vec3(-1.0f, 0.0f, 0.0f);
        glm::vec4 worldForward = heliTransform * glm::vec4(localForward, 0.0f);
        glm::vec3 actualForward = glm::normalize(glm::vec3(worldForward));
        
        // 로컬 상하좌우 방향 벡터도 계산
        glm::vec3 localUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 localRight = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 actualUp = glm::normalize(glm::vec3(heliTransform * glm::vec4(localUp, 0.0f)));
        glm::vec3 actualRight = glm::normalize(glm::vec3(heliTransform * glm::vec4(localRight, 0.0f)));
        
        // 크로스헤어 중심점 (헬기 전방 50미터)
        glm::vec3 crosshairCenter = heliPos + actualForward * -crosshairDistance;
        
    
        // 십자 모양 선 정의 (4개의 선)
        std::vector<glm::vec3> crosshairLines;
        
        // 수평선 (좌 -> 중심)
        crosshairLines.push_back(crosshairCenter - actualRight * crosshairSize * 0.5f);
        crosshairLines.push_back(crosshairCenter - actualRight * crosshairSize * 0.15f);
        
        // 수평선 (중심 -> 우)
        crosshairLines.push_back(crosshairCenter + actualRight * crosshairSize * 0.15f);
        crosshairLines.push_back(crosshairCenter + actualRight * crosshairSize * 0.5f);
        
        // 수직선 (하 -> 중심)
        crosshairLines.push_back(crosshairCenter - actualUp * crosshairSize * 0.5f);
        crosshairLines.push_back(crosshairCenter - actualUp * crosshairSize * 0.15f);
        
        // 수직선 (중심 -> 상)
        crosshairLines.push_back(crosshairCenter + actualUp * crosshairSize * 0.15f);
        crosshairLines.push_back(crosshairCenter + actualUp * crosshairSize * 0.5f);
        
        // 임시 VAO/VBO 생성하여 선 그리기
        GLuint tempVAO, tempVBO;
        glGenVertexArrays(1, &tempVAO);
        glGenBuffers(1, &tempVBO);
        
        glBindVertexArray(tempVAO);
        glBindBuffer(GL_ARRAY_BUFFER, tempVBO);
        glBufferData(GL_ARRAY_BUFFER, crosshairLines.size() * sizeof(glm::vec3), 
                     crosshairLines.data(), GL_DYNAMIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
        
        // 셰이더 유니폼 설정
        GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
        GLint viewLoc2 = glGetUniformLocation(shaderProgramID, "view");
        GLint projLoc2 = glGetUniformLocation(shaderProgramID, "proj");
        GLint colorLoc = glGetUniformLocation(shaderProgramID, "objectColor");
        GLint useTextureLoc2 = glGetUniformLocation(shaderProgramID, "useTexture");
        
        glm::mat4 model = glm::mat4(1.0f);
        
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc2, 1, GL_FALSE, glm::value_ptr(proj));
        glUniform3f(colorLoc, 0.0f, 1.0f, 0.0f); // 녹색
        glUniform1f(useTextureLoc2, 0.0f); // 텍스처 사용 안함
        
        // 선 두께 설정
        glLineWidth(3.0f);
        
        // 선 그리기
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(crosshairLines.size()));
        
        // 정리
        glBindVertexArray(0);
        glDeleteBuffers(1, &tempVBO);
        glDeleteVertexArrays(1, &tempVAO);
        
        // 깊이 테스트 다시 켜기
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }

    // ===== 2단계: 기본 프레임버퍼에 후처리 적용 =====
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(postprocessShaderID);
    glBindVertexArray(quadVAO);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    
    // 야간투시 모드는 거너뷰일 때만 활성화
    bool applyNightVision = enableNightVision && (camera && camera->GetCameraMode() == 2);
    glUniform1i(glGetUniformLocation(postprocessShaderID, "enableNightVision"), applyNightVision);
    glUniform1f(glGetUniformLocation(postprocessShaderID, "time"), (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // ===== 3단계: ImGui 렌더링 =====
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
    
    // 카메라 정보
    if (camera) {
        ImGui::Separator();
        ImGui::Text("Camera");
        ImGui::Text("Distance: %.1f", camera->GetDistance());
        ImGui::Text("(Mouse Wheel to Zoom)");
        ImGui::Separator();
    }

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

    // 카메라 모드 전환
    if (camera) {
        ImGui::Separator();
        ImGui::Text("Camera");

        const char* modes[] = { "3rd Person", "Cockpit View", "Gunner View" };
        int currentMode = camera->GetCameraMode();

        if (ImGui::Combo("View Mode", &currentMode, modes, IM_ARRAYSIZE(modes))) {
            camera->SetCameraMode(currentMode);
        }

        // Gunner View 모드일 때만 오프셋 조정 가능
        if (currentMode == 2) {
            ImGui::Separator();
            ImGui::Text("Gunner Offset");
            glm::vec3& offset = camera->GetGunnerOffset();
            
            if (ImGui::SliderFloat("Forward/Back", &offset.x, -50.0f, 50.0f)) {
                camera->SetGunnerOffset(offset);
            }
            if (ImGui::SliderFloat("Up/Down", &offset.y, -30.0f, 30.0f)) {
                camera->SetGunnerOffset(offset);
            }
            if (ImGui::SliderFloat("Left/Right", &offset.z, -30.0f, 30.0f)) {
                camera->SetGunnerOffset(offset);
            }
        }

        ImGui::Separator();
    }

	ImGui::DragFloat("crosshair size", &crosshairSize, 0.1f, 1.0f, 100.0f);
	ImGui::DragFloat("crosshair distance", &crosshairDistance, 1.0f, -200.0f, 400.0f);
    ImGui::Separator();  

    if (ImGui::Button("wired frame"))
    {
        wireframeMode = !wireframeMode;
    }

    ImGui::Separator();
    
    // 야간투시 토글 (거너뷰일 때만 활성화)
    if (camera && camera->GetCameraMode() == 2) {
        ImGui::Text("Night Vision (Gunner View)");
        if (ImGui::Checkbox("Enable Night Vision", &enableNightVision)) {
            std::cout << "Night Vision: " << (enableNightVision ? "ON" : "OFF") << std::endl;
        }
    }
    else {
        ImGui::TextDisabled("Night Vision (Gunner View Only)");
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
    
    // 프레임버퍼 텍스처 크기 재조정
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    
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
            float cannonYaw = helicopter->GetCannonYaw();
            float cannonPitch = helicopter->GetCannonPitch();
            camera->ProcessMouseDrag(deltaX, deltaY, yaw, cannonYaw, cannonPitch);
            helicopter->SetYaw(yaw);
			helicopter->SetCannonYaw(cannonYaw);
			helicopter->SetCannonPitch(cannonPitch);

            lastMouseX = x;
            lastMouseY = y;

            glutPostRedisplay();
        }
    }
}

void InitBuffers() {
    // 프레임버퍼 생성
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    // 텍스처 생성 (컬러 버퍼)
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    
    // 렌더버퍼 생성 (깊이/스텐실)
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    
    // 프레임버퍼 완성 확인
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "프레임버퍼 생성 실패!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // 전체 화면 쿼드 생성
    float quadVertices[] = {
        // 위치       // 텍스처 좌표
        -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };
    
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
    
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

    //크로스헤어 버퍼 생성
    float crosshairVertices[] = {
        // 위치 (십자가 모양)
        -1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
         1.0f, -1.0f
    };
    
    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);
    glBindVertexArray(crosshairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    
    glBindVertexArray(0);
}

void InitializeAAUnits()
{
    aaUnits = new AA*[NUM_AA_UNITS];
    
    AA::LoadSharedModel();
    
    // 랜덤 생성기 초기화
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // 맵 크기 기준 (Ground 크기 참고: -5000 ~ 5000)
    std::uniform_real_distribution<float> distX(-1300.0f, 1300.0f);
    std::uniform_real_distribution<float> distZ(-1300.0f, 1300.0f);
    
    std::cout << "\n=== AA 유닛 배치 시작 ===" << std::endl;
    
    for (int i = 0; i < NUM_AA_UNITS; ++i) {
        aaUnits[i] = new AA();
        aaUnits[i]->Initialize();
        aaUnits[i]->InitBuffers();  // LoadModel() 대신 InitBuffers()만 호출
        
        // 랜덤 위치 생성
        float randomX = distX(gen);
        float randomZ = distZ(gen);
        float y = 5.0f;
        
        glm::vec3 randomPos(randomX, y, randomZ);
        aaUnits[i]->SetPosition(randomPos);
        
        std::cout << "AA Unit " << i << " 배치: (" 
                  << randomX << ", " << y << ", " << randomZ << ")" << std::endl;
    }
    
    std::cout << "=== AA 유닛 " << NUM_AA_UNITS << "대 배치 완료 ===" << std::endl;
}