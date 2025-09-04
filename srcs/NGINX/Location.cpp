/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohanafi <yohanafi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 17:17:36 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/06/17 16:33:20 by iait-ouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location() {
       
	_clientBodyTmpPath = "";	
	path = "";
	root = "";
	getMethod = false;
	postMethod = false;
	deleteMethod = false;
	_autoIndexOn = false;
	_redirection = false;
	_redirPath = "";
}

Location::Location(const std::string& _path) : path(_path) {

	_clientBodyTmpPath = "";	
	root = "";
	getMethod = false;
	postMethod = false;
	deleteMethod = false;
	_autoIndexOn = false;
	_redirection = false;
	_redirPath = "";
}

Location::Location(std::string _path, std::string _root, bool _getMethod, bool _postMethod, bool _deleteMethod, bool autoIndex, std::string clientBodyTmpPath, bool redirection, std::string redirPath): path(_path), root(_root), getMethod(_getMethod), postMethod(_postMethod), deleteMethod(_deleteMethod), _autoIndexOn(autoIndex), _clientBodyTmpPath(clientBodyTmpPath), _redirection(redirection), _redirPath(redirPath) {}

std::string Location::getRedirPath() const {
	return _redirPath;
}

std::string Location::getPath() const {
	return path;
}

std::string Location::getRoot() const {
	return root;
}

std::string Location::getClientBodyTmpPath() {
	return _clientBodyTmpPath;
}

bool Location::isRedirection() const {
	return _redirection;
}

bool Location::isGetAllowed() const{
	return getMethod;
}

bool Location::isPostAllowed() const {
	return postMethod;
}

bool Location::isAutoIndexOn() const {
	return _autoIndexOn;
}

bool Location::isDeleteAllowed() const{
	return deleteMethod;
}

void Location::setClientDir(const std::string& clientBodyTmpPath) {
        _clientBodyTmpPath = clientBodyTmpPath;
}

void Location::setRedirectPath(std::string &redirPath) {
	_redirection = true;
	_redirPath = redirPath;
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

void Location::setAutoIndex() {
	_autoIndexOn = true;
}
void Location::setPath(std::string& _path) {
	path = _path;
}

void Location::setRoot(std::string& _root) {
	root = _root;
}

std::ostream& operator<<(std::ostream& out, const Location& loc) {

	out << "[LOCATION]" << std::endl;
	out << "Path : " 			<< loc.path 					<< "\n";
	out << "Root : " 			<< loc.root 					<< "\n";
	out << "GET Method Allowed : " 		<< (loc.getMethod ? "true" : "false") 		<< "\n";
	out << "POST Method Allowed : " 	<< (loc.postMethod ? "true" : "false") 		<< "\n";
	out << "DELETE Method Allowed : " 	<< (loc.deleteMethod ? "true" : "false") 	<< "\n";
	out << "is Directory Listing on : " 	<< (loc._autoIndexOn ? "true" : "false") 	<< "\n";
	out << "CLIENT UPLOAD FILES -> " 	<< loc._clientBodyTmpPath << "\n";
	out << "is a redirection ? : " 		<< (loc._redirection ? "true" : "false") 	<< "\n";
	out << "Loc of redirection -> " 	<< loc._redirPath << "\n";

    return out;
}

Location::~Location() {}
