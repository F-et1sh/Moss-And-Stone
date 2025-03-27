#pragma once
#include "IPropertiesDrawable.h"

namespace FE2D {
	// ISelectable - an interface for classes, which can be selected and displayed in InspectorUI
	class FOR_API ISelectable : public IPropertiesDrawable {
	public:
		ISelectable() = default;
		~ISelectable() = default;

		// This function will be called one time when your object was selected
		// If you overide this function don't forget to call default like : FE2D::ISelectable::OnSelected();
		virtual void OnSelected	 () { m_Selected = true ; }

		// This function will be called one time when your object will stop be selected
		// If you overide this function don't forget to call default like : FE2D::ISelectable::OnDeselected();
		virtual void OnDeselected() { m_Selected = false; }

	public:
		// Is this ISelectable object selected
		bool m_Selected = false;
	};
}