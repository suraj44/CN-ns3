#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/point-to-point-dumbbell.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/on-off-helper.h"

using namespace ns3;


// Network topology - PointToPointDumbell
//
//  n0                                n5
//     \ 5 Mb/s, 2ms                  /
//      \          5Mb/s, 2ms        /
//n2----n1 -------------------------n4----n6
//      /                            \
//     /                              \
//  n3                                 n7

// This script is for simulating a scenario where
// one of the left leaves of the dumbell tries sending
// packets to one of the right nodes.

// Each pair of links is a different network 

// n1 and n4 are acting as routers in this topology.



NS_LOG_COMPONENT_DEFINE("PointToPointExample");

int main(int argc, char const *argv[])
{
    CommandLine cmd;

    //cmd.Parse(argc, argv);
    PointToPointHelper lHelper, rHelper, bottleNeck;

    PointToPointDumbbellHelper network1(3, lHelper, 3, rHelper, bottleNeck);

    InternetStackHelper stack;
    network1.InstallStack(stack);

    Ipv4AddressHelper addressLeft, addressRight, addressRouter;

    // Assign different ranges of addresses to each pair of nodes
    // the important thing to note is that each pair of nodes forms a 
    // separate network. Hence, one has to be careful while assigning 
    // network IDs and subnet masks as there might be collisions.

    // Another option would have been to have network IDs like
    // 10.1.1.0, 10.1.2.0, 10.1.3.0 withsubnet masks as 
    // 255.255.255.253
    addressLeft.SetBase("192.168.1.0", "255.255.255.0");
    addressRight.SetBase("10.1.2.0", "255.255.255.0");
    addressRouter.SetBase("11.168.3.0", "255.255.255.0");
    network1.AssignIpv4Addresses(addressLeft, addressRight, addressRouter);

    // IMPORTANT: Very easy to miss this statement.
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // The first argument specifies the destination.
    // We will install this application on the source in the next statement.
    OnOffHelper onoff("ns3::UdpSocketFactory",  Address(InetSocketAddress(network1.GetRightIpv4Address(1),  10)));

    onoff.SetConstantRate(DataRate("448kb/s"));


    ApplicationContainer apps = onoff.Install(network1.GetLeft(1));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(50.0));


    // We install a sink application at the destination and tell it to receive 
    // packets from any node.
    PacketSinkHelper sink("ns3::UdpSocketFactory", Address(InetSocketAddress(Ipv4Address::GetAny(),  10)));
    apps=sink.Install(network1.GetRight(1));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(50.0));


    NS_LOG_INFO ("Run Simulation.");
    //Simulator::Stop (Seconds (10));
    Simulator::Run ();
    NS_LOG_INFO ("Done.");
    Simulator::Destroy();

    Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (apps.Get(0));
    std::cout << "Total Bytes Received: " << sink1->GetTotalRx() << std::endl;
    
    return 0;
}