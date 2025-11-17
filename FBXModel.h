#pragma once
#include "CommonInclude.h"
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
    Texture* normalMap = nullptr; // 노말 맵 텍스처 포인터
    glm::vec3 center;
    bool loaded = false;
};

// FBX 관련 함수 선언
bool LoadFBX(const char* filename, FBXModel* model);
glm::vec3 CalculateModelCenter(const std::vector<glm::vec3>& vertices);
void UpdateModelBuffers(FBXModel* model, GLuint vao, GLuint vbo, GLuint ebo);
void getTangent(std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, 
                std::vector<glm::vec3>& normals, std::vector<glm::vec3>& tangents);
