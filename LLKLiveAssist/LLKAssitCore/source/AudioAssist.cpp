#include "AudioAssist.h"
#include "GSoVITSAssist.h"
#include "ModuleManager.h"

#include <fstream>
#include <iostream>

namespace NAssist {

void AudioAssist::init() {

  // init the audio engine
  m_Engine = AudioEngine::Create();

  m_PlaybackTask = std::make_shared<PeriodicTask>(
      [this]() {
        auto gsovits_assist =
            ModuleManager::getInstance().getModule<GSoVITSAssist>();
        std::vector<uint8_t> audio_stream = gsovits_assist->popAudioSteam();
        if (audio_stream.size() > 0) {
          m_PlaybackQueue.emplace_back() =
              Sound::CreateFromBytes(audio_stream, m_Engine);
          m_PlaybackQueue.back()->Play();
        }
      },
      1.0);

  CORE_INFO_TAG("AudioAssist", "AudioAssist Init success");
}

void AudioAssist::shutdown() {

  m_PlaybackTask->Stop();
  if (m_PlaybackThread.joinable()) {
    m_PlaybackThread.join();
  }
  CORE_INFO_TAG("AudioAssist", "AudioAssist Shutdown");
}

std::error_code AudioAssist::start() {

  m_PlaybackThread = std::thread([this] { m_PlaybackTask->Start(); });

  CORE_INFO_TAG("AudioAssist", "AudioAssist Start success");
  return make_error_code(audio_engine_errc::success);
}

Sound::Sound(std::vector<uint8_t> bytes, std::shared_ptr<AudioEngine> engine)
    : m_audio_buffer(nullptr) {
  auto audio_assist = ModuleManager::getInstance().getModule<AudioAssist>();
  m_buffer = std::vector<uint8_t>(bytes.begin(), bytes.end());
  // decoder the memory  to  resolve audiostream
  ma_uint64 size_in_frames = 0;
  void *frames_out = nullptr;
  ma_channel channels = ma_engine_get_channels(engine->GetPMaEngine());
  ma_format format = audio_assist->getAudioConfig().Dcoderformat;
  ma_decoder_config decoder_config = ma_decoder_config_init(
      format, channels, audio_assist->getAudioConfig().DcoderSampleRate);
  ma_result decode_memory_res =
      ma_decode_memory(m_buffer.data(), m_buffer.size(), &decoder_config,
                       &size_in_frames, &frames_out);

  if (decode_memory_res != MA_SUCCESS) {
    std::string error =
        std::format("[{}]-[{}]:fail to decode memory", "Sound", "ERROR");
  }

  // audio config
  ma_audio_buffer_config audio_buffer_config = ma_audio_buffer_config_init(
      format, channels, size_in_frames, m_buffer.data(), nullptr);
  audio_buffer_config.sampleRate =
      audio_assist->getAudioConfig().AudioSampleRate;

  // allocate audio buffer and copy the data
  ma_result audio_buffer_alloc_res =
      ma_audio_buffer_alloc_and_init(&audio_buffer_config, &m_audio_buffer);
  if (audio_buffer_alloc_res != MA_SUCCESS) {
    ma_audio_buffer_uninit_and_free(m_audio_buffer);
    std::string error = std::format(
        "[{}]-[{}]:fail to create a ma_audio_buffer", "Sound", "ERROR");
  }
  // sound->engine_sound is a ma_sound
  ma_result sound_init_res = ma_sound_init_from_data_source(
      engine->GetPMaEngine(), m_audio_buffer,
      MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE |
          MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_STREAM,
      NULL, &m_sound);
  // sound_init_res is MA_SUCCESS
  if (sound_init_res != MA_SUCCESS) {
    std::string error =
        std::format("[{}]-[{}]:fail to create a ma_sound", "Sound", "ERROR");
  }
}
Sound::~Sound() { ma_audio_buffer_uninit_and_free(m_audio_buffer); }

std::shared_ptr<Sound>
Sound::CreateFromBytes(std::vector<uint8_t> bytes,
                       std::shared_ptr<AudioEngine> engine) {
  return std::make_shared<Sound>(bytes, engine);
}

void Sound::Play() { ma_sound_start(&m_sound); }

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

} // namespace NAssist