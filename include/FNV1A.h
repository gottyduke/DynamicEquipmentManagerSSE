#pragma once

#include <stdint.h>


//fnv1a 32 and 64 bit hash functions
// key is the data to hash, len is the size of the data (or how much of it to hash against)
// code license: public domain or equivalent
// post: https://notes.underscorediscovery.com/constexpr-fnv1a/


inline const std::uint32_t hash_32_fnv1a(const void* a_key, const std::uint32_t a_len)
{
	const char* data = (char*)a_key;
	std::uint32_t hash = 0x811c9dc5;
	std::uint32_t prime = 0x1000193;

	for (int i = 0; i < a_len; ++i) {
		std::uint8_t value = data[i];
		hash = hash ^ value;
		hash *= prime;
	}

	return hash;
} //hash_32_fnv1a


inline const std::uint64_t hash_64_fnv1a(const void* a_key, const std::uint64_t a_len)
{
	const char* data = (char*)a_key;
	std::uint64_t hash = 0xcbf29ce484222325;
	std::uint64_t prime = 0x100000001b3;

	for (int i = 0; i < a_len; ++i) {
		std::uint8_t value = data[i];
		hash = hash ^ value;
		hash *= prime;
	}

	return hash;
} //hash_64_fnv1a


// FNV1a c++11 constexpr compile time hash functions, 32 and 64 bit
// str should be a null terminated string literal, value should be left out
// e.g hash_32_fnv1a_const("example")
// code license: public domain or equivalent
// post: https://notes.underscorediscovery.com/constexpr-fnv1a/


constexpr std::uint32_t val_32_const = 0x811c9dc5;
constexpr std::uint32_t prime_32_const = 0x1000193;
constexpr std::uint64_t val_64_const = 0xcbf29ce484222325;
constexpr std::uint64_t prime_64_const = 0x100000001b3;


constexpr std::uint32_t hash_32_fnv1a_const(const char* const a_str, const std::uint32_t a_value = val_32_const) noexcept
{
	return (a_str[0] == '\0') ? a_value : hash_32_fnv1a_const(&a_str[1], (a_value ^ std::uint32_t(a_str[0])) * prime_32_const);
}


constexpr std::uint64_t hash_64_fnv1a_const(const char* const a_str, const std::uint64_t a_value = val_64_const) noexcept
{
	return (a_str[0] == '\0') ? a_value : hash_64_fnv1a_const(&a_str[1], (a_value ^ std::uint64_t(a_str[0])) * prime_64_const);
}
