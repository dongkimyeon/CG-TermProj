#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include "CommonInclude.h" 
#include "Time.h"
#include "Texture.h"


// FBX 모델 데이터 
struct FBXModel
{
    std::vector<glm::vec3> vertices;  // 위치
    std::vector<glm::vec3> colors;
    std::vector<glm::vec2> uvs; // UV 좌표
    std::vector<GLuint> indices;      // 인덱스
    std::vector<Texture*> textureList; // 텍스처 리스트
    
    // 메시별 정보 추가
    struct MeshInfo {
        GLuint indexStart;  // 이 메시의 인덱스 시작 위치
        GLuint indexCount;  // 이 메시의 인덱스 개수
        GLuint materialIndex; // 이 메시가 사용하는 머티리얼 인덱스
    };
    std::vector<MeshInfo> meshes; // 메시별 정보
    
    glm::vec3 center;
    bool loaded = false;
};

// 함수 선언
void MakeVertexShaders();
void MakeFragmentShaders();
GLuint MakeShaderProgram();
void InitBuffers();
GLvoid DrawScene();
GLvoid Reshape(int w, int h);

//glut 콜백 함함수

void SpecialKeyboard(int key, int x, int y);
void Mouse(int button, int state, int x, int y);
void WhellFunc(int whell, int dir, int x, int y);
void Motion(int x, int y);
void Timer(int value);
void KeyBoard();
void UpdateModelBuffers(FBXModel* model, GLuint vao, GLuint vbo, GLuint ebo); //해당 모델을 버퍼에 업데이트 (버퍼 ID 매개변수 추가)
bool LoadFBX(const char* filename, FBXModel* model); // FBX 로드 함수 
glm::vec3 CalculateModelCenter(const std::vector<glm::vec3>& vertices); // 센터 계산 


//스카이 박스 관련 함수
void MakeSkyboxVertexShader();
void MakeSkyboxFragmentShader();
GLuint MakeSkyboxShaderProgram();
GLuint loadCubemap(std::vector<std::string> faces);

// 전역 변수
GLint width = 1280, height = 720;
GLuint shaderProgramID, vertexShader, fragmentShader;

// 모델별 VAO/VBO/EBO 
GLuint VAO_Body, VBO_Body, EBO_Body;
GLuint VAO_Blade, VBO_Blade, EBO_Blade;
GLuint VAO_Tail, VBO_Tail, EBO_Tail;
GLuint axisVAO, axisVBO;  // 축 그리기용 별도 VAO/VBO




//스카이박스 관련
GLuint skyboxVAO, skyboxVBO;
GLuint skyboxShaderProgramID, vertexSkyboxShader, fragmentSkyboxShader;
GLuint cubemapTexture;


//카메라 관련
glm::vec3 cameraPos = glm::vec3(75.0f, 75.0f, 75.0f);
float cameraRadius = 0.0f; // 카메라 반지름 추가
float cameraAngle = 0.0f; // 초기 X-Z 각도 (45도 라디안)
float cameraYAngle = 0.0f; // 초기 Y 각도 (고도각)
float targetCameraXAngle = 0.0f; // 타겟 X-Z 카메라 각도
float targetCameraYAngle = 0.0f; // 타겟 Y 카메라 각도

bool rightClickDown = false; // 마우스 우클릭 상태
int lastMouseX = 0;          // 마지막 마우스 X 위치
int lastMouseY = 0;          // 마지막 마우스 Y 위치
float rotationSpeed = 0.005f; // 카메라 회전 속도 (조정 가능)

float cameraDistance = 150.0f;  // 헬기로부터의 거리
float cameraHeight = 50.0f;     // 헬기 위쪽으로의 높이

//선형보간 속도
float interpSpeed = 10.0f;



//imgui 관련 변수
bool showAxis = true;  // 축 표시 토글

float modelScale = 0.1f;       // ImGui로 제어할 스케일
int targetFrameDelay = 1;
bool wireframeMode = false;
float glassAlpha = 0.5f;  // 유리 투명도 추가

//전체 모델 회전 
float xModelRotation = 0.0f;
float yModelRotation = 0.0f;
float zModelRotation = 0.0f;

//전체 모델 이동
glm::vec3 modelPosition = glm::vec3(0.0f, 0.0f, 0.0f);



float mainBladeRotation = 0.0f;
float mainBladeSpeed = 2000.0f;
float mainBladeX = 0.0f;
float mainBladeY = 0.0f;
float mainBladeZ = 0.0f;

float tailBladeRotation = 0.0f;
float tailBladeSpeed = 2000.0f;
float tailBladeX = 0.0f;
float tailBladeY = 0.0f;
float tailBladeZ = 0.0f;


//물리에 필요한거
float gravity = 9.81f; // 중력 가속도
float thrust = 0.0f;   // 상승력
bool thrustUp = false; // 상승력 증가 여부

// *** 가속도 기반 물리 시스템 변수 ***
glm::vec3 velocity = glm::vec3(0.0f);           // 헬기 속도
glm::vec3 acceleration = glm::vec3(0.0f);       // 헬기 가속도
glm::vec3 angularVelocity = glm::vec3(0.0f);    // 각속도 (roll, pitch, yaw)
glm::vec3 angularAcceleration = glm::vec3(0.0f); // 각가속도

// 물리 상수
float heliMass = 1.0f;              // 헬기 질량 (상대값)
float thrustAcceleration = 30.0f;   // 추력 가속도
float airDrag = 0.5f;               // 공기 저항 계수
float angularDrag = 5.0f;           // 회전 저항 계수
float maxVelocity = 100.0f;         // 최대 속도
float maxAngularVelocity = 180.0f;  // 최대 각속도 (도/초)

