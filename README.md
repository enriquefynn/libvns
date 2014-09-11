This is a fork from [vns](http://www.dcc.fc.up.pt/~rjf/vns/) Vehicular Network Simulator,
a simulator for VANET.

VNS is a high-performance simulator for vehicular networks. It was developed by 
[Ricardo Fernandes](http://www.dcc.fc.up.pt/~rjf/) at DCC-FCUP and Instituto de 
Telecomunicações | Porto, supported by the Portuguese Foundation for Science and 
Technology (FCT) under the doctoral grant (SFRH/BD/61676/2009).

VNS is a simulation framework that completely integrates the mobility and 
network components in a transparent and efficient way, reducing the overhead 
of communication and synchronization between different simulators. VNS provides 
bi-directionally interaction between a microscopic mobility model and network 
simulators such as [NS-3](http://www.nsnam.org/) or [OMNET++](http://www.omnetpp.org/).


Main Features:

* Integrated framework for vehicular networks simulation.
* Transparent bi-directional coupling between mobility and network components.
* Efficient communication and synchronization.
* Several microscopic mobility models (IDM,GIPPS,etc.).
* Several protocol modules and radio access technologies such as 802.11p 
(NS-3 and OMNET++ modules).
* Parallel traffic simulations.
* Large-scale simulations.
* Can import different map formats (OpenStreetMap,Tiger,Shapefiles,etc.).
* 2D and 3D Visualization.
* Well designed C++ library. Easy to extend and integrate with other projects.

Mobility Simulation

The mobility model of VNS is based on DIVERT, a microscopic traffic simulator developed by Prof. Luís Damas at the University of Porto. However, in order to create a scalable and realistic VANET simulator, we have redesigned a new traffic simulator from scratch. VNS was designed to support efficienty integration with other network simulators, where the level of interaction between both simulators not only allows vehicular mobility to affect the network component, but also the opposite. This new mobility simulator can also be called as "DIVERT 2.0", but is now part of the VNS framework.
Network Simulation

Currently, besides a simple network module, VNS can use NS-3 or OMNET++ as network simulators. NS-3 is a discrete-event network simulator widely used by the research community. It provides various protocol modules and several radio access technologies. In order to increase the performance and scalability, NS-3 was extended to enable efficient neighbor searching during wireless transmissions. These improvements resulted in significant performance gains, enabling simulations of large-scale vehicular networks.

OMNET++ is an extensible, modular and component-based C++ simulation framework with a large community. Under the [Veins](http://veins.car2x.org/) project, researchers have developed fully-detailed models of IEEE 802.11p and IEEE 1609.4 DSRC/WAVE that can be used for advanced VANET simulations. 

