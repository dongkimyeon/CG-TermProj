#include "ParticleSystem.h"

#include <algorithm>
#include <iostream>

ParticleSystem::ParticleSystem(size_t maxCount)
    : maxParticles(maxCount) {
    particles.reserve(maxParticles);
    instanceData.reserve(maxParticles);
    instanceColorData.reserve(maxParticles);
}

void ParticleSystem::initialize() {
    setupCubeGeometry();
    // 인스턴스 VBO
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);

    // 인스턴스 컬러 VBO
    glGenBuffers(1, &instanceColorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceColorVBO);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);

    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glVertexAttribDivisor(4, 1);

    // 컬러 속성(location5)
    glEnableVertexAttribArray(5);
    glBindBuffer(GL_ARRAY_BUFFER, instanceColorVBO);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glVertexAttribDivisor(5, 1);

    glBindVertexArray(0);

    // 확장자 원래 형태(.vert/.frag)로 복구
    particleShaderProgram = LoadShaders("Shaders/particle_instanced.vert", "Shaders/particle_instanced.frag");
    if (!particleShaderProgram) {
        std::cerr << "Particle shader load failed" << std::endl;
    }
}

void ParticleSystem::setupCubeGeometry() {
    if (cubeVAO) return;
    // Replace cube with a simple camera-facing quad (centered at origin)
    GLfloat verts[] = {
        // pos(x,y,z) // uv
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
    };
    GLuint idx[] = {
        0, 1, 2, 2, 3, 0
    };
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glGenBuffers(1, &cubeEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

    GLsizei stride = 5 * sizeof(GLfloat);
    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1); // uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));

    // 색상/인스턴스용 4번은 initialize에서 생성
    glBindVertexArray(0);
}

// 기존 emit은 기본 색상/사이즈 사용
void ParticleSystem::emitParticle(const glm::vec3& pos, const glm::vec3& vel) {
    // 기본: 흰색, size20.8f
    emitParticle(pos, vel, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 20.8f);
}

// 새로운 오버로드: 색상 및 크기 지정
void ParticleSystem::emitParticle(const glm::vec3& pos, const glm::vec3& vel, const glm::vec4& color, float particleSize) {
    if (particles.size() >= maxParticles) return;
    Particle p;
    
    p.initialize(pos, vel, color, 18.0f, particleSize, 0.0f);
    particles.push_back(p);
}

// 새 오버로드: 수명 지정 가능
void ParticleSystem::emitParticle(const glm::vec3& pos, const glm::vec3& vel, const glm::vec4& color, float particleSize, float lifeTime) {
    if (particles.size() >= maxParticles) return;
    Particle p;
    p.initialize(pos, vel, color, lifeTime, particleSize, 0.0f);
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
    instanceColorData.clear();
    instanceColorData.reserve(particles.size());
    for (auto& p : particles) {
        instanceData.emplace_back(p.pos.x, p.pos.y, p.pos.z, p.size);
        // use particle color.a for alpha
        instanceColorData.emplace_back(p.color.r, p.color.g, p.color.b, p.color.a);
    }
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    if (!instanceData.empty())
        glBufferSubData(GL_ARRAY_BUFFER, 0, instanceData.size() * sizeof(glm::vec4), instanceData.data());
    glBindBuffer(GL_ARRAY_BUFFER, instanceColorVBO);
    if (!instanceColorData.empty())
        glBufferSubData(GL_ARRAY_BUFFER, 0, instanceColorData.size() * sizeof(glm::vec4), instanceColorData.data());
}

void ParticleSystem::render(const glm::mat4& view, const glm::mat4& proj) {
    if (!particleShaderProgram || particles.empty()) return;
    updateInstanceBuffer();
    glUseProgram(particleShaderProgram);
    GLint viewLoc = glGetUniformLocation(particleShaderProgram, "uView");
    GLint projLoc = glGetUniformLocation(particleShaderProgram, "uProj");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

    // Save GL state
    GLboolean depthTestEnabled;
    GLboolean blendEnabled;
    GLint srcBlend, dstBlend;
    GLboolean depthMask;

    glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
    glGetBooleanv(GL_BLEND, &blendEnabled);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &srcBlend);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &dstBlend);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);

    //glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glBindVertexArray(cubeVAO);
    // quad has 6 indices
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0,
        static_cast<GLsizei>(instanceData.size()));
    glBindVertexArray(0);

    // Restore GL state
    if (depthTestEnabled) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (!blendEnabled) glDisable(GL_BLEND); else glBlendFunc(srcBlend, dstBlend);
    glDepthMask(depthMask);
}

void ParticleSystem::clear() {
    // Immediately remove all particles
    particles.clear();
}

std::vector<Particle> ParticleSystem::stealParticles() {
    // Move particles out to caller
    std::vector<Particle> moved;
    moved.reserve(particles.size());
    for (auto &p : particles) moved.push_back(std::move(p));
    particles.clear();
    return moved;
}

void ParticleSystem::addParticles(std::vector<Particle>&& newParticles) {
    // append up to capacity
    size_t space = (maxParticles > particles.size()) ? (maxParticles - particles.size()) :0;
    size_t toAdd = std::min(space, newParticles.size());
    for (size_t i =0; i < toAdd; ++i) particles.push_back(std::move(newParticles[i]));
}

bool ParticleSystem::hasLiveParticles() const {
    return !particles.empty();
}