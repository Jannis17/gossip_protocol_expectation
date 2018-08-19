# Implementation for Reachability and Expectation in Gossiping

Ioannis Kokkinis, ykokkinis@gmail.com

### Short Description:

For every protocol defined in the paper "Dynamic Gossip" (https://arxiv.org/abs/1511.00867) our implementation provides algorithms for:

a) computing the exact value of its expected duration

b) computing the approximate value of its expectated duration

c) producing the reachable states

in COMPLETE networks.

Detailed definition for the above notions can be found in the paper "Reachability and Expectation in Gossiping" (https://sites.google.com/site/ykokkinis/prima17.pdf).
The algorithms are explained in section 4 of the aforementiond paper. The isomorphism check that is needed for our algorithms is done with the
help of the software nauty (http://pallini.di.uniroma1.it/).

The downloading, building and running instructions are for a UNIX system. The commands should be run in a terminal.

---

### Download:
Type "git clone https://github.com/Jannis17/gossip_protocol_expectation".

---

### Build:


Navigate to folder "gossip_protocol_expectation/nauty" and type "./confige" and then "make".
Then navigate to "gossip_protocol_expectation/protocol/src" and type "make".

---

### Run:

Navigate to "gossip_protocol_expectation/protocl/src" and type
"./protocol". Follow the instructions in your terminal.
