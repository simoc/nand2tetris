#include <iostream>
#include <cerrno>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "parser.h"
#include "codewriter.h"

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
	std::vector<std::string> vmFilenames;
	std::vector<std::string>::iterator it;
	CodeWriter c;

	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " filename.vm|directory" << std::endl;
		std::cerr << "Implements VM in Hack language from www.nand2tetris.org Chapter 7" << std::endl;
		std::cerr << "Output written to filename.asm" << std::endl;
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
				if ((endsWith(filename2, ".vm") || endsWith(filename2, ".VM")) &&
					isDir(filename2) == false)
				{
					vmFilenames.push_back(filename2);
				}
				entry = readdir(dir);
			}
			closedir(dir);
		}
	}
	else
	{
		vmFilenames.push_back(filename);
	}

	it = vmFilenames.begin();
	while (it != vmFilenames.end())
	{
		std::string vmFilename = *it;
		std::string asmFilename = vmFilename;
		int index;
		size_t index2;

		/*
		 * Replace .vm suffix with .asm.
		 */
		size_t dotIndex = asmFilename.find_last_of('.');
		if (dotIndex != std::string::npos)
			asmFilename = asmFilename.substr(0, dotIndex);
		asmFilename += ".asm";

		c.setVmFilename(vmFilename);
		c.setAsmFilename(asmFilename);
		c.open();
		Parser p(vmFilename);
		if (!p.open())
		{
			std::cerr << strerror(errno) << ": " << vmFilename << std::endl;
			goto CLEANUP;
		}

		while (p.hasMoreCommands())
		{
			p.advance();
			switch (p.commandType())
			{
				case Parser::C_UNKNOWN:
					std::cerr << "Unknown command" << std::endl;
					break;
				case Parser::C_ARITHMETIC:
					c.writeArithmetic(p.arg1());
					break;
				case Parser::C_PUSH:
					
					index = std::stol(p.arg2(), &index2);
					c.writePushPop(p.commandType(), p.arg1(), index);
					break;
				case Parser::C_POP:
					index = std::stol(p.arg2(), &index2);
					c.writePushPop(p.commandType(), p.arg1(), index);
					break;
				default:
					break;
			}
		}
		it++;
	}

	status = 0;
CLEANUP:
	return status;
}
