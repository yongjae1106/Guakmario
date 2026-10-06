#include "sound.h"
#include "data.h"
#include "game.h"

Sound jump_small_Sound;
Sound jump_big_Sound;
Sound fireball_Sound;
Sound bowserfire_Sound;
Sound kick_Sound;
Sound stomp_Sound;

Sound bump_Sound;

Sound bowserfalls_Sound;
Sound bowserdead_Sound;

Sound coin_Sound;
Sound up_Sound;
Sound powerup_Sound;
Sound powerup_appears_Sound;
Sound powerdown_Sound;

Sound die_Sound;
Sound gameover_Sound;
Sound stage_clear_Sound;
Sound world_clear_Sound;

LPDIRECTSOUND8 g_pDS = NULL;
HWND g_hWnd = NULL; // ���� ������ �ڵ� (WinMain���� �Ѱ���� ��)
LPDIRECTSOUNDBUFFER g_pBGMBuffer = NULL;

bool InitDirectSound(HWND hwnd)
{
    g_hWnd = hwnd;

    if (FAILED(DirectSoundCreate8(NULL, &g_pDS, NULL))) return false;

    // �켱�� ����
    if (FAILED(g_pDS->SetCooperativeLevel(hwnd, DSSCL_PRIORITY))) return false;

    return true;
}

Sound LoadWavSound(const char* filename) 
{
    Sound sound;
    FILE* file = fopen(filename, "rb");
    if (!file) {
        MessageBoxA(NULL, (std::string("���� ���� ����: ") + filename).c_str(), "Error", MB_OK);
        return sound;
    }

    // WAV Header �б�
    char chunkId[4];
    fread(chunkId, 1, 4, file); // "RIFF"
    fseek(file, 4, SEEK_CUR);  // ���� ũ��
    fread(chunkId, 1, 4, file); // "WAVE"

    fread(chunkId, 1, 4, file); // "fmt "
    DWORD fmtSize;
    fread(&fmtSize, 4, 1, file);

    WAVEFORMATEX wfx = {};
    fread(&wfx, 1, fmtSize, file);

    fread(chunkId, 1, 4, file); // "data"
    DWORD dataSize;
    fread(&dataSize, 4, 1, file);

    BYTE* wavData = new BYTE[dataSize];
    fread(wavData, 1, dataSize, file);
    fclose(file);

    DSBUFFERDESC desc = {};
    desc.dwSize = sizeof(desc);
    desc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC | DSBCAPS_GLOBALFOCUS; // �÷��� ����
    desc.dwBufferBytes = dataSize;
    desc.lpwfxFormat = &wfx;

    LPDIRECTSOUNDBUFFER pBuf = nullptr;
    HRESULT hr = g_pDS->CreateSoundBuffer(&desc, &pBuf, NULL);
    if (FAILED(hr)) {
        MessageBoxA(NULL, (std::string("CreateSoundBuffer ����: ") + filename).c_str(), "Error", MB_OK);
        delete[] wavData;
        return sound;
    }

    void* pLock1 = nullptr;
    DWORD size1 = 0;
    pBuf->Lock(0, dataSize, &pLock1, &size1, NULL, NULL, 0);
    memcpy(pLock1, wavData, dataSize);
    pBuf->Unlock(pLock1, size1, NULL, 0);

    delete[] wavData;
    sound.pBuffer = pBuf;
    return sound;
}
LPDIRECTSOUNDBUFFER LoadWavSound_BGM(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (!file) {
        MessageBoxA(NULL, (std::string("���� ���� ����: ") + filename).c_str(), "Error", MB_OK);
        return g_pBGMBuffer;
    }

    // WAV Header �б�
    char chunkId[4];
    fread(chunkId, 1, 4, file); // "RIFF"
    fseek(file, 4, SEEK_CUR);  // ���� ũ��
    fread(chunkId, 1, 4, file); // "WAVE"

    fread(chunkId, 1, 4, file); // "fmt "
    DWORD fmtSize;
    fread(&fmtSize, 4, 1, file);

    WAVEFORMATEX wfx = {};
    fread(&wfx, 1, fmtSize, file);

    fread(chunkId, 1, 4, file); // "data"
    DWORD dataSize;
    fread(&dataSize, 4, 1, file);

    BYTE* wavData = new BYTE[dataSize];
    fread(wavData, 1, dataSize, file);
    fclose(file);

    DSBUFFERDESC desc = {};
    desc.dwSize = sizeof(desc);
    desc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC | DSBCAPS_GLOBALFOCUS; // �÷��� ����
    desc.dwBufferBytes = dataSize;
    desc.lpwfxFormat = &wfx;

    LPDIRECTSOUNDBUFFER pBuf = nullptr;
    HRESULT hr = g_pDS->CreateSoundBuffer(&desc, &pBuf, NULL);
    if (FAILED(hr)) {
        MessageBoxA(NULL, (std::string("CreateSoundBuffer ����: ") + filename).c_str(), "Error", MB_OK);
        delete[] wavData;
        return g_pBGMBuffer;
    }

    void* pLock1 = nullptr;
    DWORD size1 = 0;
    pBuf->Lock(0, dataSize, &pLock1, &size1, NULL, NULL, 0);
    memcpy(pLock1, wavData, dataSize);
    pBuf->Unlock(pLock1, size1, NULL, 0);

    delete[] wavData;
    g_pBGMBuffer = pBuf;
    return g_pBGMBuffer;
}

