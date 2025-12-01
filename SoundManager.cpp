//#include "SoundManager.h"
//
//#include "SoundManager.h"
//
//#define MAX_SE_SOUND 20
//#define MAX_BGM_SOUND 10
//
//void SoundManager::Initialize() {
//    // Create FMOD system
//
//    mBGMVolume = 0.15f;
//    mSEVolume = 0.1f;
//    FMOD_RESULT result = FMOD::System_Create(&mSystem);
//    if (result != FMOD_OK) return;
//    result = mSystem->init(128, FMOD_INIT_NORMAL, 0);
//    if (result != FMOD_OK) return;
//
//    // Create channel groups
//    result = mSystem->createChannelGroup("SEGroup", &mSEGroup);
//    if (result != FMOD_OK) return;
//    result = mSystem->createChannelGroup("BGMGroup", &mBGMGroup);
//    if (result != FMOD_OK) return;
//
//    // Load SE sounds
//    // Enemy
//    mSystem->createSound("resources/Audio/SE/Hit.wav", FMOD_DEFAULT, 0, &mSE[19]);
//    mSoundMap["Hit"] = mSE[19];
//    mSystem->createSound("resources/Audio/SE/EnemyDead.wav", FMOD_DEFAULT, 0, &mSE[20]);
//    mSoundMap["EnemyDead"] = mSE[20];
//
//    // Archer
//    mSystem->createSound("resources/Audio/SE/ArcherRelease.wav", FMOD_DEFAULT, 0, &mSE[0]);
//    mSoundMap["ArcherRelease"] = mSE[0];
//    mSystem->createSound("resources/Audio/SE/ArcherReload.wav", FMOD_DEFAULT, 0, &mSE[1]);
//    mSoundMap["ArcherReload"] = mSE[1];
//    mSystem->createSound("resources/Audio/SE/ArcherShoot.wav", FMOD_DEFAULT, 0, &mSE[2]);
//    mSoundMap["ArcherShoot"] = mSE[2];
//
//    // SwordMan
//    mSystem->createSound("resources/Audio/SE/SwordManAttack.wav", FMOD_DEFAULT, 0, &mSE[9]);
//    mSoundMap["SwordManAttack"] = mSE[9];
//
//    // Wizard
//    mSystem->createSound("resources/Audio/SE/EnemyFireBall.wav", FMOD_DEFAULT, 0, &mSE[18]);
//    mSoundMap["EnemyFireBall"] = mSE[18];
//
//    // Player
//    mSystem->createSound("resources/Audio/SE/PlayerFootstep.wav", FMOD_DEFAULT, 0, &mSE[8]);
//    mSoundMap["PlayerFootstep"] = mSE[8];
//    mSystem->createSound("resources/Audio/SE/Dash.wav", FMOD_DEFAULT, 0, &mSE[3]);
//    mSoundMap["Dash"] = mSE[3];
//    mSystem->createSound("resources/Audio/SE/Fireball.wav", FMOD_DEFAULT, 0, &mSE[4]);
//    mSoundMap["Fireball"] = mSE[4];
//    mSystem->createSound("resources/Audio/SE/FireBlastEnd.wav", FMOD_DEFAULT, 0, &mSE[5]);
//    mSoundMap["FireBlastEnd"] = mSE[5];
//    mSystem->createSound("resources/Audio/SE/FireBlastLOOP.wav", FMOD_DEFAULT, 0, &mSE[6]);
//    mSoundMap["FireBlastLOOP"] = mSE[6];
//    mSystem->createSound("resources/Audio/SE/FireBlastStart.wav", FMOD_DEFAULT, 0, &mSE[7]);
//    mSoundMap["FireBlastStart"] = mSE[7];
//    mSystem->createSound("resources/Audio/SE/StandardPlayerAttack.wav", FMOD_DEFAULT, 0, &mSE[10]);
//    mSoundMap["PlayerAttack"] = mSE[10];
//
//    // Boss
//    mSystem->createSound("resources/Audio/SE/BossBubbleRelease.wav", FMOD_DEFAULT, 0, &mSE[11]);
//    mSoundMap["BossBubbleRelease"] = mSE[11];
//    mSystem->createSound("resources/Audio/SE/BossDash.wav", FMOD_DEFAULT, 0, &mSE[12]);
//    mSoundMap["BossDash"] = mSE[12];
//    mSystem->createSound("resources/Audio/SE/BossAttackStart.wav", FMOD_DEFAULT, 0, &mSE[13]);
//    mSoundMap["BossAttackStart"] = mSE[13];
//    mSystem->createSound("resources/Audio/SE/BossSwordRelease.wav", FMOD_DEFAULT, 0, &mSE[14]);
//    mSoundMap["BossAttackRelease"] = mSE[14];
//    mSystem->createSound("resources/Audio/SE/BossAquaBallStart.wav", FMOD_DEFAULT, 0, &mSE[15]);
//    mSoundMap["BossAquaBallStart"] = mSE[15];
//
//    // Portal
//    mSystem->createSound("resources/Audio/SE/ExitPortal.wav", FMOD_DEFAULT, 0, &mSE[17]);
//    mSoundMap["ExitPortal"] = mSE[17];
//
//    //Menu
//    mSystem->createSound("resources/Audio/SE/MenuOpen.wav", FMOD_DEFAULT, 0, &mSE[16]);
//    mSoundMap["MenuOpen"] = mSE[16];
//
//    // Load BGM sounds
//    mSystem->createSound("resources/Audio/BGM/Tutorial_Jazz.wav", FMOD_LOOP_NORMAL, 0, &mBGM[0]);
//    mSoundMap["Tutorial_Jazz"] = mBGM[0];
//    mSystem->createSound("resources/Audio/BGM/Boss.wav", FMOD_LOOP_NORMAL, 0, &mBGM[1]);
//    mSoundMap["Boss"] = mBGM[1];
//    mSystem->createSound("resources/Audio/BGM/Earth.wav", FMOD_LOOP_NORMAL, 0, &mBGM[2]);
//    mSoundMap["Earth"] = mBGM[2];
//    mSystem->createSound("resources/Audio/BGM/TitleScreen.wav", FMOD_LOOP_NORMAL, 0, &mBGM[3]);
//    mSoundMap["TitleScreen"] = mBGM[3];
//    mSystem->createSound("resources/Audio/BGM/EndScene.wav", FMOD_LOOP_NORMAL, 0, &mBGM[4]);
//    mSoundMap["EndScene"] = mBGM[4];
//    mSystem->createSound("resources/Audio/BGM/GameOver.wav", FMOD_LOOP_NORMAL, 0, &mBGM[5]);
//    mSoundMap["GameOver"] = mBGM[5];
//    mSystem->createSound("resources/Audio/BGM/BossDead.wav", FMOD_LOOP_NORMAL, 0, &mBGM[6]);
//    mSoundMap["BossDead"] = mBGM[6];
//
//
//    // Set default volumes
//    if (mSEGroup) mSEGroup->setVolume(mBGMVolume);
//    if (mBGMGroup) mBGMGroup->setVolume(mSEVolume);
//}
//
//void SoundManager::mPlaySound(const std::string& SoundName, bool loop) {
//    auto it = mSoundMap.find(SoundName);
//    if (it == mSoundMap.end()) return;
//
//    FMOD::Sound* sound = it->second;
//    FMOD::Channel* channel = nullptr;
//    bool isBGM = false;
//
//    // Check if the sound is a BGM
//    for (int i = 0; i < MAX_BGM_SOUND; i++) {
//        if (sound == mBGM[i]) {
//            isBGM = true;
//            break;
//        }
//    }
//
//    if (isBGM) {
//        if (mBGMChannel) {
//            mBGMChannel->stop();
//            mBGMChannel = nullptr;
//        }
//        FMOD_RESULT result = mSystem->playSound(sound, mBGMGroup, !loop, &mBGMChannel);
//        if (result != FMOD_OK) return;
//    }
//    else {
//        FMOD_MODE mode = FMOD_DEFAULT;
//        if (loop) mode |= FMOD_LOOP_NORMAL;
//        sound->setMode(mode);
//        FMOD_RESULT result = mSystem->playSound(sound, mSEGroup, false, &channel);
//        if (result != FMOD_OK) return;
//        if (channel) mSEChannels.push_back(channel);
//    }
//}
//
//void SoundManager::Update() {
//
//    // Set default volumes
//    if (mSEGroup) mSEGroup->setVolume(mBGMVolume);
//    if (mBGMGroup) mBGMGroup->setVolume(mSEVolume);
//
//    // Clean up finished SE channels
//    for (auto it = mSEChannels.begin(); it != mSEChannels.end();) {
//        bool isPlaying = false;
//        if (*it) {
//            (*it)->isPlaying(&isPlaying);
//            if (!isPlaying) {
//                it = mSEChannels.erase(it);
//            }
//            else {
//                ++it;
//            }
//        }
//        else {
//            it = mSEChannels.erase(it);
//        }
//    }
//    // Update FMOD system
//    if (mSystem) mSystem->update();
//}
//
//void SoundManager::Release() {
//    // Release SE sounds
//    for (int i = 0; i < MAX_SE_SOUND; i++) {
//        if (mSE[i]) {
//            mSE[i]->release();
//            mSE[i] = nullptr;
//        }
//    }
//    // Release BGM sounds
//    for (int i = 0; i < MAX_BGM_SOUND; i++) {
//        if (mBGM[i]) {
//            mBGM[i]->release();
//            mBGM[i] = nullptr;
//        }
//    }
//    // Release channel groups
//    if (mSEGroup) {
//        mSEGroup->release();
//        mSEGroup = nullptr;
//    }
//    if (mBGMGroup) {
//        mBGMGroup->release();
//        mBGMGroup = nullptr;
//    }
//    // Release system
//    if (mSystem) {
//        mSystem->release();
//        mSystem = nullptr;
//    }
//    mSEChannels.clear();
//}