// 헬기 로컬 좌표계 기저 벡터
glm::vec3 heliRight = glm::vec3(1.0f, 0.0f, 0.0f);   // 로컬 X축 (우측)
glm::vec3 heliUp = glm::vec3(0.0f, 1.0f, 0.0f);      // 로컬 Y축 (상향)
glm::vec3 heliForward = glm::vec3(0.0f, 0.0f, 1.0f); // 로컬 Z축 (전방)

// FBX 모델들 
FBXModel mHeliBody;
FBXModel mHeliBlade;
FBXModel mHeliTail;

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

    MakeVertexShaders();
    MakeFragmentShaders();
    shaderProgramID = MakeShaderProgram();
    if (shaderProgramID == 0) {
        std::cerr << "셰이더 프로그램 생성 실패" << std::endl;
        return -1;
    }

    MakeSkyboxVertexShader();
    MakeSkyboxFragmentShader();
    skyboxShaderProgramID = MakeSkyboxShaderProgram();
    if (skyboxShaderProgramID == 0) {
        std::cerr << "스카이박스 셰이더 프로그램 생성 실패" << std::endl;
        return -1;
    }

    InitBuffers();  // 변경: 모델별 버퍼 초기화
    // 카메라 초기 설정
    cameraRadius = glm::length(cameraPos);
    float r_xz_init = glm::sqrt(cameraPos.x * cameraPos.x + cameraPos.z * cameraPos.z);
    cameraAngle = atan2(cameraPos.z, cameraPos.x);
    cameraYAngle = atan2(cameraPos.y, r_xz_init);
    targetCameraXAngle = cameraAngle;
    targetCameraYAngle = cameraYAngle;

    // 초기 FBX 로드 (예시 파일) + 버퍼 업데이트
    if (!LoadFBX("HeliBlade.FBX", &mHeliBlade)) {
        std::cerr << "초기 FBX 로드 실패. 파일을 확인하세요." << std::endl;
    }
    else {
        UpdateModelBuffers(&mHeliBlade, VAO_Blade, VBO_Blade, EBO_Blade);  // 변경: 모델별 버퍼 호출
    }
    if (!LoadFBX("HeliBody.FBX", &mHeliBody)) {
        std::cerr << "초기 FBX 로드 실패. 파일을 확인하세요." << std::endl;
    }
    else {
        UpdateModelBuffers(&mHeliBody, VAO_Body, VBO_Body, EBO_Body);  // 변경: 모델별 버퍼 호출
    }
    if (!LoadFBX("HeliTail.FBX", &mHeliTail)) {
        std::cerr << "초기 FBX 로드 실패. 파일을 확인하세요." << std::endl;
    }
    else {
        UpdateModelBuffers(&mHeliTail, VAO_Tail, VBO_Tail, EBO_Tail);  // 변경: 모델별 버퍼 호출
    }

    std::vector<std::string> faces = {
          "SkyBox-Texture/px.png", // 오른쪽
          "SkyBox-Texture/nx.png", // 왼쪽
          "SkyBox-Texture/py.png", // 위
          "SkyBox-Texture/ny.png", // 아래
          "SkyBox-Texture/pz.png", // 앞
          "SkyBox-Texture/nz.png"  // 뒤
    };
    cubemapTexture = loadCubemap(faces);
    if (cubemapTexture == 0) {
        std::cerr << "스카이박스 큐브맵 로드 실패" << std::endl;
    }
    Time::Initialize();

    glutDisplayFunc(DrawScene);
    glutReshapeFunc(Reshape);
  
    glutTimerFunc(targetFrameDelay, Timer, 0); // ~60 FPS
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
	glutPassiveMotionFunc(Motion);
    glutSpecialFunc(SpecialKeyboard);
	glutMouseWheelFunc(WhellFunc);

    glutMainLoop();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
    return 0;
}
void WhellFunc(int whell, int dir, int x, int y)
{
	float zoomSpeed = 5.0f;
    if(dir > 0)
    {
        cameraRadius -= zoomSpeed;
        if (cameraRadius < 2.0f)
            cameraRadius = zoomSpeed;
    }
    else
    {
        cameraRadius += zoomSpeed;
	}
}
void UpdateModelBuffers(FBXModel* model, GLuint vao, GLuint vbo, GLuint ebo)  // 변경: 버퍼 ID 매개변수 추가
{
    if (!model->loaded || model->vertices.empty() || model->indices.empty()) {
        std::cerr << "모델 버퍼 업데이트 실패: 로드된 데이터 없음" << std::endl;
        return;
    }

    // 1. 버텍스 데이터: 위치 + 색상 + UV (센터 정규화)
    std::vector<GLfloat> vertexData;
    vertexData.reserve(model->vertices.size() * 8); // 8 = pos(3) + color(3) + uv(2)

    for (size_t i = 0; i < model->vertices.size(); ++i) {
        glm::vec3 pos = model->vertices[i] - model->center;
        vertexData.insert(vertexData.end(), { pos.x, pos.y, pos.z });
        vertexData.insert(vertexData.end(), { model->colors[i].r, model->colors[i].g, model->colors[i].b });
        vertexData.insert(vertexData.end(), { model->uvs[i].x, model->uvs[i].y }); // UV 추가 
    }

    // 2. VBO 및 EBO에 데이터 업로드 (GL_STATIC_DRAW 사용, 해당 버퍼만)
    glBindVertexArray(vao);  // 변경: 매개변수 vao 사용

    glBindBuffer(GL_ARRAY_BUFFER, vbo);  // 변경: 매개변수 vbo 사용
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat), vertexData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);  // 변경: 매개변수 ebo 사용
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->indices.size() * sizeof(GLuint), model->indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    std::cout << "모델 버퍼가 GPU에 업로드되었습니다. (정점 " << model->vertices.size() << "개)" << std::endl;  // 변경: model 이름 출력 위해 간단히
}
bool LoadFBX(const char* filename, FBXModel* fbxModel)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Assimp 로드 실패: " << importer.GetErrorString() << std::endl;
        return false;
    }

    fbxModel->vertices.clear();
    fbxModel->colors.clear();
    fbxModel->uvs.clear();
    fbxModel->indices.clear();
    fbxModel->meshes.clear();  // 메시 정보 초기화
    fbxModel->loaded = false;

    // *** 머티리얼 슬롯 정보 출력 추가 ***
    std::cout << "\n=== 파일: " << filename << " ===" << std::endl;
    std::cout << "머티리얼 개수: " << scene->mNumMaterials << std::endl;
    
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* material = scene->mMaterials[i];
        aiString matName;
        material->Get(AI_MATKEY_NAME, matName);
        
        std::cout << "\n[Material " << i << "]" << std::endl;
        std::cout << "  이름: " << matName.C_Str() << std::endl;
        
        // Diffuse 텍스처 확인
        unsigned int diffuseCount = material->GetTextureCount(aiTextureType_DIFFUSE);
        std::cout << "  Diffuse 텍스처 개수: " << diffuseCount << std::endl;
        
        for (unsigned int j = 0; j < diffuseCount; j++) {
            aiString texPath;
            if (material->GetTexture(aiTextureType_DIFFUSE, j, &texPath) == AI_SUCCESS) {
                std::cout << "    - Diffuse[" << j << "]: " << texPath.C_Str() << std::endl;
            }
        }
        
        // Specular 텍스처 확인
        unsigned int specularCount = material->GetTextureCount(aiTextureType_SPECULAR);
        if (specularCount > 0) {
            std::cout << "  Specular 텍스처 개수: " << specularCount << std::endl;
        }
        
        // Normal 텍스처 확인
        unsigned int normalCount = material->GetTextureCount(aiTextureType_NORMALS);
        if (normalCount > 0) {
            std::cout << "  Normal 텍스처 개수: " << normalCount << std::endl;
        }
    }
    std::cout << "====================\n" << std::endl;

    // 메쉬
    // 모든 메쉬를 순회하며 데이터를 병합합니다.
    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[m];

        // 이 메쉬의 정점이 시작되기 전, fbxModel에 이미 저장된 정점의 개수.
        // 그것이 이 메쉬의 인덱스 오프셋(offset)이 됩니다.
        GLuint vertexOffset = static_cast<GLuint>(fbxModel->vertices.size());
        GLuint indexStart = static_cast<GLuint>(fbxModel->indices.size());

        // 1. 정점(Vertices), UV, 색상(Colors) 데이터 추가
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            // 위치 로드 후 변환 적용
            glm::vec3 pos(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

            // 좌표계 변환: Y-up -> Z-up (X축 90도 회전 매트릭스 적용)
            glm::mat4 coordTransform = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            pos = glm::vec3(coordTransform * glm::vec4(pos, 1.0f));

            fbxModel->vertices.push_back(pos);

            // UV 로드 추가: aiTextureCoords[0] 채널 사용 (없으면 0으로 기본값)
            glm::vec2 uv(0.0f, 0.0f);
            if (mesh->HasTextureCoords(0)) {
                uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }
            fbxModel->uvs.push_back(uv);

            // 색상 
            glm::vec3 colors[4] = { {1,0,0}, {0,1,0}, {0,0,1}, {1,1,0} };
            fbxModel->colors.push_back(colors[(vertexOffset + i) % 4]);
        }

        // 2. 인덱스(Indices) 데이터 추가
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            // Assimp가 aiProcess_Triangulate를 보장하므로 mNumIndices는 3입니다.
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                // 중요: 그냥 face.mIndices[j]를 추가하면 안 됩니다!
                // 이미 fbxModel.vertices에 저장된 정점들을 고려하여
                // 'vertexOffset'을 더해줘야 합니다.
                fbxModel->indices.push_back(face.mIndices[j] + vertexOffset);
            }
        }

        // 메시 정보 저장
        FBXModel::MeshInfo meshInfo;
        meshInfo.indexStart = indexStart;
        meshInfo.indexCount = static_cast<GLuint>(fbxModel->indices.size() - indexStart);
        meshInfo.materialIndex = mesh->mMaterialIndex;
        fbxModel->meshes.push_back(meshInfo);
        
        std::cout << "Mesh " << m << ": Material Index = " << mesh->mMaterialIndex 
                  << ", Indices = " << meshInfo.indexCount << std::endl;

    } // 다음 메쉬로 이동


    // 모든 메쉬가 병합된 후 센터 계산
    fbxModel->center = CalculateModelCenter(fbxModel->vertices);
    fbxModel->loaded = true;
    std::cout << "FBX 로드 성공: " << scene->mNumMeshes << " meshes combined." << std::endl;
    std::cout << "Total Vertices: " << fbxModel->vertices.size() << ", Total Indices: " << fbxModel->indices.size() << std::endl;



    // 텍스처 로딩: material별로
    fbxModel->textureList.resize(scene->mNumMaterials);
    for (size_t i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* material = scene->mMaterials[i];
        fbxModel->textureList[i] = nullptr;

        // 머티리얼 이름 가져오기
        aiString matName;
        material->Get(AI_MATKEY_NAME, matName);
        std::string materialName = matName.C_Str();

        // Diffuse 텍스처 경로 확인
        aiString texPath;
        bool hasTexture = false;
        std::string textureFile;

        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
            // FBX에서 지정한 텍스처 경로가 있는 경우
            std::string fullPath = texPath.C_Str();
            
            // 경로에서 파일 이름만 추출 (역슬래시와 슬래시 모두 고려)
            size_t lastSlash = fullPath.find_last_of("/\\");
            if (lastSlash != std::string::npos) {
                textureFile = fullPath.substr(lastSlash + 1);
            } else {
                textureFile = fullPath;
            }
            
            // .TGA를 .png로 변경 (확장자 변환)
            size_t extPos = textureFile.find_last_of(".");
            if (extPos != std::string::npos) {
                std::string ext = textureFile.substr(extPos);
                // TGA를 PNG로 변환
                if (ext == ".TGA" || ext == ".tga") {
                    textureFile = textureFile.substr(0, extPos) + ".png";
                }
            }
            
            hasTexture = true;
        }

        // 머티리얼 이름 또는 인덱스에 따라 텍스처 할당
        if (materialName == "MI_West_Heli_AH64D_Main") {
            // Material 0: HeliTexture.png 사용 (FBX에 텍스처 정보가 없으므로 직접 지정)
            fbxModel->textureList[i] = new Texture("HeliTexture.png");
            std::cout << "Material " << i << " (" << materialName << ")에 HeliTexture.png 할당" << std::endl;
        }
        else if (materialName == "MI_West_Heli_AH64D_Glass_" && hasTexture) {
            // Material 1: FBX에서 지정한 텍스처 사용
            fbxModel->textureList[i] = new Texture(textureFile.c_str());
            std::cout << "Material " << i << " (" << materialName << ")에 " << textureFile << " 할당" << std::endl;
        }
        else {
            // 기본값: HeliTexture.png 사용
            fbxModel->textureList[i] = new Texture("HeliTexture.png");
            std::cout << "Material " << i << " (" << materialName << ")에 HeliTexture.png 할당 (기본값)" << std::endl;
        }

        // 텍스처 로드 시도
        if (!fbxModel->textureList[i]->LoadTexture()) {  
            std::cerr << "텍스처 로드 실패: Material " << i << std::endl;
            delete fbxModel->textureList[i];
            fbxModel->textureList[i] = nullptr;
        }
        else {
            std::cout << "텍스처 로드 성공: Material " << i << std::endl;
        }
    }

    
    for (size_t i = 0; i < fbxModel->textureList.size(); ++i) {
        if (fbxModel->textureList[i]) {
            std::cout << "Material " << i << " Texture ID: " << fbxModel->textureList[i] << std::endl;
        }
        else {
            std::cout << "Material " << i << ": No Texture" << std::endl;
        }
    }

    return fbxModel->loaded;
}

