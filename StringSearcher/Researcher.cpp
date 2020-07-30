#include "Researcher.h"
#include <future>
#include <utility>

Researcher::Researcher(std::string directory_path, std::string pattern):_directory_path(std::move(directory_path)),_pattern(std::move(pattern))
{}
void Researcher::execute()
{
	const std::regex r = buildRegFromPattern();
	if (isTextFile(this->_directory_path))
	{
		singleFileNaiveSearching(this->_directory_path, r);
	}
	else
	{
		//If directory path is not a text file then it is directory and if it is we create threads to take care of each file of subdirectories
		std::vector<std::thread> threads;
		std::cout << "Multithreading results : \n";
		for (const auto& file_path : std::filesystem::recursive_directory_iterator(this->_directory_path))
		{
			if (isTextFile(file_path.path().string()))
			{
				threads.emplace_back(&Researcher::singleFileNaiveSearching, this, file_path.path().string(), r);
			}
		}
		for (auto& t : threads)
			t.join();

		std::cout << "\n======================================\n";
		std::cout << "Single thread results :\n";
		for (const auto& file_path : std::filesystem::recursive_directory_iterator(this->_directory_path))
		{
			if (isTextFile(file_path.path().string()))
				singleFileNaiveSearching(file_path.path().string(), r);
		}
	}
}
void Researcher::singleFileNaiveSearching(const std::string& filePath, const std::regex& pattern_reg)
{
	std::ifstream file;
	file.open(filePath);
	if (file.fail())
	{
		const std::string errorMessage = "Error: " + filePath + " doesn't exist";
		throw std::runtime_error(errorMessage);
	}
	else
	{
		if (file.is_open())
		{
			auto start = std::chrono::high_resolution_clock::now();
			std::string text;
			text.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
			std::ostringstream buffer;
			bool f = false;
			//Iteration through all pattern matches in file and loading everything to buffer to have synchronized outputs even if we use multiple threads
			for (std::sregex_iterator i = std::sregex_iterator(text.begin(), text.end(), pattern_reg); i != std::sregex_iterator(); i++)
			{
				std::smatch match = *i;
				if (!f)
				{
					buffer << "\n<" << filePath << ">";
					f = !f;
				}
				buffer << "(" << match.position() << ")";
				buffer << "<" << stringFormat(text.substr(match.position() - downRange(match.position()), downRange(match.position()))) << ">";
				buffer << "<" << stringFormat(text.substr(match.position() + match.length(), upRange(match.position() + match.length(), text.size() - 1))) << ">\n";
			}
			if (f)
			{
				auto end = std::chrono::high_resolution_clock::now();
				auto d = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
				buffer << "Time of regex searching: " << d.count() << " ms\n";
				std::lock_guard lock(_mutex);
				std::cout << buffer.str();
			}
		}
		file.close();
	}
}
//Function to build regex
std::regex Researcher::buildRegFromPattern()
{
	std::string pat = "(";
	for (int i = 0; i < this->_pattern.length(); i++)
	{
		//we check if this character in pattern is a space
		//if it is we increment k as long as we get non whitespace character to replace all spaces or multiple newlines with \\s+ which means
		//space, tab or newline once or more times
		if (isspace(this->_pattern[i]))
		{
			int k = 0;
			do {
				k++;
			} while (isspace(this->_pattern[i + k]) && (i + k) < _pattern.length());
			pat += "\\s+";
			i = i + k - 1;
		}
		else
		{
			pat += handleRegSpecials(i);
		}
	}
	std::cout << (pat + ')') << '\n';

	return std::regex((pat + ')'), std::regex::icase);
}
//Function to handle reg specials
std::string Researcher::handleRegSpecials(size_t index)
{
	switch (this->_pattern[index])
	{
	case '\\':
		return "\\\\";
	case '.':
		return "\\.";
	case '?':
		return "\\?";
	case '(':
		return "\\(";
	case ')':
		return "\\)";
	case '[':
		return "\\[";
	case ']':
		return "\\]";
	case '{':
		return "\\{";
	case '}':
		return "\\}";
	case '*':
		return "\\*";
	case '+':
		return "\\+";
	case '|':
		return "\\|";
	case '$':
		return "\\$";
	case '-':
		return "\\-";
	case '^':
		return "\\^";
	default:
		return this->_pattern.substr(index, 1);
	}

}
//To match \n character in text we use \\n which regex interpretation is that \- next character is special character and then gets \n which is newline
//we replace that with string \\n to get newline in string as \n character because first backslash dissapear in string - same with \t
std::string Researcher::stringFormat(std::string& toFormat)
{
	const std::regex r("(\\n)");
	std::string result = regex_replace(toFormat, r, "\\n");
	const std::regex r2("\\t");
	return regex_replace(result, r2, "\\t");
}
//Checks if at the end of directory we have sequence ".txt"  
bool Researcher::isTextFile(const std::string& file)
{
	std::regex r("(\\.txt$)");
	return regex_search(file, r);
}
//Checks if 3 letters of suffix are possible if they are returns 3 otherwise returns difference between string size and position of last character of match
unsigned long Researcher::upRange(unsigned long position, unsigned long upLimit)
{
	return (upLimit - position) > 3 ? 3 : (upLimit - position);
}
//Checks if match starting position is bigger than 3 to get the full prefix if it is return 3 otherwise position of match
unsigned long Researcher::downRange(unsigned long position)
{
	return position > 3 ? 3 : position;
}

