#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
using namespace ns3;

// Network topology
//
//  n0
//     \ 5 Mb/s, 2ms
//      \          5Mb/s, 2ms
//       n1 -------------------------n2

// We're sending packets from n0 to n2 through n1 which is acting as a router


//Instructions to run:
// Place this file in ns3-dev/scratch
// export NS_LOG="PartA"=info
// in the root folder,
// ./waf --run scratch/qa



NS_LOG_COMPONENT_DEFINE ("PartA");
int main(int argc, char  *argv[])
{
    CommandLine cmd;
    cmd.Parse(argc, argv);

    Time::SetResolution (Time::NS);
    NodeContainer nodes;
    nodes.Create(3);

    NS_LOG_INFO("Created three nodes\n");

    PointToPointHelper pointToPoint01, pointToPoint12 ;
    pointToPoint01.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint01.SetChannelAttribute("Delay",  StringValue("2ms"));

    pointToPoint12.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint12.SetChannelAttribute("Delay",  StringValue("2ms"));

    NS_LOG_INFO("Created point to point links\n");

    NetDeviceContainer devices01, devices12;
    devices01 = pointToPoint01.Install(nodes.Get(0), nodes.Get(1));
    devices12 = pointToPoint12.Install(nodes.Get(1), nodes.Get(2));

    InternetStackHelper stack01;
    stack01.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("192.168.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces01 = address.Assign(devices01);
    address.SetBase("192.168.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces12 = address.Assign(devices12);
    NS_LOG_INFO("Assigned IP addresses to the nodes \n");

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    NS_LOG_INFO("Creating Applications.");
    uint16_t port = 9;
    NS_LOG_INFO("here");
    OnOffHelper onoff("ns3::UdpSocketFactory",  Address(InetSocketAddress(interfaces12.GetAddress(1),  port)));

    onoff.SetConstantRate(DataRate("448kb/s"));


    ApplicationContainer apps = onoff.Install(nodes.Get(0));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(50.0));


    PacketSinkHelper sink("ns3::UdpSocketFactory", Address(InetSocketAddress(Ipv4Address::GetAny(),  port)));
    apps=sink.Install(nodes.Get(2));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(50.0));

    // Flow Monitor
    // FlowMonitorHelper flowmonHelper;
    // if (enableFlowMonitor)
    // {
    //     flowmonHelper.InstallAll ();
    // }

    std::cout << "hello\n";
    NS_LOG_INFO ("Run Simulation.");
    //Simulator::Stop (Seconds (10));
    Simulator::Run ();
    NS_LOG_INFO ("Done.");
    Simulator::Destroy();


    Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (apps.Get(0));
    std::cout << "Total Bytes Received: " << sink1->GetTotalRx() << std::endl;

    return 0;
}