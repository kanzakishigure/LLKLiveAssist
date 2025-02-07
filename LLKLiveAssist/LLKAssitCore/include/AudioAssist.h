#pragma once
#include "PluginBase.h"
#include <thread>
#include <queue>

#include <miniaudio.h>


#include "Core/PeriodicTask.h"


namespace NAssist {

	class AudioEngine
	{
	public:
		static std::shared_ptr<AudioEngine> Create();
		explicit AudioEngine();
		~AudioEngine();

		ma_engine* GetPMaEngine() { return &m_engine; }

		void init();
		void uninit();
	private:
		ma_engine m_engine;
	};
	class Sound
	{
	public:
		Sound() = default;
		explicit Sound(std::vector<uint8_t> bytes, std::shared_ptr<AudioEngine> engine);
		~Sound();
		static std::shared_ptr<Sound> CreateFromBytes(std::vector<uint8_t> bytes, std::shared_ptr<AudioEngine> engine);

		void Play();
	private:
		ma_audio_buffer* m_audio_buffer = nullptr;
		ma_sound m_sound;
		std::vector<uint8_t> m_buffer;
	};
	class AudioAssist : public PluginBase {
	public:
		virtual void init() override;
		virtual void start() override;
		virtual void drawUI() override;
		virtual void shutdown() override;

		virtual PluginType getType() override { return PluginType::Audio; }
		static PluginType getStaticType() { return PluginType::Audio; }

	private:

		std::shared_ptr<PeriodicTask> m_PlaybackTask;
		std::thread m_PlaybackThread;
		std::vector<std::shared_ptr<Sound>> m_PlaybackQueue;
		std::shared_ptr<AudioEngine> m_Engine;



	};



} // namespace NAssist
