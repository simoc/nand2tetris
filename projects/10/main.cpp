#include <iostream>
#include <cerrno>
#include <string.h>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "jacktokenizer.h"
#include "xmlcompiler.h"
#include "jackanalyzer.h"

/*
 * Returns true if given path is a directory.
 */
bool
isDir(const std::string &path)
{
	bool isDir = false;

	struct stat statBuf;
	if (stat(path.c_str(), &statBuf) == 0)
	{
		if (S_ISDIR(statBuf.st_mode))
		{
			isDir = true;
		}
	}
	return isDir;
}

/*
 * Returns true if string has given suffix.
 */
bool
endsWith(const std::string &str, const std::string &suffix)
{
	size_t index = str.find_last_of(suffix);
	return (index != std::string::npos &&
		index + suffix.length() == str.length());
}

int
main(int argc, char *argv[])
{
	int status = 1;
	std::string filename;
	std::string dirname;
	std::vector<std::string> jackFilenames;
	std::vector<std::string>::iterator it;

	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " filename.jack|directory" << std::endl;
		std::cerr << "Jack language compiler from www.nand2tetris.org Chapters 10-11" << std::endl;
		std::cerr << "Output written to filename.xml" << std::endl;
		goto CLEANUP;

	}

	filename = argv[1];

	if (isDir(filename))
	{
		DIR *dir = opendir(filename.c_str());
		if (dir)
		{
			struct dirent *entry = readdir(dir);
			while (entry != nullptr)
			{
				std::string filename2 = filename + "/" + entry->d_name;
				if ((endsWith(filename2, ".jack") || endsWith(filename2, ".JACK")) &&
					isDir(filename2) == false)
				{
					jackFilenames.push_back(filename2);
				}
				entry = readdir(dir);
			}
			closedir(dir);

			size_t slashIndex = filename.find_last_of('/');
			dirname = filename.substr(slashIndex + 1);
		}
	}
	else
	{
		jackFilenames.push_back(filename);
	}

	it = jackFilenames.begin();
	while (it != jackFilenames.end())
	{
		std::string jackFilename = *it;
		std::string xmlFilename = jackFilename;
		std::ofstream xmlFs;

		/*
		 * Replace .jack suffix with _gen.xml.
		 */
		size_t dotIndex = xmlFilename.find_last_of('.');
		if (dotIndex != std::string::npos)
			xmlFilename = xmlFilename.substr(0, dotIndex);
		xmlFilename += ".xml";

		JackTokenizer jt(jackFilename);
		if (!jt.open())
		{
			std::cerr << strerror(errno) << ": " << jackFilename << std::endl;
			goto CLEANUP;
		}

		XmlCompiler xmlCompiler(xmlFilename);

		JackAnalyzer ja;

		if (!ja.compile(jt, xmlCompiler))
			goto CLEANUP;

		it++;
	}

	status = 0;
CLEANUP:
	return status;
}