void PlaySoundBuffer(Sound& sound) 
{
    if (sound.pBuffer) 
    {
        sound.pBuffer->SetCurrentPosition(0);
        sound.pBuffer->Play(0, 0, 0); // �ѹ� ���
    }
}

void PlayBGM(const char* filename)
{
    // ���� BGM ���� �� ����
    if (g_pBGMBuffer)
    {
        g_pBGMBuffer->Stop();
        g_pBGMBuffer->Release();
        g_pBGMBuffer = NULL;
    }

    // �� BGM �ε�
    g_pBGMBuffer = LoadWavSound_BGM(filename);
    if (g_pBGMBuffer)
    {
        g_pBGMBuffer->SetVolume(0); // �ִ� ����
        g_pBGMBuffer->Play(0, 0, DSBPLAY_LOOPING);  // ���� ����
    }
}
void SetStage_BGM()
{
    if (stage == 1)
        PlayBGM("resource\\sound\\bgm\\GroundTheme.wav");
    else if (stage == 2)
        PlayBGM("resource\\sound\\bgm\\GroundTheme.wav");
    else if (stage == 3)
        PlayBGM("resource\\sound\\bgm\\CastleTheme.wav");
}
void sound_load()
{
    jump_small_Sound = LoadWavSound("resource\\sound\\smb_jump-small.wav");
    jump_big_Sound = LoadWavSound("resource\\sound\\smb_jump-super.wav");
    fireball_Sound = LoadWavSound("resource\\sound\\smb_fireball.wav");
    bowserfire_Sound = LoadWavSound("resource\\sound\\smb_bowserfire.wav");
    kick_Sound = LoadWavSound("resource\\sound\\smb_kick.wav");
    stomp_Sound = LoadWavSound("resource\\sound\\smb_stomp.wav");

    bump_Sound = LoadWavSound("resource\\sound\\smb_bump.wav");

    bowserfalls_Sound = LoadWavSound("resource\\sound\\smb_bowserfalls.wav");
    bowserdead_Sound = LoadWavSound("resource\\sound\\smb_bowserdead.wav");
    
    up_Sound = LoadWavSound("resource\\sound\\smb_1-up.wav");
    powerup_Sound = LoadWavSound("resource\\sound\\smb_powerup.wav");
    powerup_appears_Sound = LoadWavSound("resource\\sound\\smb_powerup_appears.wav");
    coin_Sound = LoadWavSound("resource\\sound\\smb_coin.wav");
    powerdown_Sound = LoadWavSound("resource\\sound\\smb_pipe.wav");
    
    die_Sound = LoadWavSound("resource\\sound\\smb_mariodie.wav");
    gameover_Sound = LoadWavSound("resource\\sound\\smb_gameover.wav");

    stage_clear_Sound = LoadWavSound("resource\\sound\\smb_stage_clear.wav");
    world_clear_Sound = LoadWavSound("resource\\sound\\smb_world_clear.wav");
}