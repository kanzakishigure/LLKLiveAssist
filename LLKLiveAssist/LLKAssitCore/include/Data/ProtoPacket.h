#pragma once
#include <boost/endian/buffers.hpp>

#include <vector>
#include "Data/JsonParser.h"
namespace NAssist {
enum class ProtoOperation : uint32_t {
  /// <summary>
  /// 心跳包
  /// </summary>
  HeartBeat = 2,

  /// <summary>
  /// 服务器心跳回应(包含人气信息)
  /// </summary>
  HeartBeatResponse = 3,

  /// <summary>
  /// 服务器消息(正常消息)
  /// </summary>
  ServerNotify = 5,

  /// <summary>
  /// 客户端认证请求
  /// </summary>
  Authority = 7,

  /// <summary>
  /// 认证回应
  /// </summary>
  AuthorityResponse = 8
};

/// <summary>
/// 弹幕协议版本
/// </summary>
enum class ProtoVersion : uint16_t {
  /// <summary>
  /// 未压缩数据
  /// </summary>
  UnCompressed = 0,

  /// <summary>
  /// 心跳数据
  /// </summary>
  HeartBeat = 1,

  /// <summary>
  /// zlib数据
  /// </summary>
  Zlib = 2,

  /// <summary>
  /// Br数据
  /// </summary>
  Brotli = 3
};

struct ProtoPacketHeader {
  int32_t packet_length;
  int16_t header_length = 16;
  ProtoVersion version;
  ProtoOperation operation;
  int32_t sequence_id;
};

struct ProtoPacket : public Serializeable {
  ProtoPacketHeader header;
  std::vector<uint8_t> body;
};

static bool is_little_endian2() {
  int a = 0x12345678;
  char b = *(reinterpret_cast<char *>(&a));
  return 0x78 == b;
}

static std::vector<uint8_t> ProtoPacket2bytes(ProtoPacket packet) {
  if (is_little_endian2()) {
    packet.header.packet_length =
        boost::endian::endian_reverse(packet.header.packet_length);
    packet.header.header_length =
        boost::endian::endian_reverse(packet.header.header_length);
    packet.header.version =
        boost::endian::endian_reverse(packet.header.version);
    packet.header.operation =
        boost::endian::endian_reverse(packet.header.operation);
    packet.header.sequence_id =
        boost::endian::endian_reverse(packet.header.sequence_id);
  }
  std::vector<uint8_t> data(16);
  *(reinterpret_cast<uint32_t *>(&data[0])) = static_cast<uint32_t>(packet.header.packet_length);
  *(reinterpret_cast<uint16_t *>(&data[4])) = static_cast<uint16_t>(packet.header.header_length);
  *(reinterpret_cast<uint16_t *>(&data[6])) = static_cast<uint16_t>(packet.header.version);
  *(reinterpret_cast<uint32_t *>(&data[8])) = static_cast<uint32_t>(packet.header.operation);
  *(reinterpret_cast<uint32_t *>(&data[12])) = static_cast<uint32_t>(packet.header.sequence_id);

  data.insert(data.end(), packet.body.begin(), packet.body.end());

  return data;
}
} // namespace NAssist