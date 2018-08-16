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
    addressLeft.SetBase("192.168.1.0", "255.255.255.0");
    addressRight.SetBase("10.1.2.0", "255.255.255.0");
    addressRouter.SetBase("11.168.3.0", "255.255.255.0");
    network1.AssignIpv4Addresses(addressLeft, addressRight, addressRouter);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    OnOffHelper onoff("ns3::UdpSocketFactory",  Address(InetSocketAddress(network1.GetRightIpv4Address(1),  10)));

    onoff.SetConstantRate(DataRate("448kb/s"));


    ApplicationContainer apps = onoff.Install(network1.GetLeft(1));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(50.0));


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