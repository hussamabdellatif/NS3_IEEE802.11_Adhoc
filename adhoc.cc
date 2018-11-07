#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("lab2_task1");

int main ()
{
	uint32_t nwifi = 6;
	//bool tracing = true;
	
	 if (true)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    bool enableCtsRts = true;
    UintegerValue ctsThr = (enableCtsRts ? UintegerValue (100) : UintegerValue (2200));
    Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", ctsThr);
    
    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(nwifi);
    
    WifiHelper wifi;
    wifi.SetStandard(WIFI_PHY_STANDARD_80211g);
    wifi.SetRemoteStationManager ("ns3::AarfWifiManager");
    
    NqosWifiMacHelper mac = NqosWifiMacHelper::Default();
    mac.SetType("ns3::AdhocWifiMac");
    
    
	YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
	YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
	phy.SetChannel (channel.Create ());
    
    NetDeviceContainer staDevices;
	staDevices = wifi.Install (phy, mac, wifiStaNodes);
	
	MobilityHelper mobility;

	mobility.SetPositionAllocator (	 "ns3::GridPositionAllocator",
									 "MinX", DoubleValue (0.0),
									 "MinY", DoubleValue (0.0),
									 "DeltaX", DoubleValue (5.0),
									 "DeltaY", DoubleValue (10.0),
									 "GridWidth", UintegerValue (6),
									 "LayoutType", StringValue ("RowFirst"));
									 
	mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-100, 100, -100, 100)));
	mobility.Install (wifiStaNodes);
	
	InternetStackHelper stack;
	stack.Install (wifiStaNodes);
	
	Ipv4AddressHelper address;
	address.SetBase ("192.168.1.0", "255.255.255.0");
	Ipv4InterfaceContainer wifiInterfaces;
	wifiInterfaces = address.Assign (staDevices);
	
	
	  UdpEchoServerHelper echoServer (20);

	  ApplicationContainer serverApps = echoServer.Install (wifiStaNodes.Get (0));
	  serverApps.Start (Seconds (1.0));
	  serverApps.Stop (Seconds (10.0));

	  UdpEchoClientHelper echoClient ( wifiInterfaces.GetAddress (0), 20);
	  echoClient.SetAttribute ("MaxPackets", UintegerValue (2));
	  echoClient.SetAttribute ("Interval", TimeValue (Seconds (2.0)));
	  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

	  ApplicationContainer clientApps = echoClient.Install (wifiStaNodes.Get (5));
	  clientApps.Start (Seconds (2.0));
	  clientApps.Stop (Seconds (10.0));
	  
	   UdpEchoClientHelper echoClient2 ( wifiInterfaces.GetAddress (0), 20);
	  echoClient2.SetAttribute ("MaxPackets", UintegerValue (2));
	  echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
	  echoClient2.SetAttribute ("PacketSize", UintegerValue (1024));
	  
	  ApplicationContainer clientApps2 = echoClient2.Install (wifiStaNodes.Get (4));
	  clientApps2.Start (Seconds (3.0));
	  clientApps2.Stop (Seconds (10.0));

	  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	  Simulator::Stop (Seconds (10.0));
	 
	 phy.EnablePcap("lab2_task1",staDevices.Get(2), true);
	 
	 Simulator::Run ();
	Simulator::Destroy ();
	return 0;
	
	
}
