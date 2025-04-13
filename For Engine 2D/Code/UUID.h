#pragma once

namespace FE2D {
    class FOR_API UUID {
    public:
        UUID() : m_UUID(Generate()) {}
        UUID(uint64_t uuid) : m_UUID(uuid) {}
        UUID(const std::string& str) { m_UUID = std::stoull(str); }

        bool operator==(const UUID& other)const noexcept { return m_UUID == other.m_UUID; }
        bool operator!=(const UUID& other)const noexcept { return !(*this == other); }
        uint64_t get() const noexcept { return m_UUID; }
        
        std::string ToString()const noexcept { return std::to_string(m_UUID); }
        static UUID FromString(const std::string& str) { return UUID(std::stoull(str)); }

    private:
        uint64_t m_UUID = 0;

        static uint64_t Generate() {
            static std::random_device rd;
            static std::mt19937_64 eng(rd());
            static std::uniform_int_distribution<uint64_t> dist;
            return dist(eng);
        }
    };
}

namespace std {
    template<>
    struct hash<FE2D::UUID> {
        std::size_t operator()(const FE2D::UUID& uuid)const noexcept { return std::hash<uint64_t>()(uuid.get()); }
    };
}
