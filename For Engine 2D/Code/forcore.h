#pragma once

namespace FE2D {

#ifdef FORENGINE2D_EXPORTS
	#define FOR_API __declspec(dllexport)
#else
	#define FOR_API __declspec(dllimport)
#endif

#define FOR_NODISCARD [[nodiscard]]

#define FOR_STRINGIFY(x) #x
#define FOR_TOSTRING(x) FE2D::string_to_wstring(FOR_STRINGIFY(x))

}