#include "../../idxcpp.hpp"

#include <filesystem>
#include <iostream>

using namespace Idxcpp;

int main() {
	// Make sure your current working directory is correct
	std::cout << std::filesystem::current_path() << std::endl;	
	Idx trainData(".\\data\\train-images.idx3-ubyte");

	for (int i = 0; i < trainData.getDimensions()[1]; i++) {
		for (int j = 0; j < trainData.getDimensions()[2]; j++) {
			// Returns the pointer where the element is
			char* ptr = trainData[0][i][j];

			// Prints the data as 2 digit hex
			printf("%02x", *ptr & 0xff);
		}

		std::cout << std::endl;
	}

	return 1;
}