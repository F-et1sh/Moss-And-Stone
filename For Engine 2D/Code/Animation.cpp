#include "forpch.h"
#include "Animation.h"

void FE2D::Animation::Release()
{
}

bool FE2D::Animation::LoadFromFile(const std::wstring& file_path)
{
	return false;
}

json FE2D::Animation::Serialize() const
{
	return json();
}

void FE2D::Animation::Deserialize(const json& j)
{
}
