#pragma once
#include <iostream>
#include <regex>
#include <chrono>
#include <sstream>
#include <memory>
#include <algorithm>
#include <thread>
#include <filesystem>
#include <fstream>
#include <mutex>

class Researcher {
private:
	std::string _directory_path;
	std::string _pattern;
	std::mutex _mutex;
public:
	explicit Researcher(std::string directory_path, std::string pattern);
	void execute();
	void singleFileNaiveSearching(const std::string& filePath, const std::regex& pattern_reg);
	std::regex buildRegFromPattern();
	std::string handleRegSpecials(size_t index);
	std::string stringFormat(std::string& toFormat);
	bool isTextFile(const std::string& file);
	unsigned long upRange(unsigned long position, unsigned long upLimit);
	unsigned long downRange(unsigned long position);
};