#include "ParticleSystem.h"

#include <algorithm>
#include <iostream>

ParticleSystem::ParticleSystem(size_t maxCount)
    : maxParticles(maxCount) {
    particles.reserve(maxParticles);
    instanceData.reserve(maxParticles);
}

void ParticleSystem::initialize() {
    setupCubeGeometry();
    // 인스턴스 버퍼
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glVertexAttribDivisor(4, 1);
    glBindVertexArray(0);

    // 확장자 원래 형태(.vert/.frag)로 복구
    particleShaderProgram = LoadShaders("Shaders/particle_instanced.vert", "Shaders/particle_instanced.frag");
    if (!particleShaderProgram) {
        std::cerr << "Particle shader load failed" << std::endl;
    }
}

void ParticleSystem::setupCubeGeometry() {
    if (cubeVAO) return;
    // 단위 큐브 (중심 기준 약간 작게)
    const float s = 0.5f;
    GLfloat verts[] = {
        // pos              // uv(더미) // normal(더미) // tangent(더미)
        -s,-s,-s, 0,0, 0,0,-1, 1,0,0,
         s,-s,-s, 0,0, 0,0,-1, 1,0,0,
         s, s,-s, 0,0, 0,0,-1, 1,0,0,
        -s, s,-s, 0,0, 0,0,-1, 1,0,0,
        -s,-s, s, 0,0, 0,0, 1, 1,0,0,
         s,-s, s, 0,0, 0,0, 1, 1,0,0,
         s, s, s, 0,0, 0,0, 1, 1,0,0,
        -s, s, s, 0,0, 0,0, 1, 1,0,0
    };
    GLuint idx[] = {
        0,1,2, 2,3,0, // back
        4,5,6, 6,7,4, // front
        0,4,7, 7,3,0, // left
        1,5,6, 6,2,1, // right
        3,2,6, 6,7,3, // top
        0,1,5, 5,4,0  // bottom
    };
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glGenBuffers(1, &cubeEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

    GLsizei stride = 11 * sizeof(GLfloat);
    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1); // uv (미사용)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2); // normal (더미)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3); // tangent (더미)
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(GLfloat)));

    // 색상/인스턴스용 4번은 initialize에서 생성
    glBindVertexArray(0);
}

void ParticleSystem::emitParticle(const glm::vec3& pos, const glm::vec3& vel) {
    if (particles.size() >= maxParticles) return;
    Particle p;
    // 수명을 6.0f에서 18.0f로 증가 (3배 더 증가)
    // 초기 크기를 0.8f에서 2.4f로 증가 (3배 더 크게)
    p.initialize(pos, vel, glm::vec4(0.9f, 0.9f, 0.9f, 0.9f), 18.0f, 2.4f, 0.0f);
    particles.push_back(p);
}

void ParticleSystem::update(float dt) {
    for (auto& p : particles) p.update(dt);
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const Particle& p) { return p.isDead(); }),
        particles.end());
}

void ParticleSystem::updateInstanceBuffer() {
    instanceData.clear();
    instanceData.reserve(particles.size());
    for (auto& p : particles)
        instanceData.emplace_back(p.pos.x, p.pos.y, p.pos.z, p.size);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    if (!instanceData.empty())
        glBufferSubData(GL_ARRAY_BUFFER, 0, instanceData.size() * sizeof(glm::vec4), instanceData.data());
}

void ParticleSystem::render(const glm::mat4& view, const glm::mat4& proj) {
    if (!particleShaderProgram || particles.empty()) return;
    updateInstanceBuffer();
    glUseProgram(particleShaderProgram);
    GLint viewLoc = glGetUniformLocation(particleShaderProgram, "uView");
    GLint projLoc = glGetUniformLocation(particleShaderProgram, "uProj");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);

    glBindVertexArray(cubeVAO);
    glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0,
        static_cast<GLsizei>(instanceData.size()));
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void ParticleSystem::clear() {
    particles.clear();
}