glm::vec3 CalculateModelCenter(const std::vector<glm::vec3>& vertices) {
    if (vertices.empty()) return glm::vec3(0.0f);
    glm::vec3 center(0.0f);
    for (const auto& v : vertices) {
        center += v;
    }
    center /= static_cast<float>(vertices.size());
    return center;
}

void DrawScene()
{
    Time::Update();

    mainBladeRotation += mainBladeSpeed * Time::DeltaTime();
    tailBladeRotation += tailBladeSpeed * Time::DeltaTime();
    // 카메라가 헬기를 뒤에서 쫓아가도록 위치 계산
  // 헬기의 로컬 좌표계 기준 (회전 반영)
    glm::vec3 cameraOffset = -heliForward * cameraDistance + heliUp * cameraHeight;
    cameraPos = modelPosition + cameraOffset;

    // 카메라 목표 지점 (헬기 위치보다 약간 위)
    glm::vec3 cameraTarget = modelPosition + heliUp * 10.0f;


    // 씬 클리어
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // 알파 블렌딩 활성화
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // --- 기본 깊이 테스트 함수 설정 ---
    glDepthFunc(GL_LESS);

    glUseProgram(shaderProgramID);

    // Uniform 설정 (View, Proj - 모든 객체 공통)
    GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLint projLoc = glGetUniformLocation(shaderProgramID, "proj");
    GLint textureLoc = glGetUniformLocation(shaderProgramID, "textureSampler");
    GLint useVertexColorLoc = glGetUniformLocation(shaderProgramID, "useVertexColor");
    GLint alphaValueLoc = glGetUniformLocation(shaderProgramID, "alphaValue");  // 알파값 유니폼 위치

    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, heliUp);



    glm::mat4 proj = glm::perspective(glm::radians(100.0f), (float)width / (float)height, 0.1f, 1000.0f);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

    GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");

    //헬기 전체 모델 매트릭스 
	glm::mat4 worldModelMat = glm::mat4(1.0f);
	worldModelMat = glm::translate(worldModelMat, modelPosition);
    worldModelMat = glm::rotate(worldModelMat, glm::radians(yModelRotation), glm::vec3(0.0f, 1.0f, 0.0f));
	worldModelMat = glm::rotate(worldModelMat, glm::radians(xModelRotation), glm::vec3(1.0f, 0.0f, 0.0f));
	worldModelMat = glm::rotate(worldModelMat, glm::radians(zModelRotation), glm::vec3(0.0f, 0.0f, 1.0f));


    // 5. 모델 그리기
    //몸체 (변경: VAO_Body 바인딩, 메시별로 다른 텍스처 적용)
    if (mHeliBody.loaded && !mHeliBody.indices.empty())
    {
        glUniform1i(useVertexColorLoc, 0);
        // 5-1. 모델 매트릭스 설정
        glm::mat4 modelMat = worldModelMat;
        modelMat = glm::scale(modelMat, glm::vec3(modelScale));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));

        glBindVertexArray(VAO_Body);
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // 메시별로 다른 텍스처를 적용하여 그리기
        for (size_t i = 0; i < mHeliBody.meshes.size(); ++i) {
            const auto& meshInfo = mHeliBody.meshes[i];
            
            // 머티리얼 인덱스에 따라 알파값 설정
            if (meshInfo.materialIndex == 1) {
                // Material 1 (유리): 투명도 적용
                glUniform1f(alphaValueLoc, glassAlpha);
                // 투명 객체는 깊이 쓰기 비활성화 (선택적)
                glDepthMask(GL_FALSE);
            } else {
                // 다른 Material: 불투명
                glUniform1f(alphaValueLoc, 1.0f);
                glDepthMask(GL_TRUE);
            }
            
            // 해당 메시의 머티리얼 인덱스로 텍스처 바인딩
            if (meshInfo.materialIndex < mHeliBody.textureList.size() && 
                mHeliBody.textureList[meshInfo.materialIndex]) {
                mHeliBody.textureList[meshInfo.materialIndex]->UseTexture();
                glUniform1i(textureLoc, 0);
            }

            // 해당 메시만 그리기 (인덱스 오프셋 사용)
            glDrawElements(GL_TRIANGLES, meshInfo.indexCount, GL_UNSIGNED_INT, 
                          (void*)(meshInfo.indexStart * sizeof(GLuint)));
        }
        
        // 깊이 쓰기 다시 활성화
        glDepthMask(GL_TRUE);

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    //Main Blade
    if (mHeliBlade.loaded && !mHeliBlade.indices.empty())
    {
        glUniform1i(useVertexColorLoc, 0);
        glUniform1f(alphaValueLoc, 1.0f);  // 불투명
        
        // 5-1. 모델 매트릭스 설정
        glm::mat4 modelMat = worldModelMat;
        modelMat = glm::translate(modelMat, glm::vec3(2.5f, 18.0f, 0.0f));
        modelMat = glm::rotate(modelMat, glm::radians(mainBladeRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMat = glm::scale(modelMat, glm::vec3(modelScale));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));

        glBindVertexArray(VAO_Blade);
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // 메시별로 다른 텍스처를 적용하여 그리기
        for (size_t i = 0; i < mHeliBlade.meshes.size(); ++i) {
            const auto& meshInfo = mHeliBlade.meshes[i];
            
            if (meshInfo.materialIndex < mHeliBlade.textureList.size() && 
                mHeliBlade.textureList[meshInfo.materialIndex]) {
                mHeliBlade.textureList[meshInfo.materialIndex]->UseTexture();
                glUniform1i(textureLoc, 0);
            }

            glDrawElements(GL_TRIANGLES, meshInfo.indexCount, GL_UNSIGNED_INT, 
                          (void*)(meshInfo.indexStart * sizeof(GLuint)));
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    //Tail Blade
    if (mHeliTail.loaded && !mHeliTail.indices.empty())
    {
        glUniform1i(useVertexColorLoc, 0);
        glUniform1f(alphaValueLoc, 1.0f);  // 불투명
        
        // 5-1. 모델 매트릭스 설정
        glm::mat4 modelMat = worldModelMat;
        modelMat = glm::translate(modelMat, glm::vec3(-88.0f, 17.0f, -7.0f));
        modelMat = glm::rotate(modelMat, glm::radians(tailBladeRotation), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMat = glm::scale(modelMat, glm::vec3(modelScale));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));

        glBindVertexArray(VAO_Tail);
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // 메시별로 다른 텍스처를 적용하여 그리기
        for (size_t i = 0; i < mHeliTail.meshes.size(); ++i) {
            const auto& meshInfo = mHeliTail.meshes[i];
            
            if (meshInfo.materialIndex < mHeliTail.textureList.size() && 
                mHeliTail.textureList[meshInfo.materialIndex]) {
                mHeliTail.textureList[meshInfo.materialIndex]->UseTexture();
                glUniform1i(textureLoc, 0);
            }

            glDrawElements(GL_TRIANGLES, meshInfo.indexCount, GL_UNSIGNED_INT, 
                          (void*)(meshInfo.indexStart * sizeof(GLuint)));
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // 축 그리기 (토글 가능)
    if (showAxis) {
        glUniform1i(useVertexColorLoc, 1);
        // 6-1. 모델 매트릭스 설정 (축은 기본)
        glm::mat4 axisMat = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(axisMat));

        // 6-2. 그리기
        glBindVertexArray(axisVAO);
        glDrawArrays(GL_LINES, 0, 6);
        glBindVertexArray(0);
    }



    // --- 스카이박스 그리기 ---
    // 깊이 테스트 함수를 GL_LEQUAL로 변경합니다. 
    glDepthFunc(GL_LEQUAL);

    glUseProgram(skyboxShaderProgramID); // 스카이박스 전용 셰이더 사용

    // 셰이더에 view, projection 행렬 전달
    // (스카이박스 셰이더가 내부적으로 view의 이동값을 제거함)
    GLint skyViewLoc = glGetUniformLocation(skyboxShaderProgramID, "view");
    GLint skyProjLoc = glGetUniformLocation(skyboxShaderProgramID, "projection");
    glUniformMatrix4fv(skyViewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(skyProjLoc, 1, GL_FALSE, glm::value_ptr(proj));

    // 스카이박스 큐브맵 텍스처 바인딩
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0); // 텍스처 유닛 0 사용
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    // 셰이더의 skybox 샘플러가 텍스처 유닛 0을 사용하도록 설정
    glUniform1i(glGetUniformLocation(skyboxShaderProgramID, "skybox"), 0);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 스카이박스 큐브 그리기
    glBindVertexArray(0);

    glDepthFunc(GL_LESS); // 깊이 테스트 함수를 다시 기본값으로 복원
    // --- 스카이박스 그리기 끝 ---


    // ImGui 렌더링 (이하 동일)
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(
        ImVec2((float)width, 0.0f),
        ImGuiCond_FirstUseEver,
        ImVec2(1.0f, 0.0f)
    );
    ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);

    // ImGui UI 윈도우
    ImGui::Begin("FBX Model Controls");

    static float time = 0.0f;
    time += Time::DeltaTime();
    float fps = 1.0f / Time::DeltaTime();
    ImGui::Text("FPS: %.1f", fps);
    
    // *** 물리 정보 표시 ***
    ImGui::Separator();
    ImGui::Text("Physics Info:");
    ImGui::Text("Velocity: (%.1f, %.1f, %.1f)", velocity.x, velocity.y, velocity.z);
    ImGui::Text("Speed: %.1f", glm::length(velocity));
    ImGui::Text("Thrust: %.1f", thrust);
    ImGui::Text("Position: (%.1f, %.1f, %.1f)", modelPosition.x, modelPosition.y, modelPosition.z);
    ImGui::Separator();
    ImGui::Text("Angular Velocity: (%.1f, %.1f, %.1f)", angularVelocity.x, angularVelocity.y, angularVelocity.z);
    ImGui::Separator();
     
    // === 물리 파라미터 조정 ===
    ImGui::SliderFloat("Thrust Accel", &thrustAcceleration, 10.0f, 100.0f);
    ImGui::SliderFloat("Air Drag", &airDrag, 0.0f, 2.0f);
    ImGui::SliderFloat("Angular Drag", &angularDrag, 0.0f, 10.0f);
    ImGui::SliderFloat("Gravity", &gravity, 0.0f, 20.0f);
    ImGui::Separator();
    
    // === 갱신 주기 제어 슬라이더 ===
    ImGui::SliderInt("Frame Delay ", &targetFrameDelay, 1, 16);
    ImGui::Separator();
    ImGui::SliderFloat("Blade Speed", &mainBladeSpeed, 100, 1000);
    ImGui::Separator();
    ImGui::SliderFloat("Tail Speed", &tailBladeSpeed, 100, 1000);
    ImGui::Separator();
    ImGui::SliderFloat("Glass Alpha", &glassAlpha, 0.0f, 1.0f);  // 유리 투명도 슬라이더 추가
    ImGui::Separator();

	ImGui::SliderFloat("Model ModelRotationX", &xModelRotation, -180.0f, 180.0f);
	ImGui::SliderFloat("Model ModelRotationY", &yModelRotation, -180.0f, 180.0f);
	ImGui::SliderFloat("Model ModelRotationZ", &zModelRotation, -180.0f, 180.0f);
    ImGui::Separator();
    ImGui::SliderFloat("Camera Distance", &cameraDistance, 10.0f, 200.0f);
    ImGui::SliderFloat("Camera Height", &cameraHeight, 0.0f, 100.0f);




    if (ImGui::Button("wired frame"))
    {
        wireframeMode = !wireframeMode;
    }

    if (ImGui::Button("XYZ"))
    {
        showAxis = !showAxis;
    }
    
    // 물리 초기화 버튼
    if (ImGui::Button("Reset Physics"))
    {
        velocity = glm::vec3(0.0f);
        angularVelocity = glm::vec3(0.0f);
        acceleration = glm::vec3(0.0f);
        angularAcceleration = glm::vec3(0.0f);
        thrust = 0.0f;
        modelPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        xModelRotation = 0.0f;
        yModelRotation = 0.0f;
        zModelRotation = 0.0f;
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
    ImGui_ImplGLUT_ReshapeFunc(w, h);  // ImGui 리셰이프
}


void SpecialKeyboard(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_RIGHT:
		modelPosition.x += 10.0f; 
        break;
    case GLUT_KEY_LEFT:
		modelPosition.x -= 10.0f;
        break;
    case GLUT_KEY_UP:
		modelPosition.z += 1.0f;
        break;
    case GLUT_KEY_DOWN:
		modelPosition.z -= 1.0f;
        break;
   
    }
}

