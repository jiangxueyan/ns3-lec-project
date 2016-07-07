/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/point-to-point-module.h"
#include "ns3/lte-helper.h"
#include "ns3/epc-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/config-store.h"

// Default Network Topology
//
//   Wifi2 10.1.2.0
//                 AP
//  *    *    *    *
//  |    |    |    |          7.0.0.0
// n13  ...   n8   n0 ----------pgw--------- n1   n2   n3   n4   n5   n6   n7
//                                            |    |    |    |    |    |    |
//                                            *    *    *    *    *    *    * //                                           AP
//                                             wifi1 10.1.1.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ForthScriptExample");

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nWifi1 = 6;
  uint32_t nWifi2 = 6;
  uint16_t numberOfNodes = 2;

  CommandLine cmd;
  cmd.AddValue ("nWifi1", "Number of wifi1 STA devices", nWifi1);
  cmd.AddValue ("nWifi2", "Number of wifi2 STA devices", nWifi2);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);

  cmd.Parse (argc,argv);

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults();

  // parse again so you can override default values from the command line
  cmd.Parse(argc, argv);

  Ptr<Node> pgw = epcHelper->GetPgwNode();

  if ((nWifi1 > 18) || (nWifi2 > 18))
    {
      std::cout << "Number of wifi nodes " << nWifi1 << " "<< nWifi2<<
                   " specified exceeds the mobility bounding box" << std::endl;
      exit(1);
    }

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (pgw->GetObject<Ipv4>());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address("10.1.1.0"), Ipv4Mask ("255.0.0.0"), 2);
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address("10.1.2.0"), Ipv4Mask ("255.0.0.0"), 3);
  
//create uenodes and enbnodes
  NodeContainer ueNodes;
  NodeContainer enbNodes;
  enbNodes.Create(numberOfNodes);
  ueNodes.Create(numberOfNodes);
  
 //create wifi nodes :wifinet1
  NodeContainer wifiStaNodes1;
  wifiStaNodes1.Create(nWifi1);
  NodeContainer wifiApNode1 = ueNodes.Get(0);
  YansWifiChannelHelper channel1 = YansWifiChannelHelper::Default();
  YansWifiPhyHelper phy1 = YansWifiPhyHelper::Default();
  phy1.SetChannel (channel1.Create());

  WifiHelper wifi1 = WifiHelper::Default ();
  wifi1.SetRemoteStationManager("ns3::AarfWifiManager");

  NqosWifiMacHelper mac1 = NqosWifiMacHelper::Default ();

  Ssid ssid1 = Ssid("ns-3-ssid");
  mac1.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid1),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices1;
  staDevices1 = wifi1.Install (phy1, mac1,wifiStaNodes1);

  mac1.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid1));

  NetDeviceContainer apDevices1;
  apDevices1 = wifi1.Install (phy1, mac1, wifiApNode1);

