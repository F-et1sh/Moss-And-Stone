#pragma once
#include <foroutput.hpp>
#include <vector>
#include <future>
#include <memory>
#include <algorithm>
#include <stdexcept>

namespace FE2D {
	// Type of Elements in this Dynamic Array | Capacity of Dynamic Array on Start
	template<typename T, size_t occupied_memory = 0>
	class dynamic_array {
	public:
		// Initialize the Data of the Dynamic Array
		dynamic_array() : mData(nullptr) { 
			mData = new T[occupied_memory]; 
			mCapacity = occupied_memory;
		}
		dynamic_array(const T* begin, const T* end) {
			if (begin == nullptr || end == nullptr || begin > end) {
				SAY("ERROR : Invalid range provided to dynamic_array constructor");
				throw std::invalid_argument("ERROR : Invalid range provided to dynamic_array constructor");
			}
			mCapacity = std::distance(begin, end);
			mSize = mCapacity;
			mData = new T[mCapacity];
			std::copy(begin, end, mData);
		}
		// Use clear() function
		~dynamic_array() { this->clear(); }

		// Add an Element to the Dynamic Array
		constexpr void add(const T& load) {
			mSize++;
			if (mSize > mCapacity) set_capacity(mSize);
			mData[this->last()] = load;
		}

		// Add an Element to the Dynamic Array
		constexpr void add(T&& load) {
			mSize++;
			if (mSize > mCapacity) set_capacity(mSize);
			mData[this->last()] = std::move(load);
		}

		// Add an Array to the Dynamic Array
		constexpr void add(const T* load, const size_t& array_size) {
			if (array_size != 0) {
				if (mSize + array_size > mCapacity) set_capacity(mSize + array_size);
				for (size_t i = mSize - 1; i < mSize + array_size - 1; i++) {
					mData[i] = load[i - mSize + 1];
				}
				mSize += array_size;
			}
		}

		// Add a Dynamic Array of Elements to the Dynamic Array
		constexpr void add(const dynamic_array<T>& load) {
			if (load.size() > 0) {
				if (mSize + load.size() > mCapacity) set_capacity(mSize + load.size());
				for (size_t i = mSize; i < mSize + load.size(); i++) mData[i] = std::move(load[i - mSize]);
				mSize += load.size();
			}
		}

		// Increase the Capacity of Dynamic Array
		constexpr void increase_capacity(const size_t& increase_by) {
			T* new_data = new T[mCapacity + increase_by];
			if (mData != nullptr) {
				std::copy(mData, mData + mSize, new_data);
				delete[] mData;
			}
			mData = std::move(new_data);
			mCapacity += increase_by;
		}

		// Set the Capacity of Dynamic Array
		constexpr void set_capacity(const size_t& new_capacity) {
			if (new_capacity == mCapacity) return;

			T* new_data = new T[new_capacity];
			if (mData) {
				for (size_t i = 0; i < mSize; i++) {
					new_data[i] = std::move(mData[i]);
				}
				delete[] mData;
			}
			mData = new_data;
			mCapacity = new_capacity;
		}

		// Remove an Element from Dynamic Array
		constexpr void remove(const size_t& index) {
			if (index < mSize) {
				for (size_t i = index; i < mSize - 1; i++) {
					mData[i] = std::move(mData[i + 1]);
				}
				mSize--;
			}
		}

		// Remove a few Elements from Dynamic Array
		constexpr void remove(const size_t& index_begin, const size_t& index_end) {
			if (index_begin <= index_end && index_end < mSize) {
				size_t count = index_end - index_begin + 1;
				for (size_t i = index_begin; i < mSize - count; i++) {
					mData[i] = std::move(mData[i + count]);
				}
				mSize -= count;
			}
		}

		// Remove Last Element
		constexpr void remove_last() { 
			if (mSize > 0) {
				mSize--;
			}
			else {
				SAY("ERROR : Failed to Remove last Element of Dynamic Array\nThere is no Elements\nSize : " << mSize << "\nCapacity : " << mCapacity);
				throw std::runtime_error("ERROR : Failed to Remove last Element of Dynamic Array\nThere is no Elements");
			}
		}

		// Swap two Elements in Dynamic Array
		constexpr void swap(const size_t& index_from, const size_t& index_to) { std::swap(mData[index_from], mData[index_to]); }

