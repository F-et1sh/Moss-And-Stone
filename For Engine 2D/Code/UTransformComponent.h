#pragma once
#include "Component.h"

namespace FE2D {
	class FOR_API UTransformComponent : public UComponent {
	public:
		UTransformComponent() = default;
		~UTransformComponent() = default;
	private:
		// If it's true the actor will stop calculate the transform matrix
		// It will start to use last calculated matrix
		// You will not be able to change it's transform
		bool m_IsStatic = false;
    private:
        vec2 m_Position = vec2();

        int m_Layer = 0.0f;

        // Flag for Auto Sprite Sorting
        bool m_AutoSort = false;

        vec2 m_Scale = vec2(1.0f, 1.0f);

        float m_Rotation = 0.0f;
        vec2  m_Origin = vec2();

    public:
        inline FOR_NODISCARD const vec2 getPosition() const noexcept { return m_Position; }
        inline FOR_NODISCARD float      getLayer   () const noexcept { return m_Layer   ; }
        inline FOR_NODISCARD bool       getAutoSort() const noexcept { return m_AutoSort; }
        inline FOR_NODISCARD const vec2 getScale   () const noexcept { return m_Scale   ; }
        inline FOR_NODISCARD float      getRotation() const noexcept { return m_Rotation; }
        inline FOR_NODISCARD const vec2 getOrigin  () const noexcept { return m_Origin  ; }

        inline void setPosition(const vec2  position)noexcept { if (!this->m_IsStatic) m_Position = position, m_IsTransformChanged = true; }
        inline void setLayer   (const float layer   )noexcept { if (!this->m_IsStatic) m_Layer    = layer   , m_IsTransformChanged = true; }
        inline void setAutoSort(const bool  autoSort)noexcept { if (!this->m_IsStatic) m_AutoSort = autoSort, m_IsTransformChanged = true; }
        inline void setScale   (const vec2  scale   )noexcept { if (!this->m_IsStatic) m_Scale    = scale   , m_IsTransformChanged = true; }
        inline void setRotation(const float rotation)noexcept { if (!this->m_IsStatic) m_Rotation = rotation, m_IsTransformChanged = true; }
        inline void setOrigin  (const vec2  origin  )noexcept { if (!this->m_IsStatic) m_Origin   = origin  , m_IsTransformChanged = true; }

    private:
        // do NOT use this matrix, instead use getTransform() function
        // the matrix updates only if you recalculate it using RecalculateTransform()
        // getTransform() function will automatically recalculate this matrix if it's needed
        mat4 m_Matrix = mat4();

        // if you use any setter of transform like setPosition(), setLayer etc this flag will become true
        // if this flag is true in getTransform() m_Matrix will be recalculated with RecalculateTransform()
        bool m_IsTransformChanged = false;
    public:
        inline void RecalculateTransform()noexcept;
    public:
        // this function will recalculate matrix of actor if it's needed
        inline const mat4& getTransform()noexcept;

    public:
        json Serialize()const override;
        void Deserialize(const json& j)override;

		void show_properties()override;
	};
}