//create wifi nodes :wifinet2
  NodeContainer wifiStaNodes2;
  wifiStaNodes2.Create (nWifi2);
  NodeContainer wifiApNode2 = ueNodes.Get(0);
  YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default();
  YansWifiPhyHelper phy2 = YansWifiPhyHelper::Default();
  phy2.SetChannel(channel2.Create());

  WifiHelper wifi2 = WifiHelper::Default();
  wifi2.SetRemoteStationManager ("ns3::AarfWifiManager");

  NqosWifiMacHelper mac2 = NqosWifiMacHelper::Default ();

  Ssid ssid2 = Ssid ("ns-3-ssid");
  mac2.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid2),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices2;
  staDevices2 = wifi2.Install (phy2, mac2,wifiStaNodes2);

  mac2.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid2));

  NetDeviceContainer apDevices2;
  apDevices2 = wifi2.Install (phy2, mac2, wifiApNode2);


 // Install Mobility Model:uenodes and enbnodes

   Ptr<ListPositionAllocator> positionAlloc =   CreateObject<ListPositionAllocator>();
   for (uint16_t i = 0; i < numberOfNodes; i++) 
       {
	positionAlloc->Add(Vector(25 * (2 * i - 1), 0, 0));
       }
	
  MobilityHelper mobility;
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator(positionAlloc);
  mobility.Install(enbNodes);
  mobility.Install(ueNodes);

 // Install Mobility Model:wifinet1
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
	"Bounds", RectangleValue (Rectangle (-250, 250, -250, 250)));

  mobility.Install (wifiStaNodes1.Get(0));
  Ptr<RandomWalk2dMobilityModel> mob1 = wifiStaNodes1.Get(0)-> GetObject<RandomWalk2dMobilityModel>();
  mob1->SetPosition(Vector(-15, 0, 0.0));

  mobility.Install (wifiStaNodes1.Get(1));
  Ptr<RandomWalk2dMobilityModel> mob2 = wifiStaNodes1.Get(1)-> GetObject<RandomWalk2dMobilityModel>();
  mob2->SetPosition(Vector(-20, 10*sqrt(3)/2, 0.0));
  
  mobility.Install (wifiStaNodes1.Get(2));
  Ptr<RandomWalk2dMobilityModel> mob3 = wifiStaNodes1.Get(2)-> GetObject<RandomWalk2dMobilityModel>();
  mob3->SetPosition(Vector(-30, 10*sqrt(3)/2, 0.0));

  mobility.Install (wifiStaNodes1.Get(3));
  Ptr<RandomWalk2dMobilityModel> mob4 = wifiStaNodes1.Get(3)->GetObject<RandomWalk2dMobilityModel>();
  mob4->SetPosition(Vector(-35, 0, 0.0));

  mobility.Install (wifiStaNodes1.Get(4));
  Ptr<RandomWalk2dMobilityModel> mob5 = wifiStaNodes1.Get(4)->GetObject<RandomWalk2dMobilityModel>();
  mob5->SetPosition(Vector(-30, -10*sqrt(3)/2, 0.0));

  mobility.Install (wifiStaNodes1.Get(5));
  Ptr<RandomWalk2dMobilityModel> mob6 = wifiStaNodes1.Get(5)->GetObject<RandomWalk2dMobilityModel>();
  mob6->SetPosition(Vector(-20, -10*sqrt(3)/2, 0.0));

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode1);
  mobility.Install (pgw);
  pgw->GetObject<ConstantPositionMobilityModel>()->SetPosition(
Vector(0, 10, 0));
  wifiApNode1.Get(0)->GetObject<ConstantPositionMobilityModel>()->SetPosition(
Vector(-25, 0, 0));

// Install Mobility Model:wifinet2
  
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
	"Bounds", RectangleValue (Rectangle (-250, 250, -250, 250)));

  mobility.Install (wifiStaNodes2.Get(0));
  Ptr<RandomWalk2dMobilityModel> moc1 = wifiStaNodes2.Get(0)->GetObject<RandomWalk2dMobilityModel>();
  moc1->SetPosition(Vector(15, 0, 0.0));

  mobility.Install (wifiStaNodes2.Get(1));
  Ptr<RandomWalk2dMobilityModel> moc2 = wifiStaNodes2.Get(1)->GetObject<RandomWalk2dMobilityModel>();
  moc2->SetPosition(Vector(20, 10*sqrt(3)/2, 0.0));
  
  mobility.Install (wifiStaNodes2.Get(2));
  Ptr<RandomWalk2dMobilityModel> moc3 = wifiStaNodes2.Get(2)->GetObject<RandomWalk2dMobilityModel>();
  moc3->SetPosition(Vector(30, 10*sqrt(3)/2, 0.0));

  mobility.Install (wifiStaNodes2.Get(3));
  Ptr<RandomWalk2dMobilityModel> moc4 = wifiStaNodes2.Get(3)->GetObject<RandomWalk2dMobilityModel>();
  moc4->SetPosition(Vector(35, 0, 0.0));

  mobility.Install (wifiStaNodes2.Get(4));
  Ptr<RandomWalk2dMobilityModel> moc5 = wifiStaNodes2.Get(4)->GetObject<RandomWalk2dMobilityModel>();
  moc5->SetPosition(Vector(30, -10*sqrt(3)/2, 0.0));

  mobility.Install (wifiStaNodes2.Get(5));
  Ptr<RandomWalk2dMobilityModel> moc6 = wifiStaNodes2.Get(5)-> GetObject<RandomWalk2dMobilityModel>();
  moc6->SetPosition(Vector(20, -10*sqrt(3)/2, 0.0));

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode2);
  wifiApNode2.Get(0)->GetObject<ConstantPositionMobilityModel>()->SetPosition(
Vector(25, 0, 0));

