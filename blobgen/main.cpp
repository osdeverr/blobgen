#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include "fmt/format.h"

namespace fs = std::filesystem;
namespace util
{
	std::vector<char> read_bytes(std::istream& is)
	{
		return std::vector<char>(std::istreambuf_iterator<char>(is),
			std::istreambuf_iterator<char>());
	}
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		fmt::print("Usage: {} <filename> [output filename]", argv[0]);
		std::exit(-1);
	}
	else
	{
		std::string in = argv[1];
		std::string in_replaced = fs::path(in).filename().generic_string();
		std::replace_if(in_replaced.begin(), in_replaced.end(), [](char c) { return !isalnum(c); }, '_');

		std::string out = fmt::format("blob-{}.h", in_replaced);
		if (argc >= 3)
			out = argv[2];
		
		std::ifstream inFile(in, std::ios_base::binary);
		if (!inFile)
		{
			fmt::print("ERROR - can't open file {} for reading!", in);
			std::exit(-1);
		}

		auto bytes = util::read_bytes(inFile);

		std::ofstream outFile(out);

		outFile << fmt::format(
"#include <array>\n\n\
namespace blobs {{\n\
\tconstexpr std::array<unsigned char, {}> {} = {{ \n", bytes.size(), in_replaced);

		for (auto& b : bytes)
		{
			outFile << fmt::format("\t\t{:#04x},\n", (unsigned char) b);
		}

		outFile << fmt::format(
"\t}}\n\
}}");

		fmt::print("{} -> {}", in, out);
	}
}