void Timer(int value) {
    Input::Update();
    KeyBoard();

    float deltaTime = Time::DeltaTime();

    // 카메라용 회전 매트릭스: Y축 회전(yaw)만 적용
    glm::mat4 cameraRotationMat = glm::mat4(1.0f);
    cameraRotationMat = glm::rotate(cameraRotationMat, glm::radians(yModelRotation), glm::vec3(0.0f, 1.0f, 0.0f));

    // 헬기의 기본 방향 벡터
    glm::vec3 baseForward = glm::vec3(-1.0f, 0.0f, 0.0f);
    glm::vec3 baseUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // 카메라용 기저벡터 (Y축 회전만 반영)
    heliForward = glm::vec3(cameraRotationMat * glm::vec4(baseForward, 0.0f));
    heliUp = glm::vec3(cameraRotationMat * glm::vec4(baseUp, 0.0f));

    // 헬기 물리용 회전 매트릭스: 모든 회전 적용
    glm::mat4 heliRotationMat = glm::mat4(1.0f);
    heliRotationMat = glm::rotate(heliRotationMat, glm::radians(yModelRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    heliRotationMat = glm::rotate(heliRotationMat, glm::radians(xModelRotation), glm::vec3(1.0f, 0.0f, 0.0f));
    heliRotationMat = glm::rotate(heliRotationMat, glm::radians(zModelRotation), glm::vec3(0.0f, 0.0f, 1.0f));

    // 헬기의 실제 로컬 좌표계 (물리 계산용)
    glm::vec3 heliActualUp = glm::vec3(heliRotationMat[1]);

    // *** 가속도 기반 물리 시스템 ***
    
    // 1. 중력 가속도 (항상 아래 방향)
    glm::vec3 gravityAccel = glm::vec3(0.0f, -gravity, 0.0f);
    
    // 2. 추력 가속도 (헬기의 실제 상향 벡터 방향)
    glm::vec3 thrustAccel = glm::vec3(0.0f);
    if (thrustUp) {
        thrustAccel = heliActualUp * (thrust * thrustAcceleration * 0.01f);
    }
    
    // 3. 공기 저항 (속도에 비례, 반대 방향)
    glm::vec3 dragAccel = -velocity * airDrag;
    
    // 4. 총 가속도 계산
    acceleration = gravityAccel + thrustAccel + dragAccel;
    
    // 5. 속도 업데이트 (v = v0 + a * dt)
    velocity += acceleration * deltaTime;
    
    // 6. 최대 속도 제한
    float speed = glm::length(velocity);
    if (speed > maxVelocity) {
        velocity = glm::normalize(velocity) * maxVelocity;
    }
    
    // 7. 위치 업데이트 (p = p0 + v * dt)
    modelPosition += velocity * deltaTime;
    
    // 8. 지면 충돌 처리
    if (modelPosition.y < 0.0f) {
        modelPosition.y = 0.0f;
        // 지면에 닿으면 Y방향 속도를 감쇠
        if (velocity.y < 0.0f) {
            velocity.y = -velocity.y * 0.3f; // 30% 반발
        }
        // 지면 마찰
        velocity.x *= 0.95f;
        velocity.z *= 0.95f;
    }
    
    // *** 회전 가속도 시스템 ***
    
    // 회전 저항 (각속도에 비례)
    glm::vec3 angularDragAccel = -angularVelocity * angularDrag;
    
    // 총 각가속도
    angularAcceleration = angularDragAccel;
    
    // 각속도 업데이트
    angularVelocity += angularAcceleration * deltaTime;
    
    // 최대 각속도 제한
    angularVelocity.x = glm::clamp(angularVelocity.x, -maxAngularVelocity, maxAngularVelocity);
    angularVelocity.y = glm::clamp(angularVelocity.y, -maxAngularVelocity, maxAngularVelocity);
    angularVelocity.z = glm::clamp(angularVelocity.z, -maxAngularVelocity, maxAngularVelocity);
    
    // 회전 각도 업데이트 (부드러운 회전)
    xModelRotation += angularVelocity.x * deltaTime;
    yModelRotation += angularVelocity.y * deltaTime;
    zModelRotation += angularVelocity.z * deltaTime;
    
    // 각도 정규화 (-180 ~ 180)
    while (xModelRotation > 180.0f) xModelRotation -= 360.0f;
    while (xModelRotation < -180.0f) xModelRotation += 360.0f;
    while (yModelRotation > 180.0f) yModelRotation -= 360.0f;
    while (yModelRotation < -180.0f) yModelRotation += 360.0f;
    while (zModelRotation > 180.0f) zModelRotation -= 360.0f;
    while (zModelRotation < -180.0f) zModelRotation += 360.0f;

    // 각도 보간 (기존 코드 유지 - 부드러운 카메라 전환용)
    cameraAngle = glm::mix(cameraAngle, targetCameraXAngle, interpSpeed * deltaTime);
    cameraYAngle = glm::mix(cameraYAngle, targetCameraYAngle, interpSpeed * deltaTime);

    glutPostRedisplay();
    glutTimerFunc(targetFrameDelay, Timer, 0);
}
void KeyBoard()
{
    // *** 가속도 기반 입력 시스템 ***
    
    // W 키: 추력 증가 (가속)
    if (Input::GetKey(eKeyCode::W))
    {
        thrust += 0.5f;
        if (thrust > 100.0f) thrust = 100.0f;  // 최대 추력 제한
        thrustUp = true;    
    }
    if (Input::GetKeyUp(eKeyCode::W))
    {
        // 키를 떼면 추력 서서히 감소 (관성)
        // thrust는 Timer에서 처리되도록 유지
    }

    // S 키: 추력 감소 (하강)
    if (Input::GetKey(eKeyCode::S))
    {
        thrust -= 0.5f;
        if (thrust < 0.0f) thrust = 0.0f;
        thrustUp = (thrust > 0.0f);
    }
    if (Input::GetKeyUp(eKeyCode::S))
    {
        // 키를 떼면 현재 추력 유지
    }

    // A 키: 좌회전 (각가속도 적용)
    if (Input::GetKey(eKeyCode::A))
    {
        // Y축 회전 각속도 증가 (왼쪽 회전)
        angularVelocity.y += 100.0f * Time::DeltaTime();
        if (angularVelocity.y > maxAngularVelocity) 
            angularVelocity.y = maxAngularVelocity;
    }

    // D 키: 우회전 (각가속도 적용)
    if (Input::GetKey(eKeyCode::D))
    {
        // Y축 회전 각속도 감소 (오른쪽 회전)
        angularVelocity.y -= 100.0f * Time::DeltaTime();
        if (angularVelocity.y < -maxAngularVelocity) 
            angularVelocity.y = -maxAngularVelocity;
    }

    // Q 키: 좌측 롤 (각가속도 적용)
    if (Input::GetKey(eKeyCode::Q))
    {
        // X축 회전 각속도 감소 (왼쪽 기울임)
        angularVelocity.x -= 50.0f * Time::DeltaTime();
        if (angularVelocity.x < -maxAngularVelocity) 
            angularVelocity.x = -maxAngularVelocity;
    }

    // E 키: 우측 롤 (각가속도 적용)
    if (Input::GetKey(eKeyCode::E))
    {
        // X축 회전 각속도 증가 (오른쪽 기울임)
        angularVelocity.x += 50.0f * Time::DeltaTime();
        if (angularVelocity.x > maxAngularVelocity) 
            angularVelocity.x = maxAngularVelocity;
    }

    // ESC 키: 프로그램 종료
    if (Input::GetKey(eKeyCode::ESC))
    {
        exit(0);
    }
}
void Mouse(int button, int state, int x, int y) {
    ImGui_ImplGLUT_MouseFunc(button, state, x, y);

    // ImGui가 마우스를 사용하지 않을 때만 사용자의 마우스 로직 처리
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse)
    {
     
    }
}

void Motion(int x, int y) {
    ImGui_ImplGLUT_MotionFunc(x, y);
    // ImGui가 마우스를 사용하지 않을 때만 사용자의 마우스 로직 처리
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse)
    {
        // 화면 중앙 좌표 계산
        int centerX = width / 2;
        int centerY = height / 2;

        // 중앙으로부터의 마우스 이동량 계산
        int deltaX = x - centerX;
        int deltaY = y - centerY;

        // *** 가속도 기반 회전 입력 ***
        // 마우스 좌우 움직임 -> Y축 각속도 변경 (헬기가 좌우로 회전)
        float yawInput = (float)deltaX * rotationSpeed * 10.0f;
        angularVelocity.y += yawInput;
        angularVelocity.y = glm::clamp(angularVelocity.y, -maxAngularVelocity, maxAngularVelocity);

        // 마우스 앞뒤 움직임 -> Z축 각속도 변경 (헬기가 앞뒤로 기울어짐)
        float pitchInput = (float)deltaY * rotationSpeed * 10.0f;
        angularVelocity.z += pitchInput;
        angularVelocity.z = glm::clamp(angularVelocity.z, -maxAngularVelocity, maxAngularVelocity);

        // 마우스를 화면 중앙으로 다시 이동
        glutWarpPointer(centerX, centerY);

        // 변경 사항을 반영하기 위해 화면 다시 그리기 요청
        glutPostRedisplay();
    }
}

