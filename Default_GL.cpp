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
#include "SoundManager.h"
#include "ParticleManager.h"
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

// 좌클릭(기관포 지속 발사) 관련
int lastCannonFireTimeMs = 0; // 마지막 발사 시각(밀리초)
const int CANNON_FIRE_INTERVAL_MS = 300; //0.3초

//imgui 관련 변수
float modelScale = 0.1f;
int targetFrameDelay = 1;
bool wireframeMode = false;
float glassAlpha = 0.5f;
float crosshairSize = 22.0f;
float crosshairDistance = 315.0f;


//디버그 회전 
float xModelRotation = 0.0f;
float yModelRotation = 0.0f;
float zModelRotation = 0.0f;
float currentScale = 1.0f;
int main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);

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

	// SoundManager 초기화
	SoundManager::GetInstance()->Initialize();	

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

	crosshairShaderProgramID = ShaderManager::CreateShaderProgram(
		"crosshair_vertex.glsl", "crosshair_fragment.glsl");
	if (crosshairShaderProgramID == 0) {
		std::cerr << "크로스헤어 셰이더 로드 실패" << std::endl;
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

	// Ensure persistent particle system exists for sharing trails from short-lived systems
	EnsurePersistentParticles();

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



	return 0;
}

void WhellFunc(int wheel, int dir, int x, int y)
{
	if (camera) {
		camera->Zoom(dir);
		glutPostRedisplay(); // 화면 갱신
	}
}

// DrawScene() 함수 전체 (수정된 최종 버전)

