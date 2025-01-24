#pragma once
#include <memory>
#include <string>


#include <boost/algorithm/hex.hpp>
#include <boost/uuid/detail/md5.hpp>

#include <openssl/sha.h>
#include <openssl/hmac.h>

namespace NAssist {
class MD5Encryption {
public:
  inline static std::string getMd5(std::string source);
};

std::string MD5Encryption::getMd5(std::string source) {
  boost::uuids::detail::md5 hash;
  boost::uuids::detail::md5::digest_type digest;
  hash.process_bytes(source.data(), source.size());
  hash.get_digest(digest);

  const auto charDigest = reinterpret_cast<const char *>(&digest);
  std::string result = "";
  boost::algorithm::hex_lower(
      charDigest, charDigest + sizeof(boost::uuids::detail::md5::digest_type),
      std::back_inserter(result));

  return result;
}

class HMACEncryption {
public:
	inline static std::string SHA256(std::string decodedKey, std::string msg);
};

std::string HMACEncryption::SHA256(std::string decodedKey,std::string msg) {
	

	std::array<unsigned char, EVP_MAX_MD_SIZE> hash;
	unsigned int hashLen;

	HMAC(
		EVP_sha256(),
		decodedKey.data(),
		static_cast<int>(decodedKey.size()),
		reinterpret_cast<unsigned char const*>(msg.data()),
		static_cast<int>(msg.size()),
		hash.data(),
		&hashLen
	);

	std::string result ;
	boost::algorithm::hex_lower(
		hash.data(), hash.data() + hashLen,
		std::back_inserter(result));
	return result;
}
}; // namespace NAssist