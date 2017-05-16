#include <iostream>
#include <fstream>

#include <sys/param.h>
#include <unistd.h>

#include "settingsParser.h"
#include "nodeSpawner.h"
#include "bridgeTapBuilder.h"
#include "ns3Writer.h"
#include "linkTypeMap.h"

#include "topologyGenerator.h"

using namespace std;

void generateTopology(ns3lxc::Topology *topology){
    assignBridgesTaps(topology);
    findLinkTypesUsed(topology);
    if(Settings::genContainers()){
        tearDownAllBridgesTaps(topology);
        buildAllBridgesTaps(topology);
        NodeSpawner::createNodes(topology);
        NodeSpawner::startNodes(topology);
        NodeSpawner::installApplications(topology);
    }
    if(Settings::genNS3()){
        string scriptLoc = Settings::script_dest + "/" + topology->name + ".cc";
        Ns3Writer::writeScript(topology, scriptLoc);
        cout << "Waiting to continue (enter): ";
        cout.flush();
        string test;
        cin >> test;
        cout << "Continuing..." << endl;
        std::ifstream src(scriptLoc, std::ios::binary);
        std::ofstream dst(Settings::ns3_path + "/scratch/" + topology->name + ".cc",   std::ios::binary);
        dst << src.rdbuf();
        src.close();
        dst.close();
    }
    if(Settings::runNS3()){
        chdir(Settings::ns3_path.c_str());
        NodeSpawner::runApplications(topology);
        if(Settings::gdbNS3()){
            system(("./waf --run scratch/" + topology->name + " --command-template=\"gdb --args %s\"").c_str());
        } else {
            system(("./waf --run scratch/" + topology->name).c_str());
        }
        cout << endl;
    }
    if(Settings::teardownContainers()){
        cout << "Tearing Down Topology" << endl;
        NodeSpawner::teardownNodes(topology);
        tearDownAllBridgesTaps(topology);
    }
}

void findLinkTypesUsed(ns3lxc::Topology *topology){
    for(auto linkPtr : topology->links){
        linkTypeMap.find(linkPtr->getType())->second->setUsed();
    }
    for(auto subTopPtr : topology->subTopologies){
        findLinkTypesUsed(subTopPtr.get());
    }
}