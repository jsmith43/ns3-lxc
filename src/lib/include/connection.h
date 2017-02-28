#ifndef __CONNECTION_H_INCLUDED__
#define __CONNECTION_H_INCLUDED__

// forward declared dependencies
class Iface;

// include dependencies
#include <string>
#include <vector>

namespace ns3lxc {

// declarations
class Connection : public IfaceAccepter {
private:
	std::string name;
	std::string type;
	
	int numIfaces;
	std::vector<Iface *> ifaces;
	
public:
	Connection(std::string name, std::string type, int numIfaces);
	
	std::string getType() { return type; }
	std::string getName() { return name; }
	const vector<Iface *>& getIfaces() const { return ifaces; } 
	
	int connectIface(std::string ifaceName, Iface *iface); // OVERRIDES IfaceAccepter
};

#endif