#pragma once
#include "Resource.h"
#include "ResourceManager.h"

namespace FE2D {
	class FOR_API Animation : public Resource {
		Animation() = default;
		~Animation() = default;

		void Release();
		bool LoadFromFile(const std::wstring& file_path);

		json Serialize()const override;
		void Deserialize(const json& j)override;
	};
}