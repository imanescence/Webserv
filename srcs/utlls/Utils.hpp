/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohanafi <yohanafi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 17:23:05 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/06/13 09:33:38 by iait-ouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../inc/Libraries.h"

class Utils {

	private :
		
		Utils();
		~Utils();

	public :

		bool static isValidExtension(const std::string& filename, const std::string& extensionToCompare);
		bool static isAnExecutable(const char *exec);
		bool static isValidFile(const char *file);
		bool static isValidPath(const char* dir);
		bool static isDirectory(const char* dir);
		std::string static isSlashTerminated(std::string path);
};

const std::string operator+(const std::string lhs, int rhs);
