#pragma once
#include "Actor.h"
#include "InspectorUI.h"

namespace FE2D {
	// ActorListUI can only display AActors of bound ActorList and select them
	class FOR_API ActorListUI {
	public:
		ActorListUI() = default;
		~ActorListUI() = default;
	public:
		void Release();
		void Initialize(std::vector<AActor*>* load_actorList, InspectorUI* load_inspectorUI)noexcept;

		void Update();
	private:
		void processSelected();

		void actorPopup(bool isItemHovered);
		void windowPopup();
	private:
		std::vector<AActor*>* m_ActorList = nullptr;
		bool m_IsRenaming = false;
	private:
		InspectorUI* m_InspectorUI = nullptr;
	private:
		AActor* m_SelectedActor = nullptr;
	};
}