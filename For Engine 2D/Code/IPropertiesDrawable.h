#pragma once

namespace FE2D {
	// IPropertiesDrawable - an interface for classes, which can draw and change their properties in InspectorUI
	class FOR_API IPropertiesDrawable {
	public:
		IPropertiesDrawable() = default;
		~IPropertiesDrawable() = default;

		// Use FE2D::IMGUI to draw and change your properties
		// They will be drawn in InspectorUI
		virtual void show_properties() {}
	};
}