// Install LTE Devices to the nodes
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice(enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice(ueNodes);
// Install the IP stack 
  InternetStackHelper internet;
  internet.Install(ueNodes);
  internet.Install(wifiStaNodes1);
  internet.Install(wifiStaNodes2);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueLteDevs));
// Assign IP address 
   for (uint32_t u = 0; u < ueNodes.GetN(); ++u) 
    {
	Ptr<Node> ueNode = ueNodes.Get(u);
	// Set the default gateway for the UE
	Ptr<Ipv4StaticRouting> ueStaticRouting =
	ipv4RoutingHelper.GetStaticRouting(ueNode->GetObject<Ipv4>());
	ueStaticRouting->SetDefaultRoute(
	epcHelper->GetUeDefaultGatewayAddress(), 1);
     }
//Assign wifinet1 IP address 
   Ipv4AddressHelper address;
   address.SetBase ("10.1.1.0", "255.255.255.0");
   Ipv4InterfaceContainer wifistaip1;
   wifistaip1 = address.Assign (staDevices1);
   address.Assign (apDevices1);
//Assign wifinet2 IP address  
   address.SetBase ("10.1.2.0", "255.255.255.0");
   Ipv4InterfaceContainer wifistaip2;
   wifistaip2 = address.Assign (staDevices2);
   address.Assign (apDevices2);
	
// Attach one UE per eNodeB
    for (uint16_t i = 0; i < numberOfNodes; i++) 
       {
	lteHelper->Attach(ueLteDevs.Get(i), enbLteDevs.Get(i));
       }
	
//install echo client and server
  UdpEchoServerHelper echoServer (9);
  ApplicationContainer serverApps = echoServer.Install(
	wifiApNode1.Get(0));
  serverApps.Start(Seconds(0.0));
  serverApps.Stop(Seconds(20.0));

  UdpEchoClientHelper echoClient(Ipv4Address("7.0.0.0"), 9);
  echoClient.SetAttribute("MaxPackets", UintegerValue(4));
  echoClient.SetAttribute("Interval", TimeValue(Seconds(0.1)));
  echoClient.SetAttribute("PacketSize", UintegerValue(512));
	
  for (int i = 0; i < (int) nWifi2; i++) 
 {
  ApplicationContainer clientApps = echoClient.Install(
  wifiStaNodes2.Get(nWifi2 - 1 - i));
  clientApps.Start(Seconds(2.0 + i));
  clientApps.Stop(Seconds(10.0 + i));
  }

  AsciiTraceHelper ascii_1,ascii_2;
  phy1.EnableAscii (ascii_1.CreateFileStream("wifi1.tr"), apDevices1.Get(0));
  phy2.EnableAscii (ascii_2.CreateFileStream("wifi2.tr"), apDevices2.Get(0));
  phy1.EnablePcap("wifi1", apDevices1.Get(0));
  phy2.EnablePcap("wifi2", apDevices2.Get(0));

  Simulator::Stop(Seconds(20.0));
  Simulator::Run ();
  Simulator::Destroy();
  return 0;
}
