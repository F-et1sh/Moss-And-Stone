#pragma once

namespace FE2D {
    template<typename T> requires std::is_base_of_v<IResource, T>
    struct FOR_API ResourceID {
        FE2D::UUID uuid = FE2D::UUID(0);

        explicit ResourceID(FE2D::UUID id) : uuid(id) {}

        ResourceID() = default;
        ~ResourceID() = default;
    };
}