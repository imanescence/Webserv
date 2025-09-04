/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iait-ouf <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 17:36:13 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/06/13 09:37:37 by iait-ouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

Utils::Utils(){}
Utils::~Utils(){}


bool Utils::isValidPath(const char* dir) {
	
	struct stat sb;

	if (stat(dir, &sb) == 0)
		return true;
	
	return false;
}


bool Utils::isValidFile(const char *file) {
	
	struct stat sb;
	
	if (stat(file, &sb) == 0 && !(sb.st_mode & S_IFDIR))
		return true;
	
	return false;
}


bool Utils::isAnExecutable(const char *exec) {

	struct stat sb;

	if (stat(exec, &sb) == 0 && !(sb.st_mode & S_IFDIR)) {
		if (sb.st_mode & S_IXUSR || sb.st_mode & S_IXGRP || sb.st_mode & S_IXOTH)
			return true;
	}

	return false;
}


bool Utils::isValidExtension(const std::string& filename, const std::string& extensionToCompare) {
	
	if (extensionToCompare == ".html") {
		std::string extensionFromFile = filename.substr(filename.length() - 5);
		return (extensionFromFile == ".html");
        }

        else if (extensionToCompare == ".py") {
                std::string extensionFromFile = filename.substr(filename.length() - 3);
                return (extensionFromFile == ".py");
        }

        else if (extensionToCompare == ".php") {
                std::string extensionFromFile = filename.substr(filename.length() - 4);
                return (extensionFromFile == ".php");
        }

        else if (extensionToCompare == ".png") {
                std::string extensionFromFile = filename.substr(filename.length() - 4);
                return (extensionFromFile == ".png");
        }

        else if (extensionToCompare == ".jpeg") {
                std::string extensionFromFile = filename.substr(filename.length() - 5);
                return (extensionFromFile == ".jpeg");
        }

        return false;
}

std::string Utils::isSlashTerminated(std::string path) {

	if (path.empty() || path[path.size() - 1] != '/') {
		path += '/';
	}	
		
	return path;
}

const std::string operator+(const std::string lhs, int rhs){
	std::stringstream ss;
	ss << lhs << rhs;
	std::string result(ss.str());
	return result;
}

bool Utils::isDirectory(const char* dir) {
	
	struct stat pathStat;
	
	if (stat(dir, &pathStat) != 0) {
	
		return false;
	}
	
	return S_ISDIR(pathStat.st_mode);
}

