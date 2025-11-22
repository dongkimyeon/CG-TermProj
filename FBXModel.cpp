#include "FBXModel.h"

void UpdateModelBuffers(FBXModel* model, GLuint vao, GLuint vbo, GLuint ebo)
{
    if (!model->loaded || model->vertices.empty() || model->indices.empty()) {
        std::cerr << "모델 버퍼 업데이트 실패: 로드된 데이터 없음" << std::endl;
        return;
    }

    // 노멀 계산
    std::vector<glm::vec3> normals(model->vertices.size(), glm::vec3(0.0f));
    
    // 각 삼각형에 대해 노멀 계산
    for (size_t i = 0; i < model->indices.size(); i += 3) {
        GLuint i0 = model->indices[i];
        GLuint i1 = model->indices[i + 1];
        GLuint i2 = model->indices[i + 2];
        
        glm::vec3 v0 = model->vertices[i0] - model->center;
        glm::vec3 v1 = model->vertices[i1] - model->center;
        glm::vec3 v2 = model->vertices[i2] - model->center;
        
        glm::vec3 deltaPos1 = v1 - v0;
        glm::vec3 deltaPos2 = v2 - v0;
        
        // 노멀 계산
        glm::vec3 normal = glm::normalize(glm::cross(deltaPos1, deltaPos2));
        normals[i0] += normal;
        normals[i1] += normal;
        normals[i2] += normal;
    }
    
    // 노멀 정규화
    for (size_t i = 0; i < normals.size(); ++i) {
        if (glm::length(normals[i]) > 0.0001f)
            normals[i] = glm::normalize(normals[i]);
    }

    // 인덱스 기반의 정점 데이터로 변환하여 탄젠트 계산
    std::vector<glm::vec3> expandedVertices;
    std::vector<glm::vec2> expandedUVs;
    std::vector<glm::vec3> expandedNormals;
    std::vector<glm::vec3> tangents;
    
    expandedVertices.reserve(model->indices.size());
    expandedUVs.reserve(model->indices.size());
    expandedNormals.reserve(model->indices.size());
    
    for (size_t i = 0; i < model->indices.size(); ++i) {
        GLuint idx = model->indices[i];
        expandedVertices.push_back(model->vertices[idx] - model->center);
        expandedUVs.push_back(model->uvs[idx]);
        expandedNormals.push_back(normals[idx]);
    }
    
    // getTangent 함수를 사용하여 탄젠트 계산
    getTangent(expandedVertices, expandedUVs, expandedNormals, tangents);
    
    // 인덱스 기반으로 다시 정리 (평균화)
    std::vector<glm::vec3> vertexTangents(model->vertices.size(), glm::vec3(0.0f));
    std::vector<int> tangentCounts(model->vertices.size(), 0);
    
    for (size_t i = 0; i < model->indices.size(); ++i) {
        GLuint idx = model->indices[i];
        if (i < tangents.size() && glm::length(tangents[i]) > 0.0001f) {
            vertexTangents[idx] += tangents[i];
            tangentCounts[idx]++;
        }
    }
    
    // 탄젠트 평균 및 정규화
    for (size_t i = 0; i < vertexTangents.size(); ++i) {
        if (tangentCounts[i] > 0) {
            vertexTangents[i] /= static_cast<float>(tangentCounts[i]);
            if (glm::length(vertexTangents[i]) > 0.0001f) {
                vertexTangents[i] = glm::normalize(vertexTangents[i]);
            }
        }
    }

    // 버텍스 데이터: 위치(3) + UV(2) + 노멀(3) + 탄젠트(3) = 11 floats
    std::vector<GLfloat> vertexData;
    vertexData.reserve(model->vertices.size() * 11);

    for (size_t i = 0; i < model->vertices.size(); ++i) {
        glm::vec3 pos = model->vertices[i] - model->center;
        vertexData.insert(vertexData.end(), { pos.x, pos.y, pos.z });
        vertexData.insert(vertexData.end(), { model->uvs[i].x, model->uvs[i].y });
        vertexData.insert(vertexData.end(), { normals[i].x, normals[i].y, normals[i].z });
        vertexData.insert(vertexData.end(), { vertexTangents[i].x, vertexTangents[i].y, vertexTangents[i].z });
    }

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat), vertexData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->indices.size() * sizeof(GLuint), model->indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    std::cout << "모델 버퍼가 GPU에 업로드되었습니다. (정점 " << model->vertices.size() << "개, 탄젠트 적용)" << std::endl;
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
    fbxModel->meshes.clear();
    fbxModel->loaded = false;

    std::cout << "\n=== 파일: " << filename << " ===" << std::endl;
    std::cout << "머티리얼 개수: " << scene->mNumMaterials << std::endl;

    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* material = scene->mMaterials[i];
        aiString matName;
        material->Get(AI_MATKEY_NAME, matName);

        std::cout << "\n[Material " << i << "]" << std::endl;
        std::cout << "  이름: " << matName.C_Str() << std::endl;
     
    }
    std::cout << "====================\n" << std::endl;

    // 메쉬 처리
    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[m];
        GLuint vertexOffset = static_cast<GLuint>(fbxModel->vertices.size());
        GLuint indexStart = static_cast<GLuint>(fbxModel->indices.size());

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            glm::vec3 pos(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

            // 좌표계 변환: Y-up -> Z-up
            glm::mat4 coordTransform = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            pos = glm::vec3(coordTransform * glm::vec4(pos, 1.0f));
            fbxModel->vertices.push_back(pos);

            glm::vec2 uv(0.0f, 0.0f);
            if (mesh->HasTextureCoords(0)) {
                uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }
            fbxModel->uvs.push_back(uv);

            glm::vec3 colors[4] = { {1,0,0}, {0,1,0}, {0,0,1}, {1,1,0} };
            fbxModel->colors.push_back(colors[(vertexOffset + i) % 4]);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                fbxModel->indices.push_back(face.mIndices[j] + vertexOffset);
            }
        }

        FBXModel::MeshInfo meshInfo;
        meshInfo.indexStart = indexStart;
        meshInfo.indexCount = static_cast<GLuint>(fbxModel->indices.size() - indexStart);
        meshInfo.materialIndex = mesh->mMaterialIndex;
        fbxModel->meshes.push_back(meshInfo);

        std::cout << "Mesh " << m << ": Material Index = " << mesh->mMaterialIndex
            << ", Indices = " << meshInfo.indexCount << std::endl;
    }

    fbxModel->center = CalculateModelCenter(fbxModel->vertices);
    fbxModel->loaded = true;
    std::cout << "FBX 로드 완료: " << scene->mNumMeshes << " meshes combined." << std::endl;
    std::cout << "Total Vertices: " << fbxModel->vertices.size() << ", Total Indices: " << fbxModel->indices.size() << std::endl;


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

void getTangent(std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals, std::vector<glm::vec3>& tangents)
{
    for (unsigned int i = 0; i < vertices.size(); i += 3)
    {
        glm::vec3& v0 = vertices[i + 0];
        glm::vec3& v1 = vertices[i + 1];
        glm::vec3& v2 = vertices[i + 2];

        glm::vec2& uv0 = uvs[i + 0];
        glm::vec2& uv1 = uvs[i + 1];
        glm::vec2& uv2 = uvs[i + 2];

        glm::vec3 deltaPos1 = v1 - v0;
        glm::vec3 deltaPos2 = v2 - v0;

        glm::vec2 deltaUV1 = uv1 - uv0;
        glm::vec2 deltaUV2 = uv2 - uv0;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;

        tangents.push_back(tangent);
        tangents.push_back(tangent);
        tangents.push_back(tangent);
    }
}
