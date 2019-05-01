# Implementation for Reachability and Expectation in Gossiping

Ioannis Kokkinis, ykokkinis@gmail.com

### Short Description:

For every protocol defined in the paper "Dynamic Gossip" (https://arxiv.org/abs/1511.00867) our implementation provides algorithms for:

a) computing the exact value of its expected duration (for at most 8 agents - the upper bound varies on the protocol)

b) computing the approximate value of its expectated duration (for up to 200 agents)

c) producing the reachable states

in COMPLETE networks.

Detailed definition for the above notions and an explanation
of our algorithms can be found in the paper
"Reachability and Expectation in Gossiping"
(https://sites.google.com/site/ykokkinis/prima17.pdf).
The isomorphism check that is needed for our algorithms is done with
the help of the software nauty (http://pallini.di.uniroma1.it/).

An example of the states reachable by protocol ANY can be found here:
https://drive.google.com/open?id=1F5TkkTqyiNAP7uUlZuCrOWqgHHtArrVG.

An example of the states reachable by protocol LNS can be found here:
https://drive.google.com/file/d/1-YCz0MieU4f2kjrRbd02SWPtiE0dH-wg/view?usp=sharing


The downloading, building and running instructions are for a
UNIX system. The commands should be run in a terminal.

---

### Download:
Type "git clone https://github.com/Jannis17/gossip_protocol_expectation".

---

### Build:


Navigate to the folder "gossip_protocol_expectation/protocol/src" and type "make all".

---

### Run:

Navigate to the folder "gossip_protocol_expectation/protocl/src" and type
"./protocol". Follow the instructions in your terminal.
