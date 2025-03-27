#pragma once

#include <math.h>
#include <algorithm>

namespace FE2D {

	// FOR_P, FOR_RADIANS
#pragma region Global Variables

// P Number
	constexpr double FOR_P = 3.14159265358979323846;
	// Multiply by this if you want to convert Degrees to Radians
	constexpr double FOR_RADIAN = FOR_P / 180.0f;

#pragma endregion

	// Pre-Declarations of Functions
#pragma region Pre-Declarations
	struct FOR_API vec2;
	struct FOR_API vec3;
	struct FOR_API vec4;

	struct FOR_API mat4;
	struct FOR_API mat3;
	struct FOR_API mat2;

	static inline const mat4 translate(const vec3 translation);
	static inline const mat4 rotate(const vec3 rotation);
	static inline const mat4 scale(const vec3 scaling);

	static inline const mat4 lookAt(const vec3 eye, const vec3 center, const vec3 up);

	static inline const mat4 perspective(const float fov, const float aspect, const float zNear, const float zFar);

	static inline const mat4 ortho(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);

	static inline const float dot(const vec2 vec_a, const vec2 vec_b)noexcept;
	static inline const float dot(const vec3 vec_a, const vec3 vec_b)noexcept;
	static inline const float dot(const vec4 vec_a, const vec4 vec_b)noexcept;

	static inline const vec2 normalize(const vec2 load_vec2)noexcept;
	static inline const vec3 normalize(const vec3 load_vec3)noexcept;
	static inline const vec4 normalize(const vec4 load_vec4)noexcept;

	static inline const float distance(vec2 pos_a, vec2 pos_b)noexcept;

	static inline const double to_radians(const double degrees)noexcept;
	static inline const double to_degrees(const double radians)noexcept;

	static inline const vec3 cross(vec3 x, vec3 y)noexcept;

	// An example : if you have value 3.333333333 and want to get 3.33, you need to put 100 as the second argument
	static inline const float remove_numbers_after_point(const float value, const unsigned int count_of_numbers_after_point)noexcept;
	// An example : if you have value 3.333333333 and want to get 3.33, you need to put 100 as the second argument
	static inline const vec2 remove_numbers_after_point(const vec2 value, const unsigned int count_of_numbers_after_point)noexcept;
	// An example : if you have value 3.333333333 and want to get 3.33, you need to put 100 as the second argument
	static inline const vec3 remove_numbers_after_point(const vec3 value, const unsigned int count_of_numbers_after_point)noexcept;
	// An example : if you have value 3.333333333 and want to get 3.33, you need to put 100 as the second argument
	static inline const vec4 remove_numbers_after_point(const vec4 value, const unsigned int count_of_numbers_after_point)noexcept;

	static inline const vec2 abs(const vec2 value)noexcept;
	static inline const vec3 abs(const vec3 value)noexcept;
	static inline const vec4 abs(const vec4 value)noexcept;

#pragma endregion

	// vec2, vec3, vec4
#pragma region Vectors

	// Vector 2 Float
	struct FOR_API vec2 {
		float x = 0.0f;
		float y = 0.0f;

		constexpr vec2() = default;
		constexpr ~vec2() = default;

		constexpr vec2(auto x, auto y) : x(static_cast<float>(x)), y(static_cast<float>(y)) {}
		constexpr vec2(auto xy) : x(static_cast<float>(xy)), y(static_cast<float>(xy)) {}

		//

		explicit operator bool() { return x != 0.0f || y != 0.0f; }

		//

		vec2 operator-() const { return vec2(-this->x, -this->y); }
		vec2 operator+() const { return vec2(this->x, this->y); }

		//

		vec2& operator++() { this->x++; this->y++; return *this; }

		//

		vec2& operator--() { this->x--; this->y--; return *this; }

		//

		// Here is might be the problem if you quate vec2 to vec2*
		vec2& operator=(auto load) { this->x = load;    this->y = load;    return *this; }


		vec2& operator=(vec2 load) { this->x = load.x; this->y = load.y; return *this; }

		//

		vec2& operator+=(vec2 load) {
			this->x += load.x;
			this->y += load.y;
			return *this;
		}
		vec2& operator-=(vec2 load) {
			this->x -= load.x;
			this->y -= load.y;
			return *this;
		}
		vec2& operator*=(vec2 load) {
			this->x *= load.x;
			this->y *= load.y;
			return *this;
		}
		vec2& operator/=(vec2 load) {
			this->x /= load.x;
			this->y /= load.y;
			return *this;
		}

