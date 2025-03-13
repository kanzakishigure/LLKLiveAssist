#include "AudioAssist.h"
#include "GSoVITSAssist.h"
#include "ModuleManager.h"
#include "miniaudio.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <mutex>
#include <system_error>

namespace NAssist {

namespace Utils {
Sound::Format String2SoundType(std::string string) {
  if (string == "wav") {
    return Sound::Format::WAV;
  }
  if (string == "flac") {
    return Sound::Format::FLAC;
  }
  if (string == "raw") {
    return Sound::Format::RAW;
  }
  if (string == "mp3") {
    return Sound::Format::MP3;
  }

  return Sound::Format::NONE;
}
} // namespace Utils
void AudioAssist::init() {

  // init the audio engine
  m_Engine = AudioEngine::Create();

  CORE_INFO_TAG("AudioAssist", "AudioAssist Init success");
}

void AudioAssist::shutdown() {

  auto ec = stop();
  CORE_INFO_TAG("AudioAssist", "AudioAssist Shutdown");
}

std::error_code AudioAssist::start() {

  m_stoped = std::promise<void>();
  std::shared_future<void> future = m_stoped.get_future();
  m_PlaybackThread = std::thread(
      [this](std::shared_future<void> future) {
        while (future.wait_for(std::chrono::milliseconds(100)) ==
               std::future_status::timeout) {
          auto gsovits_assist =
              ModuleManager::getInstance().getModule<GSoVITSAssist>();
          auto [audio_stream, config] = gsovits_assist->popAudioSteam();
          if (!audio_stream.empty()) {
            m_PlaybackQueue.emplace_back() = Sound::CreateFromBytes(
                audio_stream, m_Engine, config, [this]() {
                  std::lock_guard<std::mutex> lg(m_playing_mutex);
                  m_stoped_condition.notify_all();
                });

            m_PlaybackQueue.back()->Play();
            std::unique_lock<std::mutex> lock(m_playing_mutex); // 获取锁
            m_stoped_condition.wait(lock);
          }
        }
      },
      std::move(future));

  CORE_INFO_TAG("AudioAssist", "AudioAssist Start success");
  return make_error_code(audio_engine_errc::success);
}
std::error_code AudioAssist::stop() {
  if (m_PlaybackThread.joinable()) {
    m_stoped.set_value();
    std::lock_guard<std::mutex> lg(m_playing_mutex);
    m_stoped_condition.notify_all();
    m_PlaybackThread.join();
  }
  CORE_INFO_TAG("AudioAssist", "AudioAssist stop ");
  return make_error_code(audio_engine_errc::success);
}

std::shared_ptr<AudioEngine> AudioEngine::Create() {
  return std::make_shared<AudioEngine>();
}

AudioEngine::AudioEngine() {
  init();

  CORE_INFO_TAG("AudioEngine", "AudioEngine init");
}

AudioEngine::~AudioEngine() {
  uninit();

  CORE_INFO_TAG("AudioEngine", "AudioEngine uninit");
}

void AudioEngine::init() {
  auto result = ma_engine_init(NULL, &m_engine);

  if (result != MA_SUCCESS) {
    std::string error = std::format("[{}]-[{}]:fail to init the audio engine",
                                    "AUDIO ASSIST", "ERROR");
    ma_engine_uninit(&m_engine);
  }
}

void AudioEngine::uninit() { ma_engine_uninit(&m_engine); }

Sound::Sound(std::vector<uint8_t> bytes, std::shared_ptr<AudioEngine> engine,
             AudioConfig config) {
  auto audio_assist = ModuleManager::getInstance().getModule<AudioAssist>();
  m_buffer = std::vector<uint8_t>(bytes.begin(), bytes.end());

  switch (config.format) {
  case Format::MP3:
  case Format::FLAC:
  case Format::WAV: {
    decodeDataFromEncodeData(config, engine);
  } break;

  case Format::RAW: {
    createBufferFromRawData(engine);
  } break;
  default:
    break;
  }
  // sound->engine_sound is a ma_sound
  ma_result sound_init_res = ma_sound_init_from_data_source(
      engine->GetPMaEngine(), &m_decoder,
      MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE |
          MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_STREAM,
      NULL, &m_sound);
  // sound_init_res is MA_SUCCESS
  if (sound_init_res != MA_SUCCESS) {
    std::string error =
        std::format("[{}]-[{}]:fail to create a ma_sound", "Sound", "ERROR");
  }
  setVolume(config.AudioVolume);
}
Sound::Sound(std::vector<uint8_t> bytes, std::shared_ptr<AudioEngine> engine,
             AudioConfig config, std::function<void()> callback)
    : m_callback(callback) {
  auto audio_assist = ModuleManager::getInstance().getModule<AudioAssist>();
  m_buffer = std::vector<uint8_t>(bytes.begin(), bytes.end());

  switch (config.format) {
  case Format::MP3:
  case Format::FLAC:
  case Format::WAV: {
    decodeDataFromEncodeData(config, engine);
  } break;

  case Format::RAW: {
    createBufferFromRawData(engine);
  } break;
  default:
    break;
  }

  // sound->engine_sound is a ma_sound
  ma_sound_config sound_config = ma_sound_config_init_2(engine->GetPMaEngine());
  sound_config.pDataSource = &m_decoder;
  sound_config.flags = MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE |
                       MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_STREAM;
  sound_config.pInitialAttachment = NULL;
  sound_config.pEndCallbackUserData = &m_callback;
  sound_config.endCallback = [](void *pUserData, ma_sound * /*pSound*/) {
    auto *callback = static_cast<std::function<void()> *>(pUserData);
    (*callback)();
  };
  ma_result sound_init_res =
      ma_sound_init_ex(engine->GetPMaEngine(), &sound_config, &m_sound);

  // sound_init_res is MA_SUCCESS
  if (sound_init_res != MA_SUCCESS) {
    std::string error =
        std::format("[{}]-[{}]:fail to create a ma_sound", "Sound", "ERROR");
  }
  setVolume(config.AudioVolume);
}
Sound::~Sound() { ma_decoder_uninit(&m_decoder); }

std::shared_ptr<Sound>
Sound::CreateFromBytes(std::vector<uint8_t> bytes,
                       std::shared_ptr<AudioEngine> engine,
                       AudioConfig config) {
  return std::make_shared<Sound>(bytes, engine, config);
}

std::shared_ptr<Sound>
Sound::CreateFromBytes(std::vector<uint8_t> bytes,
                       std::shared_ptr<AudioEngine> engine, AudioConfig config,
                       std::function<void()> callback) {
  return std::make_shared<Sound>(bytes, engine, config, callback);
}
bool Sound::isPlaying() { return ma_sound_is_playing(&m_sound); }
void Sound::setVolume(float value) { ma_sound_set_volume(&m_sound, value); }
void Sound::Play() { ma_sound_start(&m_sound); }

void Sound::decodeDataFromEncodeData(AudioConfig config,
                                     std::shared_ptr<AudioEngine> engine) {

  switch (config.format) {
  case Format::WAV:
  case Format::MP3:
  case Format::FLAC:
    break;
  default:
    CORE_ERROR_TAG("Sound", "invaild format to decode");
    return;
  }
  auto audio_assist = ModuleManager::getInstance().getModule<AudioAssist>();
  // decoder the memory  to  resolve audiostream
  ma_uint64 size_in_frames = 0;
  void *frames_out = nullptr;
  ma_channel channels = ma_engine_get_channels(engine->GetPMaEngine());

  ma_decoder_config decoder_config = ma_decoder_config_init(
      config.Dcoderformat, channels, config.DcoderSampleRate);

  ma_result decode_config_init_res = ma_decoder_init_memory(
      m_buffer.data(), m_buffer.size(), &decoder_config, &m_decoder);
  if (decode_config_init_res != MA_SUCCESS) {
    CORE_ERROR_TAG("Sound", "fail to decode format");
  }
}
void Sound::createBufferFromRawData(std::shared_ptr<AudioEngine> engine) {}
} // namespace NAssist