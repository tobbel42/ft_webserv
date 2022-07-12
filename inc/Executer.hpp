#pragma once




class Executer
{
public: // methods
	Executer();
	Executer(const Executer& other);

	Executer& operator=(const Executer& other);




private: // attributes
	Server*			p_server;
	char**			p_env;
	int				m_status_code;

private: // enum
	enum e_FileType
	{
		HTML,
		PHP,
		PYTHON,
		FOLDER
	};
};