		//

		vec2 operator+(vec2 load)const {
			return vec2(this->x + load.x,
				this->y + load.y);
		}
		vec2 operator-(vec2 load)const {
			return vec2(this->x - load.x,
				this->y - load.y);
		}
		vec2 operator*(vec2 load)const {
			return vec2(this->x * load.x,
				this->y * load.y);
		}
		vec2 operator/(vec2 load)const {
			return vec2(this->x / load.x,
				this->y / load.y);
		}

		//

		vec2 operator+(auto load)const {
			return vec2(this->x + load,
				this->y + load);
		}
		vec2 operator-(auto load)const {
			return vec2(this->x - load,
				this->y - load);
		}
		vec2 operator*(auto load)const {
			return vec2(this->x * load,
				this->y * load);
		}
		vec2 operator/(auto load)const {
			return vec2(this->x / load,
				this->y / load);
		}

		//

		vec2& operator+=(auto load) {
			this->x += load;
			this->y += load;
			return *this;
		}
		vec2& operator-=(auto load) {
			this->x -= load;
			this->y -= load;
			return *this;
		}
		vec2& operator*=(auto load) {
			this->x *= load;
			this->y *= load;
			return *this;
		}
		vec2& operator/=(auto load) {
			this->x /= load;
			this->y /= load;
			return *this;
		}

		//

		bool operator>(vec2 load) const { return this->x > load.x && this->y > load.y; }
		bool operator<(vec2 load) const { return this->x < load.x && this->y < load.y; }

		//

		bool operator>=(vec2 load) const { return this->x >= load.x && this->y >= load.y; }
		bool operator<=(vec2 load) const { return this->x <= load.x && this->y <= load.y; }

		//

		bool operator==(vec2 load) const { return this->x == load.x && this->y == load.y; }
		bool operator!=(vec2 load) const { return this->x != load.x || this->y != load.y; }
	};

	// Vector 3 Float
	struct FOR_API vec3 {
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;

		vec3() = default;
		~vec3() = default;

		constexpr vec3(auto x, auto y, auto z) : x(static_cast<float>(x)), y(static_cast<float>(y)), z(static_cast<float>(z)) {}
		constexpr vec3(auto x, vec2 yz) : x(static_cast<float>(x)), y(static_cast<float>(yz.x)), z(static_cast<float>(yz.y)) {}
		constexpr vec3(vec2 xy, auto z) : x(static_cast<float>(xy.x)), y(static_cast<float>(xy.y)), z(static_cast<float>(z)) {}
		constexpr vec3(auto xyz) : x(static_cast<float>(xyz)), y(static_cast<float>(xyz)), z(static_cast<float>(xyz)) {}

		//

		explicit operator bool() { return x != 0.0f || y != 0.0f || z != 0.0f; }

		//

		vec3 operator-() const { return vec3(-this->x, -this->y, -this->z); }
		vec3 operator+() const { return vec3(this->x, this->y, this->z); }

		//

		vec3& operator++() { this->x++; this->y++, this->z++; return *this; }

		//

		vec3& operator--() { this->x--; this->y--; this->z--; return *this; }

		//

		vec3& operator=(auto load) { this->x = load; this->y = load; this->z = load; return *this; }
		vec3& operator=(vec3  load) { this->x = load.x; this->y = load.y; this->z = load.z; return *this; }

		//

		vec3& operator+=(vec3 load) {
			this->x += load.x;
			this->y += load.y;
			this->z += load.z;
			return *this;
		}
		vec3& operator-=(vec3 load) {
			this->x -= load.x;
			this->y -= load.y;
			this->z -= load.z;
			return *this;
		}
		vec3& operator*=(vec3 load) {
			this->x *= load.x;
			this->y *= load.y;
			this->z *= load.z;
			return *this;
		}
		vec3& operator/=(vec3 load) {
			this->x /= load.x;
			this->y /= load.y;
			this->z /= load.z;
			return *this;
		}

		//

		vec3 operator+(vec3 load)const {
			return vec3(this->x + load.x,
				this->y + load.y,
				this->z + load.z);
		}
		vec3 operator-(vec3 load)const {
			return vec3(this->x - load.x,
				this->y - load.y,
				this->z - load.z);
		}
		vec3 operator*(vec3 load)const {
			return vec3(this->x * load.x,
				this->y * load.y,
				this->z * load.z);
		}
		vec3 operator/(vec3 load)const {
			return vec3(this->x / load.x,
				this->y / load.y,
				this->z / load.z);
		}

