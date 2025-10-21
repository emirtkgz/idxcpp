#pragma once

#include <filesystem>
#include <fstream>

// TODO: Add a flag for endianness
// TODO: Data type consistency
// TODO: Columns can be bigger than size_t
// TODO: Add tests

namespace Idxcpp {

	enum IdxDataType {
		Unsigned_Byte = 0x08, // 1 Byte
		Byte		  = 0x09, // 1 Byte
		Short		  = 0x0B, // 2 Bytes
		Int			  = 0x0C, // 4 bytes
		Float		  = 0x0D, // 4 bytes
		Double		  = 0x0E  // 4 bytes
	};

	class Idx {
	public:
		explicit Idx(std::filesystem::path path);
		~Idx();

		// Copy semantics
		Idx(const Idx&);
		Idx& operator=(const Idx&);

		// Move semantics
		Idx(Idx&&) noexcept;
		Idx& operator=(Idx&&) noexcept;

	private:
		IdxDataType dataType;
		std::vector<char> data;
		std::uint64_t rows;
		std::size_t columns;
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

		dataType		= static_cast<IdxDataType>(mnumber[0]); // 3rd byte is the data type
		char nDimensions = mnumber[1];							// 4th byte is the number of dimensions

		std::vector<char> dimensions(nDimensions * 4);			// Each dimension size is represented with 4 bytes
		f.seekg(4);
		f.read(dimensions.data(), nDimensions * 4);

		memcpy(&rows, dimensions.data(), sizeof rows);			// 1st dimension will be the rows

		columns = 1;
		for (int i = 1; i < nDimensions; i++) {					// 2nd dimension and higher dimensions are represented together as columns
			std::uint64_t dimensionSize;
			memcpy(&dimensionSize, dimensions.data() + i * sizeof dimensionSize, sizeof dimensionSize);
			columns *= dimensionSize;
		}


	}

	Idx::~Idx() {
	}

	inline Idx::Idx(const Idx&) {
	}

	inline Idx& Idx::operator=(const Idx&) {
		// TODO: insert return statement here
	}

	inline Idx::Idx(Idx&&) noexcept {
	}

	inline Idx& Idx::operator=(Idx&&) noexcept {
		// TODO: insert return statement here
	}
}