#include "../../idxcpp.hpp"

#include <filesystem>
#include <iostream>

using namespace Idxcpp;

int main() {
	std::cout << std::filesystem::current_path() << std::endl;
	Idx trainData(".\\data\\train-images.idx3-ubyte");

	return 1;
}