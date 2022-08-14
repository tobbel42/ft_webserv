#pragma once

#include <string>

#include <dirent.h>

#include "typedefs.hpp"

class DirectoryListing
{
public: // methods
	DirectoryListing(const std::string& dirpath, const std::string& url);
	DirectoryListing(const DirectoryListing& other);

	~DirectoryListing();

	DirectoryListing& operator=(const DirectoryListing& other);

	int get_status_code() const;

	std::string run();


private: // methods
	DirectoryListing();

	std::string	get_mod_date(const std::string& entry_name) const;
	std::string	generate_table_row(const std::string& entry_name) const;


private: // attributes
	std::string		m_dirpath;
	std::string		m_url;
	int				m_status_code;


private: // subclass
	/*
	A wrapper for DIR* objects similar to std::unique_ptr in order to avoid
	manually closing the directory
	*/
	class DirWrap
	{
	public:
		DirWrap();
		DirWrap(DIR* dir);

		~DirWrap();

		operator DIR* () const;

		void set_dir(DIR* dir);

		bool open(const std::string& filename);
		void close();

	private:
		DirWrap(const DirWrap&);
		DirWrap& operator=(const DirWrap&);

		DIR* p_dir;
	};

};