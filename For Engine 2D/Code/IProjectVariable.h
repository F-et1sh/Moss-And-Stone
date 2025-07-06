#pragma once

namespace FE2D {
	class FOR_API IProjectVariable {
	public:
		IProjectVariable() = default;
		virtual ~IProjectVariable() = default;

		virtual json Serialize()const = 0;
		virtual void Deserialize(const json& j) = 0;
	};
}