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
#include "vmcompiler.h"
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

void
usage(const char *programName)
{
	std::cerr << "Usage: " << programName << " [-xml | -vm] filename.jack|directory" << std::endl;
	std::cerr << "Jack language compiler from www.nand2tetris.org Chapters 10-11" << std::endl;
	std::cerr << "Output written to filename.xml or filename.vm" << std::endl;
}

int
main(int argc, char *argv[])
{
	int status = 1;
	std::string filename;
	std::string compileOption = "xml";
	std::string dirname;
	std::vector<std::string> jackFilenames;
	std::vector<std::string>::iterator it;

	if (argc < 2)
	{
		usage(argv[0]);
		goto CLEANUP;

	}

	filename = argv[1];
	if (filename == "-xml")
	{
		compileOption = "xml";
		if (argc < 3)
		{
			usage(argv[0]);
			goto CLEANUP;
		}
		filename = argv[2];
	}
	else if (filename == "-vm")
	{
		compileOption = "vm";
		if (argc < 3)
		{
			usage(argv[0]);
			goto CLEANUP;
		}
		filename = argv[2];
	}

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

		JackTokenizer jt(jackFilename);
		if (!jt.open())
		{
			std::cerr << strerror(errno) << ": " << jackFilename << std::endl;
			goto CLEANUP;
		}

		JackAnalyzer ja;

		if (compileOption == "xml")
		{
			XmlCompiler xmlCompiler(jackFilename);

			if (!ja.compile(jt, xmlCompiler))
				goto CLEANUP;
		}
		else
		{
			VmCompiler vmCompiler(jackFilename);

			if (!ja.compile(jt, vmCompiler))
				goto CLEANUP;
		}

		it++;
	}

	status = 0;
CLEANUP:
	return status;
}
