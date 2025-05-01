#include "forpch.h"
#include "ResourceCache.h"

ResourceCache::_Resource FE2D::ResourceCache::get_resource(_TypeIndex hash_code, _ItemIndex index) {
	auto it_array = m_StoredResources.find(hash_code);
	if (it_array == m_StoredResources.end())
		return nullptr;

	auto it = it_array->second.find(index);
	if (it == it_array->second.end())
		return nullptr;

	return it->second;
}

std::optional<ResourceCache::_Metadata> FE2D::ResourceCache::get_metadata(_TypeIndex hash_code, _ItemIndex index) {
	auto it_array = m_StoredMetadata.find(hash_code);
	if (it_array == m_StoredMetadata.end())
		return std::nullopt;

	auto it = it_array->second.find(index);
	if (it == it_array->second.end())
		return std::nullopt;

	return it->second;
}

void FE2D::ResourceCache::remove_resource(_TypeIndex hash_code, _ItemIndex index) {
	auto it_array = m_StoredResources.find(hash_code);
	if (it_array == m_StoredResources.end())
		return;

	auto it = it_array->second.find(index);
	if (it == it_array->second.end())
		return;

	delete it->second;
	it_array->second.erase(it);
}

void FE2D::ResourceCache::remove_metadata(_TypeIndex hash_code, _ItemIndex index) {
	auto it_array = m_StoredMetadata.find(hash_code);
	if (it_array == m_StoredMetadata.end())
		return;

	auto it = it_array->second.find(index);
	if (it == it_array->second.end())
		return;

	it->second.clear();
	it_array->second.erase(it);
}