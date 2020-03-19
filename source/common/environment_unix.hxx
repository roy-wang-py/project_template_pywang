
#ifndef __Environment_UNIX_HXX__
#define __Environment_UNIX_HXX__

#include <mutex>
#include <map>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include "default_config.hxx"

NAMESPACE_PREFIX
	typedef unsigned char UInt8;
class  EnvironmentImpl
{
public:
	typedef UInt8 NodeId[6]; /// Ethernet address.

	static std::string getImpl(const std::string& name);	
	static bool hasImpl(const std::string& name);	
	static void setImpl(const std::string& name, const std::string& value);
	static std::string osNameImpl(){
		struct utsname uts;
		uname(&uts);
		return uts.sysname;
	}
	static std::string osDisplayNameImpl(){
		return osNameImpl();
	}
	static std::string osVersionImpl(){
		struct utsname uts;
		uname(&uts);
		return uts.release;
	}
	static std::string osArchitectureImpl(){
		struct utsname uts;
		uname(&uts);
		return uts.machine;
	}
	static std::string nodeNameImpl(){
			struct utsname uts;
		uname(&uts);
		return uts.nodename;
	}
	static void nodeIdImpl(NodeId& id);
	static unsigned processorCountImpl();

private:
	typedef std::map<std::string, std::string> StringMap;
	
	static StringMap _map;
	static std::mutex _mutex;
};


NAMESPACE_SUBFIX


#endif // __Environment_UNIX_HXX__