void MakeVertexShaders() {
    GLchar* vertexSource = filetobuf("vertex3D.glsl");
    if (!vertexSource) {
        std::cerr << "ERROR: vertex3D.glsl 파일을 읽지 못했습니다." << std::endl;
        return;
    }

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, NULL);
    glCompileShader(vertexShader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
    }
    free(vertexSource);
}

void MakeFragmentShaders() {
    GLchar* fragmentSource = filetobuf("fragment.glsl");
    if (!fragmentSource) {
        std::cerr << "ERROR: fragment.glsl 파일을 읽지 못했습니다." << std::endl;
        return;
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, NULL);
    glCompileShader(fragmentShader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
    }
    free(fragmentSource);
}

GLuint MakeShaderProgram() {
    GLuint shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexShader);
    glAttachShader(shaderID, fragmentShader);
    glLinkProgram(shaderID);

    GLint result;
    GLchar errorLog[512];
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
        std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glUseProgram(shaderID);
    return shaderID;
}


GLuint loadCubemap(std::vector<std::string> faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        // 큐브맵은 y축이 뒤집히지 않으므로 stbi_set_flip_vertically_on_load(false)
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            // 텍스처 타겟: GL_TEXTURE_CUBE_MAP_POSITIVE_X + i
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
            std::cout << "큐브맵 텍스처 로드 성공: " << faces[i] << std::endl;
        }
        else
        {
            std::cerr << "큐브막 텍스처 로드 실패: " << faces[i] << std::endl;
            stbi_image_free(data);
            glDeleteTextures(1, &textureID); // 실패 시 생성된 텍스처 삭제
            return 0; // 0 반환하여 실패 알림
        }
    }

    // 텍스처 파라미터 설정
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    stbi_set_flip_vertically_on_load(true); // 다른 텍스처 로드를 위해 복원 (선택적)
    return textureID;
}


