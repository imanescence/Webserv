/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohanafi <yohanafi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 16:56:15 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/06/17 16:31:27 by iait-ouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "../../inc/Libraries.h"

class Location {

        private :

        std::string path;        
		std::string root;         
		bool getMethod;					
		bool postMethod;				
		bool deleteMethod;
		bool _autoIndexOn;
		std::string _clientBodyTmpPath;
		bool _redirection;
		std::string _redirPath;

		void setPath(std::string& _path);
		void setRoot(std::string& _root);
		void setGet();
		void setPost();
		void setDelete();
		void setClientDir(const std::string& clientBodyTmpPath);
		void setAutoIndex();
		void setRedirectPath(std::string &redirPath);

		friend class Parsing;

        public :
                
		// Constructors and destructor
		Location();

		Location(const std::string& _path);

		Location(std::string _root, std::string _path, bool getMethod, bool postMethod, bool deleteMethod, bool autoIndex, std::string clientBodyTmpPath, bool redirection, std::string redirPath);
		
		~Location();
                

		// Getters
		std::string getPath() const;
		
		std::string getRoot() const;
		
		bool isGetAllowed() const;
		
		bool isPostAllowed() const;
		
		bool isDeleteAllowed() const;
		
		bool isAutoIndexOn() const;

		bool isRedirection() const;

		std::string getRedirPath() const;
		
		std::string getClientBodyTmpPath();

		const std::map<std::string, std::string>& getScripts() const;
		
		const std::string *getScriptByLanguage(const std::string& lang) const;

		// Overload operator to print all the elements of the obj
  		friend std::ostream& operator<<(std::ostream& out, const Location& loc);
};
