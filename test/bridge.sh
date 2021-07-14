#!/bin/sh
# Verifies (*,G) routing between VLANs on top of a VLAN filtering bridge
# with bridge ports as VETH interfaces.  UDP injection with nemesis, see
# test/README.md as for why.
set -x

echo "Creating world ..."
ip link add br0 type bridge vlan_filtering 1 mcast_snooping 0
ip link add a1 type veth peer b1
ip link add a2 type veth peer b2
ip link set b1 master br0
ip link set b2 master br0

ip link set a1 up
ip link set b1 up
ip link set a2 up
ip link set b2 up
ip link set br0 up

ip link add link br0 vlan1 type vlan id 1
ip link add link br0 vlan2 type vlan id 2

ip link set vlan1 up
ip link set vlan2 up

# Move b2 to VLAN 2
bridge vlan add vid 2 dev b2 pvid untagged
bridge vlan del vid 1 dev b2

# Set br0 as tagged member of both VLANs
bridge vlan add vid 1 dev br0 self
bridge vlan add vid 2 dev br0 self

# IP World
ip addr add 10.0.0.1/24 dev vlan1
ip addr add 20.0.0.1/24 dev vlan2

echo "Creating config ..."
cat <<EOF > bridge.conf
# vlan (*,G) multicast routing
phyint vlan1 enable
phyint vlan2 enable
mroute from vlan1 group 225.1.2.3 to vlan2
EOF
cat bridge.conf

echo "Starting smcrouted ..."
../src/smcrouted -f bridge.conf -n -N -P /tmp/smcrouted.pid &
sleep 1

echo "Starting collector ..."
tcpdump -c 2 -lni a2 -w bridge.pcap dst 225.1.2.3 &
sleep 1

echo "Starting emitter ..."
nemesis udp -c 3 -S 10.0.0.10 -D 225.1.2.3 -T 3 -M 01:00:5e:01:02:03 -d a1

# Show active routes (and counters)
cat /proc/net/ip_mr_cache
ip mroute

echo "Cleaning up ..."
killall smcrouted
sleep 1
ip link del br0
ip link del a1
ip link del a2

echo "Analyzing ..."
lines=$(tcpdump -r bridge.pcap | grep 225.1.2.3 | tee bridge.result | wc -l)
cat bridge.result
echo "=> num routes frames $lines"

# one frame lost due to initial (*,G) -> (S,G) setup
[ "$lines" = "2" ] && exit 0
exit 1