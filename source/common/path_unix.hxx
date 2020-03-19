

#ifndef __PATH_UNIX_HXX__
#define __PATH_UNIX_HXX__

#include <string>
#include <vector>
#include "default_config.hxx"

NAMESPACE_PREFIX

class PathImpl
{
public:
	static std::string currentImpl();
	static std::string homeImpl();
	static std::string configHomeImpl();
	static std::string dataHomeImpl();
	static std::string tempHomeImpl();
	static std::string cacheHomeImpl();
	static std::string tempImpl();
	static std::string configImpl(){
		return "/etc/";
	}
	static std::string nullImpl(){
		return "/dev/null";
	}
	static std::string expandImpl(const std::string& path);
	static void listRootsImpl(std::vector<std::string>& roots){
		roots.clear();
		roots.push_back("/");
	}
};


NAMESPACE_SUBFIX


#endif // __PATH_UNIX_HXX__
