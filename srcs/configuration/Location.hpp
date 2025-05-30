/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iait-ouf <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 16:56:15 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/05/19 18:41:34 by iait-ouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "../../inc/Libraries.h"

class Location {

        private :

        	std::string path;               		// ex : /images/
                

		std::string root;               		// ex : web_repo/images/
		std::string indexDefault;       		// file by default ex: index.html
		std::map<std::string, std::string> scripts;	// [language][executable]
		bool getMethod;					// is GET method allowed ?
		bool postMethod;				// is POST method allowed ?
		bool deleteMethod;				// is DELETE method allowed ?
		bool cgiScript;					// is CGI ?

		void setPath(std::string& _path);
		void setRoot(std::string& _root);
		void setIndex(std::string& _index);
		void setGet();
		void setPost();
		void setDelete();
		void setCgi(bool _cgiScript);
		void addScript(std::string& lang, std::string& path);
		friend class Parsing;

        public :
                
		// Constructors and destructor
		Location();

		Location(const std::string& _path);

		Location(std::string _root, std::string _path, std::string indexDefault, bool getMethod, bool postMethod, bool deleteMethod, bool cgiScript);
		
		~Location();
                

		// Getters
		std::string getPath();
		
		std::string getRoot();
		
		std::string getIndex();
		
		bool isGetAllowed();
		
		bool isPostAllowed();
		
		bool isDeleteAllowed();
		
		bool isCgiScript();
		
		const std::map<std::string, std::string>& getScripts() const;
		
		const std::string *getScriptByLanguage(const std::string& lang) const;

		// Overload operator to print all the elements of the obj
  		friend std::ostream& operator<<(std::ostream& out, const Location& loc);
};
