#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <map>
#include <iostream>
#include "utils.hpp"

enum e_reqState{
	HEADER,
	BODY,
	CHUNKED_BODY
};

//TODo WHat about empty header

//Data class which parses the Requests and exposens the elements to the User
//Detects Malformed requests
class Request {

	public:
	
	/*Constructors------------------------------------------------------------*/

	Request();
	~Request();
	Request(const Request & cpy);
	Request & operator=(const Request & rhs);

	private:

	/*Utils-------------------------------------------------------------------*/

	bool checkInvalidChar(const std::string & s, char *c, size_t size);
	bool	isDone();

	/*RequestLineParsing------------------------------------------------------*/

	bool parseFirstLine();
	void getNextReqLine(std::string & line);
	bool parseRequestLine(const std::string & line);
	bool isValidRequestLine();
	bool isValidHttpVer();

	/*RequestHeaderParsing----------------------------------------------------*/

	void getNextHeaderLine(std::string & line);
	bool parseHeader();

	/*RequestBodyParsing------------------------------------------------------*/

	bool parseBody();

	/*Internal MemberVariabels------------------------------------------------*/

	std::string	m_buffer;
	size_t		m_offset;
	e_reqState	m_state;

	/*DataMemberVariabels-----------------------------------------------------*/

	uint32_t	m_errCode;
	std::string m_methode;
	std::string m_target;
	std::string m_httpVer;
	std::map<std::string, std::string> m_header; 
	std::string	m_body;

	public:

	/*Public Functions--------------------------------------------------------*/

	bool	appendRead(const char *buf);

	/*Debug only--------------------------------------------------------------*/	

	void	printRequest();

	/*Getter------------------------------------------------------------------*/

	const std::string & get_methode() const;
	const std::string & get_target() const;
	const std::string & getHttpVer() const;
	std::string getHeaderEntry(const std::string & fieldName) const;
	const std::string & getBody() const;
	uint32_t get_errCode() const;

};

#endif