		//

		vec3 operator+(auto load)const {
			return vec3(this->x + load,
				this->y + load,
				this->z + load);
		}
		vec3 operator-(auto load)const {
			return vec3(this->x - load,
				this->y - load,
				this->z - load);
		}
		vec3 operator*(auto load)const {
			return vec3(this->x * load,
				this->y * load,
				this->z * load);
		}
		vec3 operator/(auto load)const {
			return vec3(this->x / load,
				this->y / load,
				this->z / load);
		}

		//

		vec3& operator+=(auto load) {
			this->x += load;
			this->y += load;
			this->z += load;
			return *this;
		}
		vec3& operator-=(auto load) {
			this->x -= load;
			this->y -= load;
			this->z -= load;
			return *this;
		}
		vec3& operator*=(auto load) {
			this->x *= load;
			this->y *= load;
			this->z *= load;
			return *this;
		}
		vec3& operator/=(auto load) {
			this->x /= load;
			this->y /= load;
			this->z /= load;
			return *this;
		}

		//

		bool operator>(vec3 load) const { return this->x > load.x && this->y > load.y && this->z > load.z; }
		bool operator<(vec3 load) const { return this->x < load.x && this->y < load.y && this->z < load.z; }

		//

		bool operator<=(vec3 load) const { return this->x <= load.x && this->y <= load.y && this->z <= load.z; }
		bool operator>=(vec3 load) const { return this->x >= load.x && this->y >= load.y && this->z >= load.z; }

		//

		bool operator==(vec3 load) const { return this->x == load.x && this->y == load.y && this->z == load.z; }
		bool operator!=(vec3 load) const { return this->x != load.x || this->y != load.y || this->z != load.z; }
	};

	// Vector 4 Float
	struct FOR_API vec4 {
		float x = 0.0f; float y = 0.0f; float z = 0.0f; float w = 0.0f;
		vec4() = default;
		~vec4() = default;

		constexpr vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

		constexpr vec4(float load) : x(load), y(load), z(load), w(load) {}

		constexpr vec4(vec2  load) : x(load.x), y(load.y) {}
		constexpr vec4(vec3  load) : x(load.x), y(load.y), z(load.z) {}

		constexpr vec4(vec3  load, float w) : x(load.x), y(load.y), z(load.z), w(w) {}
		constexpr vec4(vec2  load, float z, float w) : x(load.x), y(load.y), z(z), w(w) {}
		constexpr vec4(float x, float y, vec2  load) : x(x), y(y), z(load.x), w(load.y) {}

		constexpr vec4(vec2  load0, vec2  load1) : x(load0.x), y(load0.y), z(load1.x), w(load1.y) {}

		//

		vec4 operator+(vec4 load)const {
			return vec4(this->x + load.x,
				this->y + load.y,
				this->z + load.z,
				this->w + load.w);
		}

		vec4 operator-(vec4 load)const {
			return vec4(this->x - load.x,
				this->y - load.y,
				this->z - load.z,
				this->w - load.w);
		}

		vec4 operator*(vec4 load)const {
			return vec4(this->x * load.x,
				this->y * load.y,
				this->z * load.z,
				this->w * load.w);
		}

		vec4 operator/(vec4 load)const {
			return vec4(this->x / load.x,
				this->y / load.y,
				this->z / load.z,
				this->w / load.w);
		}

		//

		vec4& operator+=(vec4 load) {
			this->x += load.x;
			this->y += load.y;
			this->z += load.z;
			this->w += load.w;
			return *this;
		}

		vec4& operator-=(vec4 load) {
			this->x -= load.x;
			this->y -= load.y;
			this->z -= load.z;
			this->w -= load.w;
			return *this;
		}

		vec4& operator*=(vec4 load) {
			this->x *= load.x;
			this->y *= load.y;
			this->z *= load.z;
			this->w *= load.w;
			return *this;
		}

		vec4& operator/=(vec4 load) {
			this->x /= load.x;
			this->y /= load.y;
			this->z /= load.z;
			this->w /= load.w;
			return *this;
		}

		bool operator>(vec4 load) const { if (this->x > load.x && this->y > load.y && this->z > load.z && this->w > load.w) return true; else return false; }
		bool operator<(vec4 load) const { if (this->x < load.x && this->y < load.y && this->z < load.z && this->w < load.w) return true; else return false; }

		bool operator==(vec4 load) const { if (this->x == load.x && this->y == load.y && this->z == load.z && this->w == load.w) return true; else return false; }
		bool operator!=(vec4 load) const { if (this->x != load.x && this->y != load.y && this->z != load.z && this->w != load.w) return true; else return false; }

