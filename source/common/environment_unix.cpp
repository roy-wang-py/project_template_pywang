
#include "environment_unix.hxx"
#include "Exception.hxx"

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
NAMESPACE_PREFIX


EnvironmentImpl::StringMap EnvironmentImpl::_map;
std::mutex EnvironmentImpl::_mutex;


std::string EnvironmentImpl::getImpl(const std::string& name)
{
	std::unique_lock<std::mutex> lock(_mutex);
	const char* val = getenv(name.c_str());
	if (val)
		return std::string(val);
	else
		throw NotFoundException(name);
}


bool EnvironmentImpl::hasImpl(const std::string& name)
{
	std::unique_lock<std::mutex> lock(_mutex);
	return getenv(name.c_str()) != 0;
}


void EnvironmentImpl::setImpl(const std::string& name, const std::string& value)
{
	
	std::string var = name;
	var.append("=");
	var.append(value);
	std::unique_lock<std::mutex> lock(_mutex);
	_map[name] = var;
	if (putenv((char*) _map[name].c_str()))
	{
		std::string msg = "cannot set environment variable: ";
		msg.append(name);
		throw SystemException(msg);
	}
}


unsigned EnvironmentImpl::processorCountImpl()
{
	int count = sysconf(_SC_NPROCESSORS_ONLN);
	if (count <= 0) count = 1;
	return static_cast<int>(count);
}

void EnvironmentImpl::nodeIdImpl(NodeId& id)
{
	std::memset(&id, 0, sizeof(id));

	// ideally, the following code should be rewritten
	// to use netlink

	// first try to obtain the MAC address of eth0 using /sys/class/net
	int fd = open("/sys/class/net/eth0/address", O_RDONLY);
	if (fd >= 0)
	{
		char buffer[18];
		int n = read(fd, buffer, 17);
		close(fd);
		if (n == 17)
		{
			buffer[n] = 0;
			if (std::sscanf(buffer, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &id[0], &id[1], &id[2], &id[3], &id[4], &id[5]) == 6)
				return;
		}
	}	

	// if that did not work, search active interfaces
	int sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock == -1) return;

	// the following code is loosely based
	// on W. Richard Stevens, UNIX Network Programming, pp 434ff.
	int lastlen = 0;
	int len = 100*sizeof(struct ifreq);
	struct ifconf ifc;
	char* buf = 0;
	for (;;)
	{
		buf = new char[len];
		ifc.ifc_len = len;
		ifc.ifc_buf = buf;
		if (::ioctl(sock, SIOCGIFCONF, &ifc) < 0)
		{
			if (errno != EINVAL || lastlen != 0)
			{
				close(sock);
				delete [] buf;
				return;
			}
		}
		else
		{
			if (ifc.ifc_len == lastlen)
				break;
			lastlen = ifc.ifc_len;
		}
		len += 10*sizeof(struct ifreq);
		delete [] buf;
	}
	for (const char* ptr = buf; ptr < buf + ifc.ifc_len;)
	{
		const struct ifreq* ifr = reinterpret_cast<const struct ifreq*>(ptr);		
		int rc = ioctl(sock, SIOCGIFHWADDR, ifr);
		if (rc != -1)
		{
			const struct sockaddr* sa = reinterpret_cast<const struct sockaddr*>(&ifr->ifr_hwaddr);
			if (sa->sa_family == ARPHRD_ETHER)
			{
				std::memcpy(&id, sa->sa_data, sizeof(id));
				break;
			}
		}
		ptr += sizeof(struct ifreq);
	}
	close(sock);
	delete [] buf;
}

NAMESPACE_SUBFIX
