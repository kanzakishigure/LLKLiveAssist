#include "BroadcastAssist.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <iostream>


namespace NAssist {
void BroadcastAssist::init() {
  ma_result result;
  ma_decoder m_decoder;
  ma_device_config m_deviceConfig;
  ma_device m_device;
  result = ma_decoder_init_file("output/test.wav", nullptr, &m_decoder);
 if (result != MA_SUCCESS) {
       std::cout << "Failed to initialize decoder.\n";
        return ;
    }

    m_deviceConfig = ma_device_config_init(ma_device_type_playback);
    m_deviceConfig.playback.format   = m_decoder.outputFormat;
    m_deviceConfig.playback.channels = m_decoder.outputChannels;
    m_deviceConfig.sampleRate        = m_decoder.outputSampleRate;
    m_deviceConfig.dataCallback      = nullptr;
    m_deviceConfig.pUserData         = &m_decoder;


  std::cout << "BroadcastAssist Init \n";
}

void BroadcastAssist::shutdown() {
  std::cout << "BroadcastAssist Shutdown function\n";
}

void BroadcastAssist::drawUI() {}

void BroadcastAssist::start() {
  std::cout << "BroadcastAssist Start function\n";
}
} // namespace NAssist