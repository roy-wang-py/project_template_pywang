#ifndef __DIRECTORYITERATOR_UNIX_HXX__
#define __DIRECTORYITERATOR_UNIX_HXX__

#include <dirent.h>
#include <string>
#include "default_config.hxx"
NAMESPACE_PREFIX


class  DirectoryIteratorImpl
{
public:
	DirectoryIteratorImpl(const std::string& path);
	~DirectoryIteratorImpl(){
		if (_pDir) closedir(_pDir);
	}
	
	void duplicate(){
		++_rc;
	}
	void release(){
		if (--_rc == 0)
		delete this;
	}
	
	const std::string& get() const{
		return _current;
	}
	const std::string& next();
	
private:
	DIR*        _pDir;
	std::string _current;
	int _rc;
};
NAMESPACE_SUBFIX


#endif // __DIRECTORYITERATOR_UNIX_HXX__