GLvoid DrawScene()
{
	Time::Update();

	// Update persistent particles first
	UpdatePersistentParticles(Time::DeltaTime());

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
			helicopter->GetPitch(),
			helicopter->GetRoll(),
			helicopter->GetCannonWorldPosition());
	}

	// =====1단계: 프레임버퍼에 씬 렌더링 =====
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// 기본3D 셰이더 사용
	glUseProgram(shaderProgramID);

	glm::mat4 view = camera->GetViewMatrix();
	glm::mat4 proj = camera->GetProjectionMatrix((float)width / (float)height);

	// 공통 Uniform 설정
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
	glUniform3fv(glGetUniformLocation(shaderProgramID, "eyePos"), 1, glm::value_ptr(camera->GetPosition()));
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightDir"), 1.0f, -1.0f, -1.0f);
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightColor"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(shaderProgramID, "ambientStrength"), 0.5f);
	glUniform1f(glGetUniformLocation(shaderProgramID, "specularStrength"), 0.5f);
	glUniform1f(glGetUniformLocation(shaderProgramID, "shininess"), 32.0f);
	glUniform1f(glGetUniformLocation(shaderProgramID, "useTexture"), 1.0f);

	//1. 그라운드
	if (mGround) {
		glDisable(GL_BLEND);
		mGround->Render(shaderProgramID, view, proj);
		glEnable(GL_BLEND);
	}

	//2. 헬리콥터
	if (helicopter) {
		helicopter->Render(shaderProgramID, wireframeMode, glassAlpha, modelScale);
		helicopter->RenderMissiles(shaderProgramID, view, proj);
		helicopter->RenderCannonBullets(view, proj);
		// 기관포 탄환(스모크 트레일)은 Render에서 처리됨
	}

	//3. AA 유닛들
	if (aaUnits) {
		for (int i = 0; i < NUM_AA_UNITS; ++i) {
			if (aaUnits[i]) {
				aaUnits[i]->Render(shaderProgramID, wireframeMode, glassAlpha, modelScale);
			}
		}
	}

	// Render persistent/shared particles (trails transferred from short-lived systems)
	RenderPersistentParticles(view, proj);


	// =====5. 스카이박스 =====
	glDepthFunc(GL_LEQUAL);
	glUseProgram(skyboxShaderProgramID);

	// 카메라 회전만 반영 (위치 이동은 없앰)
	glm::mat4 skyView = glm::mat4(glm::mat3(view));

	glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(skyView));
	glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(proj));

	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glUniform1i(glGetUniformLocation(skyboxShaderProgramID, "skybox"), 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS); // 복구


	//4. 크로스헤어 (3D 공간에 그리는 십자선)
	if (camera && (camera->GetCameraMode() == 0 || camera->GetCameraMode() == 1) && helicopter) {
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		// 헬리콥터 방향 계산
		glm::vec3 heliPos = helicopter->GetPosition();
		float yaw = helicopter->GetYaw();
		float pitch = helicopter->GetPitch();
		float roll = helicopter->GetRoll();

		glm::mat4 heliTransform = glm::mat4(1.0f);
		heliTransform = glm::translate(heliTransform, heliPos);
		heliTransform = glm::rotate(heliTransform, glm::radians(yaw), glm::vec3(0, 1, 0));
		heliTransform = glm::rotate(heliTransform, glm::radians(roll), glm::vec3(1, 0, 0));
		heliTransform = glm::rotate(heliTransform, glm::radians(pitch), glm::vec3(0, 0, 1));

		glm::vec3 forward = glm::normalize(glm::vec3(heliTransform * glm::vec4(-1, 0, 0, 0)));
		glm::vec3 up = glm::normalize(glm::vec3(heliTransform * glm::vec4(0, 1, 0, 0)));
		glm::vec3 right = glm::normalize(glm::vec3(heliTransform * glm::vec4(0, 0, 1, 0)));

		glm::vec3 center = heliPos - forward * crosshairDistance;

		std::vector<glm::vec3> lines = {
		center - right * crosshairSize * 0.5f, center - right * crosshairSize * 0.15f,
		center + right * crosshairSize * 0.15f, center + right * crosshairSize * 0.5f,
		center - up * crosshairSize * 0.5f, center - up * crosshairSize * 0.15f,
		center + up * crosshairSize * 0.15f, center + up * crosshairSize * 0.5f
		};

		GLuint vao, vbo;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(glm::vec3), lines.data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glUseProgram(crosshairShaderProgramID);
		glUniformMatrix4fv(glGetUniformLocation(crosshairShaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glUniformMatrix4fv(glGetUniformLocation(crosshairShaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(crosshairShaderProgramID, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
		glUniform3f(glGetUniformLocation(crosshairShaderProgramID, "crosshairColor"), 0.0f, 1.0f, 0.0f);
		glLineWidth(3.0f);
		glDrawArrays(GL_LINES, 0, (GLsizei)lines.size());

		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
		glEnable(GL_DEPTH_TEST);
	}


	// =====2단계: 후처리 =====
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(postprocessShaderID);
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

	bool nightVisionOn = enableNightVision && camera && camera->GetCameraMode() == 2;
	glUniform1i(glGetUniformLocation(postprocessShaderID, "enableNightVision"), nightVisionOn);
	bool gunnerView = camera && camera->GetCameraMode() == 2;
	glUniform1i(glGetUniformLocation(postprocessShaderID, "gunnerview"), gunnerView);
	glUniform1f(glGetUniformLocation(postprocessShaderID, "time"), (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// =====3단계: ImGui =====
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGLUT_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(
		ImVec2((float)width - 10.0f, 10.0f),  // 오른쪽 위에서 약간 안쪽
		ImGuiCond_FirstUseEver,
		ImVec2(1.0f, 0.0f)   // pivot은 유지 (오른쪽 위 기준)
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


	if (Input::GetKeyDown(eKeyCode::F) && helicopter && camera) {
		helicopter->FireMissileFromCamera(camera, crosshairDistance);
	}

	// 좌클릭을 누르고 있으면0.5초 간격으로 기관포 발사
	if (rightClickDown && helicopter) {
		int nowMs = glutGet(GLUT_ELAPSED_TIME);
		if (lastCannonFireTimeMs == 0) {
			// 초기값이0이면 즉시 발사하고 타이머 설정
			if (camera && camera->GetCameraMode() != 2) {
				glm::vec3 heliPos = helicopter->GetPosition();
				float yaw = helicopter->GetYaw();
				float pitch = helicopter->GetPitch();
				float roll = helicopter->GetRoll();

				glm::mat4 heliTransform = glm::mat4(1.0f);
				heliTransform = glm::translate(heliTransform, heliPos);
				heliTransform = glm::rotate(heliTransform, glm::radians(yaw), glm::vec3(0, 1, 0));
				heliTransform = glm::rotate(heliTransform, glm::radians(roll), glm::vec3(1, 0, 0));
				heliTransform = glm::rotate(heliTransform, glm::radians(pitch), glm::vec3(0, 0, 1));

				glm::vec3 forward = glm::normalize(glm::vec3(heliTransform * glm::vec4(-1, 0, 0, 0)));
				glm::vec3 center = heliPos - forward * crosshairDistance;

				glm::vec3 cannonPos = helicopter->GetCannonWorldPosition();
				glm::vec3 desiredDir = glm::normalize(center - cannonPos);

				glm::mat4 cannonOrient = heliTransform;
				cannonOrient = glm::rotate(cannonOrient, glm::radians(helicopter->GetCannonYaw()), glm::vec3(0, 1, 0));
				cannonOrient = glm::rotate(cannonOrient, glm::radians(helicopter->GetCannonPitch()), glm::vec3(0, 0, 1));
				glm::vec3 cannonForward = glm::normalize(glm::vec3(cannonOrient * glm::vec4(-1, 0, 0, 0)));

				// If desired direction points opposite to cannon forward, use cannonForward to keep consistent forward firing
				if (glm::dot(desiredDir, cannonForward) < 0.0f) {
					desiredDir = cannonForward;
				}

				helicopter->FireCannon(desiredDir);
			}
			else {
				helicopter->FireCannon();
			}

			// 마지막 발사 시각 초기화하여 타이머와 간격을 맞춤
			lastCannonFireTimeMs = glutGet(GLUT_ELAPSED_TIME);
		}
		else if (nowMs - lastCannonFireTimeMs >= CANNON_FIRE_INTERVAL_MS) {
			if (camera && camera->GetCameraMode() != 2) {
				glm::vec3 heliPos = helicopter->GetPosition();
				float yaw = helicopter->GetYaw();
				float pitch = helicopter->GetPitch();
				float roll = helicopter->GetRoll();

				glm::mat4 heliTransform = glm::mat4(1.0f);
				heliTransform = glm::translate(heliTransform, heliPos);
				heliTransform = glm::rotate(heliTransform, glm::radians(yaw), glm::vec3(0, 1, 0));
				heliTransform = glm::rotate(heliTransform, glm::radians(roll), glm::vec3(1, 0, 0));
				heliTransform = glm::rotate(heliTransform, glm::radians(pitch), glm::vec3(0, 0, 1));

				glm::vec3 forward = glm::normalize(glm::vec3(heliTransform * glm::vec4(-1, 0, 0, 0)));
				glm::vec3 center = heliPos - forward * crosshairDistance;

				glm::vec3 cannonPos = helicopter->GetCannonWorldPosition();
				glm::vec3 desiredDir = glm::normalize(center - cannonPos);

				glm::mat4 cannonOrient = heliTransform;
				cannonOrient = glm::rotate(cannonOrient, glm::radians(helicopter->GetCannonYaw()), glm::vec3(0, 1, 0));
				cannonOrient = glm::rotate(cannonOrient, glm::radians(helicopter->GetCannonPitch()), glm::vec3(0, 0, 1));
				glm::vec3 cannonForward = glm::normalize(glm::vec3(cannonOrient * glm::vec4(-1, 0, 0, 0)));

				if (glm::dot(desiredDir, cannonForward) < 0.0f) {
					desiredDir = cannonForward;
				}

				helicopter->FireCannon(desiredDir);
			}
			else {
				helicopter->FireCannon();
			}
			lastCannonFireTimeMs = nowMs;
		}
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
				// 기관포 발사
				if (helicopter) {
					if (camera && camera->GetCameraMode() != 2) {
						glm::vec3 heliPos = helicopter->GetPosition();
						float yaw = helicopter->GetYaw();
						float pitch = helicopter->GetPitch();
						float roll = helicopter->GetRoll();

						glm::mat4 heliTransform = glm::mat4(1.0f);
						heliTransform = glm::translate(heliTransform, heliPos);
						heliTransform = glm::rotate(heliTransform, glm::radians(yaw), glm::vec3(0, 1, 0));
						heliTransform = glm::rotate(heliTransform, glm::radians(roll), glm::vec3(1, 0, 0));
						heliTransform = glm::rotate(heliTransform, glm::radians(pitch), glm::vec3(0, 0, 1));

						glm::vec3 forward = glm::normalize(glm::vec3(heliTransform * glm::vec4(-1, 0, 0, 0)));
						glm::vec3 center = heliPos - forward * crosshairDistance;

						glm::vec3 cannonPos = helicopter->GetCannonWorldPosition();
						glm::vec3 desiredDir = glm::normalize(center - cannonPos);

						glm::mat4 cannonOrient = heliTransform;
						cannonOrient = glm::rotate(cannonOrient, glm::radians(helicopter->GetCannonYaw()), glm::vec3(0, 1, 0));
						cannonOrient = glm::rotate(cannonOrient, glm::radians(helicopter->GetCannonPitch()), glm::vec3(0, 0, 1));
						glm::vec3 cannonForward = glm::normalize(glm::vec3(cannonOrient * glm::vec4(-1, 0, 0, 0)));

						// If desired direction points opposite to cannon forward, use cannonForward to keep consistent forward firing
						if (glm::dot(desiredDir, cannonForward) < 0.0f) {
							desiredDir = cannonForward;
						}

						helicopter->FireCannon(desiredDir);
					}
					else {
						helicopter->FireCannon();
					}

					lastCannonFireTimeMs = glutGet(GLUT_ELAPSED_TIME);
				}
			}
			else if (state == GLUT_UP) {
				rightClickDown = false;
				lastCannonFireTimeMs = 0;
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
		// 위치 // 텍스처 좌표
		-1.0f,1.0f,0.0f,1.0f,
		-1.0f, -1.0f,0.0f,0.0f,
	   1.0f, -1.0f,1.0f,0.0f,

		-1.0f,1.0f,0.0f,1.0f,
	   1.0f, -1.0f,1.0f,0.0f,
	   1.0f,1.0f,1.0f,1.0f
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
	-1.0f,1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f,1.0f, -1.0f,
	-1.0f,1.0f, -1.0f,

	-1.0f, -1.0f,1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,1.0f, -1.0f,
	-1.0f,1.0f, -1.0f,
	-1.0f,1.0f,1.0f,
	-1.0f, -1.0f,1.0f,

   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f,1.0f,
   1.0f,1.0f,1.0f,
   1.0f,1.0f,1.0f,
   1.0f,1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,1.0f,
	-1.0f,1.0f,1.0f,
   1.0f,1.0f,1.0f,
   1.0f,1.0f,1.0f,
   1.0f, -1.0f,1.0f,
	-1.0f, -1.0f,1.0f,

	-1.0f,1.0f, -1.0f,
   1.0f,1.0f, -1.0f,
   1.0f,1.0f,1.0f,
   1.0f,1.0f,1.0f,
	-1.0f,1.0f,1.0f,
	-1.0f,1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,1.0f,
   1.0f, -1.0f,1.0f
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
		-1.0f,1.0f,
	   1.0f,1.0f,
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
	aaUnits = new AA * [NUM_AA_UNITS];

	AA::LoadSharedModel();

	// 랜덤 생성기 초기화
	std::random_device rd;
	std::mt19937 gen(rd());

	// 맵 크기 기준 (Ground 크기 참고: -5000 ~5000)
	std::uniform_real_distribution<float> distX(-1300.0f, 1300.0f);
	std::uniform_real_distribution<float> distZ(-1300.0f, 1300.0f);

	std::cout << "\n=== AA 유닛 배치 시작 ===" << std::endl;

	for (int i = 0; i < NUM_AA_UNITS; ++i) {
		aaUnits[i] = new AA();
		aaUnits[i]->Initialize();
		aaUnits[i]->InitBuffers(); // LoadModel() 대신 InitBuffers()만 호출

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