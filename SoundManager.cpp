#include "SoundManager.h"

#pragma comment(lib, "fmod_vc.lib")
#pragma comment(lib, "fmodL_vc.lib")

#define MAX_SE_SOUND 20
#define MAX_BGM_SOUND 10

void SoundManager::Initialize() {
    // Create FMOD system

    mBGMVolume = 0.05f;
    mSEVolume = 0.02f;
    FMOD_RESULT result = FMOD::System_Create(&mSystem);
    if (result != FMOD_OK) {
        std::cout << "FMOD System_Create failed: " << result << std::endl;
        return;
    }
    result = mSystem->init(128, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK) {
        std::cout << "FMOD init failed: " << result << std::endl;
        return;
    }

    // Create channel groups
    result = mSystem->createChannelGroup("SEGroup", &mSEGroup);
    if (result != FMOD_OK) {
        std::cout << "SE Channel Group creation failed: " << result << std::endl;
        return;
    }
    result = mSystem->createChannelGroup("BGMGroup", &mBGMGroup);
    if (result != FMOD_OK) {
        std::cout << "BGM Channel Group creation failed: " << result << std::endl;
        return;
    }

    // Load SE sounds
    result = mSystem->createSound("Sound/MissileLaunch.wav", FMOD_DEFAULT, 0, &mSE[0]);
    if (result == FMOD_OK) {
        mSoundMap["Missile"] = mSE[0];
        std::cout << "Missile sound loaded successfully" << std::endl;
    } else {
        std::cout << "Failed to load Missile sound: " << result << std::endl;
    }
    
    result = mSystem->createSound("Sound/CannonLaunch.wav", FMOD_DEFAULT, 0, &mSE[1]);
    if (result == FMOD_OK) {
        mSoundMap["Cannon"] = mSE[1];
        std::cout << "Cannon sound loaded successfully" << std::endl;
    } else {
        std::cout << "Failed to load Cannon sound: " << result << std::endl;
    }
    
    result = mSystem->createSound("Sound/Explosion.wav", FMOD_DEFAULT, 0, &mSE[2]);
    if (result == FMOD_OK) {
        mSoundMap["Explosion"] = mSE[2];
        std::cout << "Explosion sound loaded successfully" << std::endl;
    } else {
        std::cout << "Failed to load Explosion sound: " << result << std::endl;
    }


	// Load BGM sounds
    result = mSystem->createSound("Sound/Helicopter.wav", FMOD_DEFAULT, 0, &mBGM[0]);
    if (result == FMOD_OK) {
        mSoundMap["Helicopter"] = mBGM[0];
        std::cout << "Helicopter sound loaded successfully" << std::endl;
    }
    else {
        std::cout << "Failed to load Helicopter sound: " << result << std::endl;
    }

    // Set default volumes (FIXED: SE gets mSEVolume, BGM gets mBGMVolume)
    if (mSEGroup) mSEGroup->setVolume(mSEVolume);
    if (mBGMGroup) mBGMGroup->setVolume(mBGMVolume);
    
    std::cout << "SoundManager initialized successfully" << std::endl;
}

void SoundManager::mPlaySound(const std::string& SoundName, bool loop) {
    auto it = mSoundMap.find(SoundName);
    if (it == mSoundMap.end()) {
        std::cout << "Sound not found: " << SoundName << std::endl;
        return;
    }

    FMOD::Sound* sound = it->second;
    if (!sound) {
        std::cout << "Sound pointer is null: " << SoundName << std::endl;
        return;
    }
    
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
        if (result != FMOD_OK) {
            std::cout << "Failed to play BGM: " << SoundName << ", error: " << result << std::endl;
            return;
        }
        std::cout << "Playing BGM: " << SoundName << std::endl;
    }
    else {
        FMOD_MODE mode = FMOD_DEFAULT;
        if (loop) mode |= FMOD_LOOP_NORMAL;
        sound->setMode(mode);
        FMOD_RESULT result = mSystem->playSound(sound, mSEGroup, false, &channel);
        if (result != FMOD_OK) {
            std::cout << "Failed to play SE: " << SoundName << ", error: " << result << std::endl;
            return;
        }
        if (channel) {
            mSEChannels.push_back(channel);
            std::cout << "Playing SE: " << SoundName << std::endl;
        }
    }
}

void SoundManager::Update() {

// Set default volumes (FIXED: SE gets mSEVolume, BGM gets mBGMVolume)
if (mSEGroup) mSEGroup->setVolume(mSEVolume);
if (mBGMGroup) mBGMGroup->setVolume(mBGMVolume);

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