#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>

// TODO: Add a flag for endianness
// TODO: Data type consistency
// TODO: Columns can be bigger than size_t
// TODO: Add tests
// TODO: Configure CMake
// TODO: Data endianness
// TODO: Optimize
// TODO: Backward c++ compability

namespace {
	// From https://stackoverflow.com/a/4956493/15394064
	template <typename T>
	T swap_endian(T u) {
		static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");

		union {
			T u;
			unsigned char u8[sizeof(T)];
		} source, dest;

		source.u = u;

		for (size_t k = 0; k < sizeof(T); k++)
			dest.u8[k] = source.u8[sizeof(T) - k - 1];

		return dest.u;
	}
}

namespace Idxcpp {

	// Forward declarations
	class Idx;
	class IdxAccessor;

	enum IdxDataType {
		Unsigned_Byte = 0x08, // 1 Byte
		Byte		  = 0x09, // 1 Byte
		Short		  = 0x0B, // 2 Bytes
		Int			  = 0x0C, // 4 bytes
		Float		  = 0x0D, // 4 bytes
		Double		  = 0x0E  // 4 bytes
	};

	class IdxAccessor {
	friend class Idx;
	public:
		IdxAccessor(Idx* idx, int scope, int i, char* ptr);
		~IdxAccessor() = default;
		
		size_t getSize() const noexcept { return size; }

		void printData() const noexcept;

		IdxAccessor operator[](int i);

		// Conversion operators
		operator unsigned char* () { return reinterpret_cast<unsigned char*>(ptr); }
		operator char* ()		   { return ptr; }
		operator short* ()		   { return reinterpret_cast<short*>(ptr); }
		operator int* ()		   { return reinterpret_cast<int*>(ptr); }
		operator float* ()		   { return reinterpret_cast<float*>(ptr); }
		operator double* ()		   { return reinterpret_cast<double*>(ptr); }


		// Copy semantics
		IdxAccessor(const IdxAccessor&)			   = default;
		IdxAccessor& operator=(const IdxAccessor&) = default;

		// Move semantics
		IdxAccessor(IdxAccessor&&) noexcept			   = default;
		IdxAccessor& operator=(IdxAccessor&&) noexcept = default;
	private:
		Idx* idx;
		int scope;
		char* ptr;
		size_t size;
	};

	class Idx {
	friend class IdxAccessor;
	public:
		explicit Idx(std::filesystem::path path);
		~Idx() = default;

		IdxAccessor operator[](int i);

		// Returns the number of rows
		std::uint32_t getRows() const noexcept { return dimensions[0]; }
		// Returns the number of columns
		size_t getColumns() const noexcept { return columns; }
		// Returns the vector holding the size of each dimension
		const std::vector<std::uint32_t>& getDimensions() const noexcept { return dimensions; }
		// Returns the data type (IdxDataType Enum)
		IdxDataType getDataType() const noexcept { return dataType; }
		// Returns the vector holding the data
		std::vector<char>& getData() noexcept { return data; }


		// Copy semantics
		Idx(const Idx&)				   = default;
		Idx& operator=(const Idx&)	   = default;

		// Move semantics
		Idx(Idx&&) noexcept			   = default;
		Idx& operator=(Idx&&) noexcept = default;

	private:
		IdxDataType dataType;
		std::vector<char> data;
		std::vector<std::uint32_t> dimensions;
		std::size_t columns;

		int dataTypeSize() const noexcept;
	};

	Idx::Idx(std::filesystem::path path) {

		std::ifstream f(path, std::ios::binary);
		if (!f.is_open())
			throw std::exception("Could not open the IDX file!");

		// Read the magic number
		char mnumber[2];
		f.seekg(2); // first 2 bytes are always zero so they can be skipped
		f.read(mnumber, 2);

		if (!f.good())
			throw std::exception("Could not read the IDX magic number!");

		dataType		 = static_cast<IdxDataType>(mnumber[0]); // 3rd byte is the data type
		char nDimensions = mnumber[1];							 // 4th byte is the number of dimensions
		dimensions.resize(nDimensions);

		// Each dimension size is represented with 4 bytes
		f.seekg(4);
		f.read((char*)dimensions.data(), nDimensions * 4);		 // 1st dimension will be the rows

		for (auto& dim : dimensions) {							 // Data is in big-endian
			dim = swap_endian<std::uint32_t>(dim);
		}

		columns = 1;
		for (int i = 1; i < nDimensions; i++) {					 // 2nd dimension and higher dimensions are represented together as columns
			columns *= dimensions[i];
		}

		int typeSize = dataTypeSize();
		size_t dataSize = typeSize * getRows() * columns;
		data.resize(dataSize);
		f.seekg(4 + nDimensions * 4);
		f.read(data.data(), dataSize);
	}

	inline IdxAccessor Idx::operator[](int i) {
		return IdxAccessor(this, 1, i, data.data());
	}

	inline int Idx::dataTypeSize() const noexcept {
		switch (dataType) {
		case Unsigned_Byte:
			return 1;
			break;
		case Byte:
			return 1;
			break;
		case Short:
			return 2;
			break;

		return 4;
		}
	}

	IdxAccessor::IdxAccessor(Idx* idx, int scope, int i, char* ptr) : idx(idx), scope(scope), size(1), ptr(ptr) {
		for (int j = scope; j < idx->getDimensions().size(); j++) {
			size *= idx->getDimensions()[j];
		}

		this->ptr += i * size;
	}

	// Intended for debug purposes
	inline void IdxAccessor::printData() const noexcept {
		for (int i = 0; i < size; i++) {
			std::cout << *((char*)ptr + i) << std::endl;
		}
	}

	inline IdxAccessor IdxAccessor::operator[](int i) {
		return IdxAccessor(idx, scope + 1, i, ptr);
	}
}