void MakeSkyboxVertexShader() {
    GLchar* vertexSource = filetobuf("skyboxVertex.glsl"); // 1단계에서 만든 파일
    if (!vertexSource) {
        std::cerr << "ERROR: skyboxVertex.glsl 파일을 읽지 못했습니다." << std::endl;
        return;
    }

    vertexSkyboxShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexSkyboxShader, 1, (const GLchar**)&vertexSource, NULL);
    glCompileShader(vertexSkyboxShader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexSkyboxShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(vertexSkyboxShader, 512, NULL, errorLog);
        std::cerr << "ERROR: skybox vertex shader 컴파일 실패\n" << errorLog << std::endl;
    }
    free(vertexSource);
}

void MakeSkyboxFragmentShader() {
    GLchar* fragmentSource = filetobuf("skyboxFrag.glsl"); // 1단계에서 만든 파일
    if (!fragmentSource) {
        std::cerr << "ERROR: skybox.frag 파일을 읽지 못했습니다." << std::endl;
        return;
    }

    fragmentSkyboxShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentSkyboxShader, 1, (const GLchar**)&fragmentSource, NULL);
    glCompileShader(fragmentSkyboxShader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentSkyboxShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(fragmentSkyboxShader, 512, NULL, errorLog);
        std::cerr << "ERROR: skybox fragment shader 컴파일 실패\n" << errorLog << std::endl;
    }
    free(fragmentSource);
}

