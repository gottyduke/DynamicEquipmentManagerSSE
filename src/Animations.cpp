#include "Animations.h"

#include <cctype>  // tolower
#include <string>  // string
#include <string_view>  // string_view

#include "RE/Skyrim.h"


Anim HashAnimation(const char* a_str, const std::uint32_t a_len)
{
	return Anim(hash_64_fnv1a(a_str, a_len));
}


Anim HashAnimation(const RE::BSFixedString& a_str)
{
	std::string str = "";
	str.reserve(a_str.size());
	std::string_view view = a_str;
	for (auto ch : view) {
		str += std::tolower(ch);
	}
	return HashAnimation(str.c_str(), str.length());
}
