#include "forpch.h"
#include "ResourceCache.h"

void FE2D::ResourceCache::set_resource(FE2D::UUID uuid, ResourcePointer resource) {
	if (auto resource = this->get_resource(uuid)) {
		resource->Release();
		delete resource;
	}
	m_StoredResources.emplace(uuid, resource);
}

void FE2D::ResourceCache::set_metadata(FE2D::UUID uuid, const MetadataPath& metadata) {
	m_StoredMetadata.emplace(uuid, metadata);
}

void FE2D::ResourceCache::set_fallback(size_t hash_code, ResourcePointer resource) {
	if (auto fallback = this->get_fallback(hash_code)) {
		fallback->Release();
		delete fallback;
	}
	m_StoredFallbacks.emplace(hash_code, resource);
}

FE2D::ResourceCache::ResourcePointer FE2D::ResourceCache::get_resource(FE2D::UUID uuid) {
	auto it = m_StoredResources.find(uuid);
	if (it == m_StoredResources.end()) 
		return nullptr;

	return it->second;
}

FE2D::ResourceCache::MetadataPath FE2D::ResourceCache::get_metadata(FE2D::UUID uuid) {
	auto it = m_StoredMetadata.find(uuid);
	if (it == m_StoredMetadata.end()) 
		return std::filesystem::path();

	return it->second;
}

FE2D::ResourceCache::ResourcePointer FE2D::ResourceCache::get_fallback(size_t hash_code) {
	auto it = m_StoredFallbacks.find(hash_code);
	if (it == m_StoredFallbacks.end())
		return nullptr;

	return it->second;
}

void FE2D::ResourceCache::remove_resource(FE2D::UUID uuid) {
	auto it = m_StoredResources.find(uuid);
	if (it != m_StoredResources.end()) {
		it->second->Release();
		delete it->second;
		m_StoredResources.erase(it);
	}
}

void FE2D::ResourceCache::remove_metadata(FE2D::UUID uuid) noexcept {
	auto it = m_StoredMetadata.find(uuid);
	if (it != m_StoredMetadata.end())
		m_StoredMetadata.erase(it);
}

void FE2D::ResourceCache::remove_fallback(size_t hash_code) {
	auto it = m_StoredFallbacks.find(hash_code);
	if (it != m_StoredFallbacks.end()) {
		it->second->Release();
		delete it->second;
		m_StoredFallbacks.erase(it);
	}
}

void FE2D::ResourceCache::clear_resources() {
	for (auto& [uuid, resource] : m_StoredResources) {
		resource->Release();
		delete resource;
	}
	m_StoredResources.clear();
}

void FE2D::ResourceCache::clear_metadata() noexcept {
	m_StoredMetadata.clear();
}

void FE2D::ResourceCache::clear_fallback() {
	for (auto& [hash_code, fallback] : m_StoredFallbacks) {
		fallback->Release();
		delete fallback;
	}
	m_StoredFallbacks.clear();
}