		//

		vec4& operator=(vec4 load) { this->x = load.x; this->y = load.y; this->z = load.z; this->w = load.w; return *this; }
		vec4& operator=(vec3 load) { this->x = load.x; this->y = load.y; this->z = load.z; return *this; }
		vec4& operator=(vec2 load) { this->x = load.x; this->y = load.y; return *this; }

		vec4& operator=(float load) { this->x = load; this->y = load; this->z = load; this->w = load; return *this; }
	};

#pragma endregion

	// mat4, mat3, mat2, lookAt, perspective, ortho
#pragma region Matrices

// Matrix 4x4
	struct FOR_API mat4 {
	private:
		union {
			float f[16];
			float m[4][4];
		};
	public:
		mat4() : f{ 1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f } {
		}

		mat4(float diagonally_nums) : f{ diagonally_nums, 0.0f, 0.0f, 0.0f,
										 0.0f, diagonally_nums, 0.0f, 0.0f,
										 0.0f, 0.0f, diagonally_nums, 0.0f,
										 0.0f, 0.0f, 0.0f, diagonally_nums } {
		}

		~mat4() = default;

		mat4(float colx1, float coly1, float colz1, float colw1,
			float colx2, float coly2, float colz2, float colw2,
			float colx3, float coly3, float colz3, float colw3,
			float colx4, float coly4, float colz4, float colw4) : f{ colx1, coly1, colz1, colw1,
																	 colx2, coly2, colz2, colw2,
																	 colx3, coly3, colz3, colw3,
																	 colx4, coly4, colz4, colw4 } {
		}

		mat4(float cols[16]) : f{ cols[0],  cols[1],  cols[2],  cols[3],
								  cols[4],  cols[5],  cols[6],  cols[7],
								  cols[8],  cols[9],  cols[10], cols[11],
								  cols[12], cols[13], cols[14], cols[15] } {
		}

		const mat4& operator*(const mat4& other)const noexcept {
			mat4 result = mat4(1.0f);
			for (unsigned int i = 0; i < 4; i++) {
				for (unsigned int j = 0; j < 4; j++) {
					result[i][j] = m[i][0] * other.m[0][j] +
						m[i][1] * other.m[1][j] +
						m[i][2] * other.m[2][j] +
						m[i][3] * other.m[3][j];
				}
			}
			return result;
		}


		mat4& operator*=(const mat4& other) noexcept { *this = *this * other; return *this; }

		float* operator[](int index) { return m[index]; }
		const float* operator[](int index) const { return m[index]; }
		inline operator const float* () const { return f; }
	};

	// Matrix 3x3
	struct FOR_API mat3 {
	private:
		float matrix[9];
	public:

		mat3() : matrix{ 1.0f, 0.0f, 0.0f,
						 0.0f, 1.0f, 0.0f,
						 0.0f, 0.0f, 1.0f } {
		}

		~mat3() = default;

		mat3(float colx1, float coly1, float colz1,
			float colx2, float coly2, float colz2,
			float colx3, float coly3, float colz3) : matrix{ colx1, coly1, colz1,
															 colx2, coly2, colz2,
															 colx3, coly3, colz3 } {
		}

		mat3(float cols[9]) : matrix{ cols[0],  cols[1],  cols[2],
									  cols[4],  cols[5],  cols[6],
									  cols[8],  cols[9],  cols[10] } {
		}

		mat3 operator+=(mat3 load) { for (int i = 0; i < 9; i++) this->matrix[i] += load.matrix[i]; return *this; }
		mat3 operator*=(mat3 load) { for (int i = 0; i < 9; i++) this->matrix[i] *= load.matrix[i]; return *this; }
		mat3 operator/=(mat3 load) { for (int i = 0; i < 9; i++) this->matrix[i] /= load.matrix[i]; return *this; }

		inline float& operator[](int index) { return matrix[index]; }
		inline float& operator()(unsigned int a, unsigned int b) { return matrix[a * 3 + b]; }
		inline operator const float* () const { return matrix; }
	};

	// Matrix 2x2
	struct FOR_API mat2 {
	private:
		float matrix[4];
	public:
		mat2() : matrix{ 1.0f, 0.0f,
						 0.0f, 1.0f } {
		}

		mat2(float colX_1, float colX_2,
			float colY_1, float colY_2) : matrix{ colX_1, colX_2,
												 colY_1, colY_2 } {
		}

