/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iait-ouf <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 17:17:36 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/05/19 18:43:30 by iait-ouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location() {
        
	path = "";
	root = "";
	indexDefault = "";
	getMethod = false;
	postMethod = false;
	deleteMethod = false;
	cgiScript = false;
}

Location::Location(const std::string& _path) : path(_path) {

	root = "";
	indexDefault = "";
	getMethod = false;
	postMethod = false;
	deleteMethod = false;
	cgiScript = false;
}

Location::Location(std::string _path, std::string _root, std::string _indexDefault, bool _getMethod, bool _postMethod, bool _deleteMethod, bool _cgiScript) : path(_path), root(_root), indexDefault(_indexDefault), getMethod(_getMethod), postMethod(_postMethod), deleteMethod(_deleteMethod), cgiScript(_cgiScript) {
        
	if (cgiScript)
        	scripts.insert(std::pair<std::string, std::string>("Python", "./bin/cgi_script"));
}

std::string Location::getPath() {
	return path;
}

std::string Location::getRoot() {
	return root;
}

std::string Location::getIndex() {
	return indexDefault;
}

bool Location::isGetAllowed() {
	return getMethod;
}

bool Location::isPostAllowed() {
	return postMethod;
}

bool Location::isDeleteAllowed() {
	return deleteMethod;
}

bool Location::isCgiScript() {
	return cgiScript;
}

const std::map<std::string, std::string> &Location::getScripts() const {
	return scripts;
}

void Location::setGet() {
	getMethod = true;
}

void Location::setPost() {
	postMethod = true;
}

void Location::setDelete() {
	deleteMethod = true;
}

void Location::setCgi(bool _cgiScript) {
	cgiScript = _cgiScript;
}

void Location::addScript(std::string& lang, std::string& path) {
	scripts[lang] = path;
}

void Location::setPath(std::string& _path) {
	path = _path;
}

void Location::setRoot(std::string& _root) {
	root = _root;
}

void Location::setIndex(std::string& _index) {
	indexDefault = _index;
}

const std::string* Location::getScriptByLanguage(const std::string& lang) const {
	std::map<std::string, std::string>::const_iterator it = scripts.find(lang);
	if (it != scripts.end())
		return &(it->second);
	return NULL;
}

std::ostream& operator<<(std::ostream& out, const Location& loc) {

	out << "Path : " 			<< loc.path 					<< "\n";
	out << "Root : " 			<< loc.root 					<< "\n";
	out << "Index Default : " 		<< loc.indexDefault 				<< "\n";
	out << "GET Method Allowed : " 		<< (loc.getMethod ? "true" : "false") 		<< "\n";
	out << "POST Method Allowed : " 	<< (loc.postMethod ? "true" : "false") 		<< "\n";
	out << "DELETE Method Allowed : " 	<< (loc.deleteMethod ? "true" : "false") 	<< "\n";
	out << "CGI Script : " 			<< (loc.cgiScript ? "true" : "false") 		<< "\n";

	for (std::map<std::string, std::string>::const_iterator it = loc.scripts.begin(); it != loc.scripts.end(); ++it) {
		out << "Scripts [language][path of the script] -> ";
		out << "" << it->first << ": " << it->second << "\n";
	}

    return out;
}

Location::~Location() {}
