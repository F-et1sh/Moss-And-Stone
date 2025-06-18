#pragma once
#include "ISystemBase.h"

namespace FE2D {
	class FOR_API ScriptManagerSystem : public ISystemBase {
	public:
		ScriptManagerSystem() = default;
		~ScriptManagerSystem() = default;

		void Initialize();

		void OnEnd();
		void OnStart();
		void OnUpdate();

	private:

	};
}