		mat2(vec2 col_1_2_X,
			vec2 col_1_2_Y) : matrix{ col_1_2_X.x, col_1_2_X.y,
									 col_1_2_Y.x, col_1_2_Y.y } {
		}

		mat2(float cols[4]) : matrix{ cols[0], cols[1],
									  cols[2], cols[3] } {
		}

		float& operator[](int index) { return matrix[index]; }

		inline operator const float* () const { return matrix; }
	};

	// Translate
	const mat4 translate(const vec3 translation) {
		mat4 load_result = mat4(1.0f);
		load_result[3][0] = translation.x;
		load_result[3][1] = translation.y;
		load_result[3][2] = translation.z;
		return load_result;
	}

	// Rotate
	const mat4 rotate(const vec3 rotation) {
		mat4 load_result = mat4(1.0f);

		float cos_x = cos(rotation.x);
		float sin_x = sin(rotation.x);

		float cos_y = cos(rotation.y);
		float sin_y = sin(rotation.y);

		float cos_z = cos(rotation.z);
		float sin_z = sin(rotation.z);

		load_result = mat4(cos_z * cos_y, -sin_z, sin_y, 0.0f,
			sin_z, cos_z * cos_x, -sin_x, 0.0f,
			-sin_y, sin_x, cos_y * cos_x, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		return load_result;
	}

	// Scale
	const mat4 scale(const vec3 scaling) {
		mat4 load_result = mat4(1.0f);
		load_result[0][0] = scaling.x;
		load_result[1][1] = scaling.y;
		load_result[2][2] = scaling.z;
		return load_result;
	}

	// Look At
	const mat4 lookAt(const vec3 eye, const vec3 center, const vec3 up) {
		vec3 const f(normalize(center - eye));
		vec3 const s(normalize(cross(f, up)));
		vec3 const u(cross(s, f));

		mat4 load_result = mat4(1.0f);
		load_result[0][0] = s.x;
		load_result[1][0] = s.y;
		load_result[2][0] = s.z;
		load_result[0][1] = u.x;
		load_result[1][1] = u.y;
		load_result[2][1] = u.z;
		load_result[0][2] = -f.x;
		load_result[1][2] = -f.y;
		load_result[2][2] = -f.z;
		load_result[3][0] = -dot(s, eye);
		load_result[3][1] = -dot(u, eye);
		load_result[3][2] = dot(f, eye);
		return load_result;
	}

	// Perspective
	const mat4 perspective(const float fov, const float aspect, const float zNear, const float zFar) {
		float const tanHalfFovy = tan(fov / 2.0f);

		mat4 load_result = mat4(0.0f);
		load_result[0][0] = 1.0f / (aspect * tanHalfFovy);
		load_result[1][1] = 1.0f / (tanHalfFovy);
		load_result[2][2] = -(zFar + zNear) / (zFar - zNear);
		load_result[2][3] = -1.0f;
		load_result[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);
		return load_result;
	}

	// Ortho
	const mat4 ortho(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar) {
		mat4 load_result = mat4(1.0f);
		load_result[0][0] = 2.0f / (right - left);
		load_result[1][1] = 2.0f / (top - bottom);
		load_result[2][2] = -2.0f / (zFar - zNear);
		load_result[3][0] = -(right + left) / (right - left);
		load_result[3][1] = -(top + bottom) / (top - bottom);
		load_result[3][2] = -(zFar + zNear) / (zFar - zNear);
		return load_result;
	}

#pragma endregion

	// Functions of dot Product
#pragma region dot product
	const float dot(const vec2 vec_a, const vec2 vec_b)noexcept { return vec_a.x * vec_b.x + vec_a.y * vec_b.y; }
	const float dot(const vec3 vec_a, const vec3 vec_b)noexcept { return vec_a.x * vec_b.x + vec_a.y * vec_b.y + vec_a.z * vec_b.z; }
	const float dot(const vec4 vec_a, const vec4 vec_b)noexcept { return vec_a.x * vec_b.x + vec_a.y * vec_b.y + vec_a.z * vec_b.z + vec_a.w * vec_b.w; }
#pragma endregion

	// Functions of Normalize
#pragma region Normalize
	const vec2 normalize(const vec2 load_vec2)noexcept {
		float length = sqrt(load_vec2.x * load_vec2.x + load_vec2.y * load_vec2.y);
		if (length != 0) return vec2(load_vec2.x / length, load_vec2.y / length);
		return load_vec2;
	}
	const vec3 normalize(const vec3 load_vec3)noexcept {
		float length = sqrt(load_vec3.x * load_vec3.x + load_vec3.y * load_vec3.y + load_vec3.z * load_vec3.z);
		if (length != 0) return vec3(load_vec3.x / length, load_vec3.y / length, load_vec3.z / length);
		return load_vec3;
	}
	const vec4 normalize(const vec4 load_vec4)noexcept {
		float length = sqrt(load_vec4.x * load_vec4.x + load_vec4.y * load_vec4.y + load_vec4.z * load_vec4.z + load_vec4.w * load_vec4.w);
		if (length != 0) return vec4(load_vec4.x / length, load_vec4.y / length, load_vec4.z / length, load_vec4.w / length);
		return load_vec4;
	}
#pragma endregion

	// Function for Finding a Distance Between two Points ( vec2 )
#pragma region Distance
	const float distance(const vec2 pos_a, const vec2 pos_b)noexcept { return sqrt(dot(pos_a - pos_b, pos_a - pos_b)); }
#pragma endregion

	// Convert Degrees & Radians Function
#pragma region Convert
	const double to_radians(const double degrees)noexcept { return degrees * FOR_RADIAN; }
	const double to_degrees(const double radians)noexcept { return radians / FOR_RADIAN; }
#pragma endregion

	// Cross Product Function
#pragma region Cross
	const vec3 cross(vec3 x, vec3 y) noexcept {
		return vec3(x.y * y.z - y.y * x.z,
			x.z * y.x - y.z * x.x,
			x.x * y.y - y.x * x.y);
	}
#pragma endregion

	// Remove Numbers after Point
#pragma region Remove Numbers after Point
	const float remove_numbers_after_point(const float value, const unsigned int count_of_numbers_after_point)noexcept {
		//   3.3333333 | * 100
		// 333.3333300 | int
		// 333		   | float
		// 333.0000000 | / 100
		// 3.33
		return static_cast<float>(static_cast<int>(value * count_of_numbers_after_point)) / count_of_numbers_after_point;
	}
	const vec2 remove_numbers_after_point(const vec2 value, const unsigned int count_of_numbers_after_point)noexcept {
		//   3.3333333 | * 100
		// 333.3333300 | int
		// 333		   | float
		// 333.0000000 | / 100
		// 3.33
		return vec2(static_cast<float>(static_cast<int>(value.x * count_of_numbers_after_point)) / count_of_numbers_after_point,
			static_cast<float>(static_cast<int>(value.y * count_of_numbers_after_point)) / count_of_numbers_after_point);
	}
	const vec3 remove_numbers_after_point(const vec3 value, const unsigned int count_of_numbers_after_point)noexcept {
		//   3.3333333 | * 100
		// 333.3333300 | int
		// 333		   | float
		// 333.0000000 | / 100
		// 3.33
		return vec3(static_cast<float>(static_cast<int>(value.x * count_of_numbers_after_point)) / count_of_numbers_after_point,
			static_cast<float>(static_cast<int>(value.y * count_of_numbers_after_point)) / count_of_numbers_after_point,
			static_cast<float>(static_cast<int>(value.z * count_of_numbers_after_point)) / count_of_numbers_after_point);
	}
	const vec4 remove_numbers_after_point(const vec4 value, const unsigned int count_of_numbers_after_point)noexcept {
		//   3.3333333 | * 100
		// 333.3333300 | int
		// 333		   | float
		// 333.0000000 | / 100
		// 3.33
		return vec4(static_cast<float>(static_cast<int>(value.x * count_of_numbers_after_point)) / count_of_numbers_after_point,
			static_cast<float>(static_cast<int>(value.y * count_of_numbers_after_point)) / count_of_numbers_after_point,
			static_cast<float>(static_cast<int>(value.z * count_of_numbers_after_point)) / count_of_numbers_after_point,
			static_cast<float>(static_cast<int>(value.w * count_of_numbers_after_point)) / count_of_numbers_after_point);
	}
#pragma endregion

	// abs for vec2, vec3, vec4
#pragma region abs
	const vec2 abs(const vec2 value)noexcept { return vec2(std::abs(value.x), std::abs(value.y)); };
	const vec3 abs(const vec3 value)noexcept { return vec3(std::abs(value.x), std::abs(value.y), std::abs(value.z)); };
	const vec4 abs(const vec4 value)noexcept { return vec4(std::abs(value.x), std::abs(value.y), std::abs(value.z), std::abs(value.w)); };
#pragma endregion

}