GLuint MakeSkyboxShaderProgram() {
    GLuint shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexSkyboxShader);
    glAttachShader(shaderID, fragmentSkyboxShader);
    glLinkProgram(shaderID);

    GLint result;
    GLchar errorLog[512];
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
        std::cerr << "ERROR: skybox shader program 연결 실패\n" << errorLog << std::endl;
        return 0;
    }

    // 스카이박스 셰이더는 삭제하지 않고 유지 (vertexShader, fragmentShader는 삭제됨)
    // glDeleteShader(vertexSkyboxShader); 
    // glDeleteShader(fragmentSkyboxShader);
    // glUseProgram(shaderID); // 바로 사용하지 않음
    return shaderID;
}


void InitBuffers() {  // 변경: 모델별 버퍼 초기화
    
    
    // 스카이박스 버텍스 (위치만 필요)
    float skyboxVertices[] = {
        // positions          
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

    // layout (location = 0) in vec3 aPos;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    // --- Body 버퍼 ---
    glGenVertexArrays(1, &VAO_Body);
    glBindVertexArray(VAO_Body);

    glGenBuffers(1, &VBO_Body);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Body);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO_Body);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Body);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

    const GLsizei stride = 8 * sizeof(GLfloat);  // *** 8로 변경 ***
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);  // pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));  // color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(GLfloat)));  // *** UV 추가 (인덱스 2) ***
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // --- Blade 버퍼 ---
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
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(GLfloat)));  // *** UV 추가 (인덱스 2) ***
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // --- Tail 버퍼 ---
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
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(GLfloat)));  // *** UV 추가 (인덱스 2) ***
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // --- 축 데이터 (기존 그대로) ---
    std::vector<GLfloat> axesData = {
        -200.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // X red
        200.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, -200.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // Y blue
        0.0f, 200.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, -200.0f, 0.0f, 1.0f, 0.0f, // Z green
        0.0f, 0.0f, 200.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &axisVAO);
    glBindVertexArray(axisVAO);

    glGenBuffers(1, &axisVBO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, axesData.size() * sizeof(GLfloat), axesData.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}