		// Free data of the Dynamic Array
		constexpr void clear() noexcept {
			if (mData) {
				delete[] mData;
				mData = nullptr;
			}
			mSize = 0;
			mCapacity = 0;
		}

		// Reset the Size of the Dynamic Array to zero
		// This operation does not deallocate memory or destroy the elements
		// You will lost access to default elements after reset() without re-adding them
		// But pointers will not be destroyed, you will have an access
		//
		// This function needed in Immediate Mode programs like Dear ImGui library
		constexpr void reset()noexcept { mSize = 0; }

		// If your dynamic_array<int, 5> arr is { 8, 1, 2, 5, 2 }
		// then you called reset(); and started to add that elements again, so you can call skip_element() function 
		//
		// arr.add(8); 
		// arr.add(1); 
		// arr.add(2); 
		// arr.skip_element(1) // you skipping the fourth element 
		// arr.add(2); 
		// 
		// You will get the same data
		constexpr void skip_element(const size_t& jump_amount) {
			if (mSize + jump_amount <= mCapacity) {
				mSize += jump_amount;
			}
			else {
				SAY("ERROR : Failed to Skip an Element of Dynamic Array\nSize : " << mSize << "\nCapacity : " << mCapacity << "\nJump Amount : " << jump_amount);
				throw std::runtime_error("ERROR : Failed to Skip an Element of Dynamic Array");
			}
		}

		// Get the Last Element of Dynamic Array
		inline const T& get_last() { return mData[this->last()]; }

		// Get Range of Elements as other dynamic_array
		inline auto get_range(const size_t& begin, const size_t& end) {
			if (begin <= end && end < mSize) {
				if (mData != nullptr) return dynamic_array<T>(mData + begin, mData + end + 1);
			}
			else {
				if (begin != end) {
					SAY("ERROR : Failed to Get Range of Dynamic Array\nSize : " << mSize << "\nRange : " << begin << " to " << end);
					throw std::runtime_error("ERROR : Failed to get Range of Dynamic Array");
				}
			}
		}

		// Get the Index of Last Element of Dynamic Array
		inline const auto last() const noexcept { return mSize - 1; }

		// Get the Size of Dynamic Array
		inline const auto size() const noexcept { return mSize; }

		// Get the Size of Dynamic Array as Bytes
		inline const auto size_in_bytes() const noexcept { return sizeof(T)*mSize; }

		// Get the Capacity of Dynamic Array
		inline const auto capacity() const noexcept { return mCapacity; }

		// Check and Get the Element of Dynamic Array
		inline T& at(const size_t index) const {
			if (index < mSize) return mData[index];
			else {
				SAY("ERROR : Index out of Range\nIndex : " << index << "\nSize : " << mSize << "\nCapacity : " << mCapacity);
				throw std::runtime_error("ERROR : Index out of Range");
			}
		}

		// Show all of Elements of Dynamic Array in Console
        constexpr void show_info() {
            std::cerr << "Type : " << typeid(T).name() << "\nSize : " << mSize << "\nCapacity : " << mCapacity << std::endl << std::endl;
            for (size_t i = 0; i < mSize; i++) std::cerr << i << " : " << mData[i] << std::endl;
            std::cerr << "\n" << std::endl;
        }

        // Show One of Elements of Dynamic Array in Console
        constexpr void show_info(const size_t id) {
            if (id >= 0) {
                if (id < mSize)               std::cerr << "Type : " << typeid(T).name() << "\nSize : " << mSize << "\nCapacity : " << mCapacity << std::endl,
                    std::cerr << id << " : " << mData[id] << std::endl;

                else if (id < mCapacity)     std::cerr << "Type : " << typeid(T).name() << "\nSize : " << mSize << "\nCapacity : " << mCapacity << std::endl,
                    std::cerr << id << " ( from Capacity ) : " << mData[id] << std::endl;

                else                         std::cerr << "Type : " << typeid(T).name() << "\nSize : " << mSize << "\nCapacity : " << mCapacity << std::endl,
                    std::cerr << id << " - out of Range" << std::endl;
            }
        }

