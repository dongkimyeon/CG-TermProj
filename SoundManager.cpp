#include "SoundManager.h"

#pragma comment(lib, "fmod_vc.lib")
#pragma comment(lib, "fmodL_vc.lib")

#define MAX_SE_SOUND 20
#define MAX_BGM_SOUND 10

void SoundManager::Initialize() {
    // Create FMOD system

    mBGMVolume = 0.15f;
    mSEVolume = 0.1f;
    FMOD_RESULT result = FMOD::System_Create(&mSystem);
    if (result != FMOD_OK) return;
    result = mSystem->init(128, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK) return;

    // Create channel groups
    result = mSystem->createChannelGroup("SEGroup", &mSEGroup);
    if (result != FMOD_OK) return;
    result = mSystem->createChannelGroup("BGMGroup", &mBGMGroup);
    if (result != FMOD_OK) return;

    // Load SE sounds
    mSystem->createSound("Sound / MissileLaunch.wav", FMOD_DEFAULT, 0, &mSE[0]);
    mSoundMap["Missile"] = mSE[0];
    mSystem->createSound("Sound / CannonLaunch.wav", FMOD_DEFAULT, 0, &mSE[1]);
    mSoundMap["Cannon"] = mSE[1];
    mSystem->createSound("Sound / Explosion.wav", FMOD_DEFAULT, 0, &mSE[2]);
    mSoundMap["Explosion"] = mSE[2];
    

    // Set default volumes
    if (mSEGroup) mSEGroup->setVolume(mBGMVolume);
    if (mBGMGroup) mBGMGroup->setVolume(mSEVolume);
}

void SoundManager::mPlaySound(const std::string& SoundName, bool loop) {
    auto it = mSoundMap.find(SoundName);
    if (it == mSoundMap.end()) return;

    FMOD::Sound* sound = it->second;
    FMOD::Channel* channel = nullptr;
    bool isBGM = false;

    // Check if the sound is a BGM
    for (int i = 0; i < MAX_BGM_SOUND; i++) {
        if (sound == mBGM[i]) {
            isBGM = true;
            break;
        }
    }

    if (isBGM) {
        if (mBGMChannel) {
            mBGMChannel->stop();
            mBGMChannel = nullptr;
        }
        FMOD_RESULT result = mSystem->playSound(sound, mBGMGroup, !loop, &mBGMChannel);
        if (result != FMOD_OK) return;
    }
    else {
        FMOD_MODE mode = FMOD_DEFAULT;
        if (loop) mode |= FMOD_LOOP_NORMAL;
        sound->setMode(mode);
        FMOD_RESULT result = mSystem->playSound(sound, mSEGroup, false, &channel);
        if (result != FMOD_OK) return;
        if (channel) mSEChannels.push_back(channel);
    }
}

void SoundManager::Update() {

    // Set default volumes
    if (mSEGroup) mSEGroup->setVolume(mBGMVolume);
    if (mBGMGroup) mBGMGroup->setVolume(mSEVolume);

    // Clean up finished SE channels
    for (auto it = mSEChannels.begin(); it != mSEChannels.end();) {
        bool isPlaying = false;
        if (*it) {
            (*it)->isPlaying(&isPlaying);
            if (!isPlaying) {
                it = mSEChannels.erase(it);
            }
            else {
                ++it;
            }
        }
        else {
            it = mSEChannels.erase(it);
        }
    }
    // Update FMOD system
    if (mSystem) mSystem->update();
}

void SoundManager::Release() {
    // Release SE sounds
    for (int i = 0; i < MAX_SE_SOUND; i++) {
        if (mSE[i]) {
            mSE[i]->release();
            mSE[i] = nullptr;
        }
    }
    // Release BGM sounds
    for (int i = 0; i < MAX_BGM_SOUND; i++) {
        if (mBGM[i]) {
            mBGM[i]->release();
            mBGM[i] = nullptr;
        }
    }
    // Release channel groups
    if (mSEGroup) {
        mSEGroup->release();
        mSEGroup = nullptr;
    }
    if (mBGMGroup) {
        mBGMGroup->release();
        mBGMGroup = nullptr;
    }
    // Release system
    if (mSystem) {
        mSystem->release();
        mSystem = nullptr;
    }
    mSEChannels.clear();
}