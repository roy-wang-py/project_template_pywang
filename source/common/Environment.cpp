
#include "Environment.hxx"
#include <cstdlib>
#include <cstdio>

#include "environment_unix.hxx"



NAMESPACE_PREFIX


std::string Environment::get(const std::string& name)
{
	return EnvironmentImpl::getImpl(name);
}


std::string Environment::get(const std::string& name, const std::string& defaultValue)
{
	if (has(name))
		return get(name);
	else
		return defaultValue;
}


bool Environment::has(const std::string& name)
{
	return EnvironmentImpl::hasImpl(name);
}


void Environment::set(const std::string& name, const std::string& value)
{
	EnvironmentImpl::setImpl(name, value);
}


std::string Environment::osName()
{
	return EnvironmentImpl::osNameImpl();
}


std::string Environment::osDisplayName()
{
	return EnvironmentImpl::osDisplayNameImpl();
}


std::string Environment::osVersion()
{
	return EnvironmentImpl::osVersionImpl();
}


std::string Environment::osArchitecture()
{
	return EnvironmentImpl::osArchitectureImpl();
}


std::string Environment::nodeName()
{
	return EnvironmentImpl::nodeNameImpl();
}


std::string Environment::nodeId()
{
	NodeId id;
	nodeId(id);
	char result[18];
	std::sprintf(result, "%02x:%02x:%02x:%02x:%02x:%02x",
		id[0],
		id[1],
		id[2],
		id[3],
		id[4],
		id[5]);
	return std::string(result);
}


void Environment::nodeId(NodeId& id)
{
	return EnvironmentImpl::nodeIdImpl(id);
}


unsigned Environment::processorCount()
{
	return EnvironmentImpl::processorCountImpl();
}


unsigned int Environment::libraryVersion()
{
	return 1;
}


int Environment::os()
{
	return 0x0005;
}


#define POCO_ARCH 0x0a

int Environment::arch()
{
	return POCO_ARCH;
}


bool Environment::isUnix()
{
	return true;

}


bool Environment::isWindows()
{
	return false;

}


NAMESPACE_SUBFIX