        // Show all of Elements of Dynamic Array in Console - from Begin to End
        constexpr void show_info(const size_t id_begin, const size_t id_end) {
            if (id_begin < 0 || id_begin > id_end) std::cerr << "ERROR : Failed to show_info() of Dynamic Arrray\nWrong IDs ( begin/end ) : " << id_begin << "/" << id_end << std::endl;
            else
                if (id_end < mSize) {
                    std::cerr << "Type : " << typeid(T).name() << "\nSize : " << mSize << "\nCapacity : " << mCapacity << std::endl;
                    for (size_t i = id_begin; i < id_end; i++) std::cerr << i << " : " << mData[i] << std::endl;
                }
                else if (id_end < mCapacity) {
                    std::cerr << "Type : " << typeid(T).name() << "\nSize : " << mSize << "\nCapacity : " << mCapacity << std::endl;
                    for (size_t i = id_begin; i < id_end; i++) if (i < mSize) std::cerr << i << " : " << mData[i] << std::endl;
                    else std::cerr << i << " ( from Capacity ) : " << mData[i] << std::endl;
                }
                else                         std::cerr << "Type : " << typeid(T).name() << "\nSize : " << mSize << "\nCapacity : " << mCapacity << std::endl,
                    std::cerr << id_begin << "/" << id_end << " - out of Range" << std::endl;
        }

		// Iterators
		T* begin() { return mData; }
		T* end() { return mData + mSize; }

		const T* begin() const noexcept { return mData; }
		const T* end() const noexcept { return mData + mSize; }

		// Find something in this Dynamic Array. Returns an Iterator
		inline const auto find(const T find_for) const noexcept { return std::find(this->begin(), this->end(), find_for); }

		// Fast Load
		dynamic_array(const dynamic_array& other) {
			mData = new T[other.mCapacity];
			mSize = other.mSize;
			mCapacity = other.mCapacity;
			std::copy(other.mData, other.mData + other.mSize, mData);
		}

		// Copy Other Dynamic Array to This
		constexpr dynamic_array& operator=(const dynamic_array& other) {
			if (this != &other) {
				this->clear();
				mSize = other.mSize;
				mCapacity = other.mCapacity;
				mData = new T[mCapacity];
				std::copy(other.mData, other.mData + other.mSize, mData);
			}
			return *this;
		}

        constexpr dynamic_array& operator=(const T* load_array) {
            size_t size = sizeof(load_array) / sizeof(T);
            if (mData != load_array) {
                this->clear();
                mSize = size;
                mCapacity = size;
                mData = new T[mCapacity];
                std::copy(load_array, load_array + size, mData);
            }
            return *this;
        }

		// Get the Reference at Data of Dynamic Array
		inline T*& reference() const noexcept { return mData; }

		// Get Dynamic Array as Type which you Want
		template <typename asType>
		inline const asType as() {
			if (mData == nullptr) {
				SAY("ERROR : Cannot convert Dynamic Array mData to the Requested Type\nConversion from : " << typeid(T).name() << "\nConversion to : " << typeid(asType).name()
					<< "Data of Dynamic Array is nullptr");
				throw std::runtime_error("ERROR : Cannot convert Dynamic Array mData to the Requested Type\nData of Dynamic Array is nullptr");
			}
			else {
				if (!std::is_convertible_v<T, asType>) {
					SAY("ERROR : Cannot convert Dynamic Array mData to the Requested Type\nConversion from : " << typeid(T).name() << "\nConversion to : " << typeid(asType).name());
					throw std::runtime_error("ERROR : Cannot convert Dynamic Array mData to the Requested Type");
				}
				else return reinterpret_cast<asType>(mData);
			}
		}

		// Get Type of this Dynamic Array
		inline const std::type_info type() const noexcept { return typeid(T); }

		bool empty()const noexcept { return mSize == 0; }

		T& operator[](const size_t index) {
			if (index < mSize) return mData[index];
			else {
				SAY("ERROR : Index out of Range\nIndex : " << index << "\nSize : " << mSize << "\nCapacity : " << mCapacity);
				throw std::runtime_error("ERROR : Index out of Range");
			}
		}

	private:
		// Data of Dynamic Array
		mutable T* mData = nullptr;

	private:
		// Size of Initialized Elements of Dynamic Array
		mutable size_t mSize = 0;

		// Capacity or Real Occupied Memory of Dynamic Array
		mutable size_t mCapacity = 0;
	};
}