#pragma once
#include "PluginBase.h"
#include <cstdint>
#include <format>
#include <functional>
#include <queue>
#include <thread>

#include <miniaudio.h>

#include "Core/PeriodicTask.h"

namespace NAssist {


struct AudioConfig;
class AudioEngine {
public:
  static std::shared_ptr<AudioEngine> Create();
  explicit AudioEngine();
  ~AudioEngine();

  ma_engine *GetPMaEngine() { return &m_engine; }

  void init();
  void uninit();

private:
  ma_engine m_engine;
};
class Sound {

public:
  enum class Format { RAW = 0, WAV, MP3, FLAC, NONE };

  Sound() = default;
  explicit Sound(std::vector<uint8_t> bytes,
                 std::shared_ptr<AudioEngine> engine, AudioConfig config);

  explicit Sound(std::vector<uint8_t> bytes,
                 std::shared_ptr<AudioEngine> engine, AudioConfig config,
                 std::function<void()> callback);
  ~Sound();
  bool isPlaying();
  void setVolume(float value);
  static std::shared_ptr<Sound>
  CreateFromBytes(std::vector<uint8_t> bytes,
                  std::shared_ptr<AudioEngine> engine, AudioConfig config);

  static std::shared_ptr<Sound>
  CreateFromBytes(std::vector<uint8_t> bytes,
                  std::shared_ptr<AudioEngine> engine, AudioConfig config,
                  std::function<void()> callback);

  void Play();
  void decodeDataFromEncodeData(AudioConfig config,
                                std::shared_ptr<AudioEngine> engine);
  void createBufferFromRawData(std::shared_ptr<AudioEngine> engine);

private:
  ma_sound m_sound;
  ma_decoder m_decoder;
  std::vector<uint8_t> m_buffer;
  std::function<void()> m_callback;
};

struct AudioConfig {
  ma_format Dcoderformat = ma_format_s16;
  uint32_t DcoderSampleRate = 32000;
  float AudioVolume = 1.0f;
  Sound::Format format;
};
class AudioAssist : public PluginBase {
public:
  virtual void init() override;
  virtual std::error_code start() override;
  virtual std::error_code stop() override;

  virtual void shutdown() override;

  virtual PluginType getType() override { return PluginType::Audio; }
  static PluginType getStaticType() { return PluginType::Audio; }

private:
  std::vector<std::shared_ptr<Sound>> m_PlaybackQueue;
  std::shared_ptr<AudioEngine> m_Engine;

  std::thread m_PlaybackThread;
  std::mutex m_playing_mutex;
  std::condition_variable m_stoped_condition;
  std::promise<void> m_stoped;
};


namespace Utils {
  Sound::Format String2SoundType(std::string string);
  }
} // namespace NAssist
