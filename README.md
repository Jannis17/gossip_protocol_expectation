# Gossip Protocol Expectation 

Ioannis Kokkinis, ykokkinis@gmail.com

### Short Description:

The aim of this project is to compute the expected duration
of some sequential gossip protocols. The current version of
this project computes the expected duration of the protocol
lns that is defined in https://arxiv.org/abs/1511.00867.
The expected duration is computed by constructing the
Markov chain that corresponds to the LNS protocol and then
by computing the average time that this Markov chain needs
in order to hit the absorption state (i.e. the state where
all the agents know all the secrets).

---

### Download:

git clone https://github.com/Jannis17/gossip_protocol_expectation

---

### Build:

We use the nauty program for checking the graphs that we
generate for isomorphism. The files of this program are
in the folder /gossip_protocol_expectation/nauty26r7 and
can be build using the the installation directives from
the page http://pallini.di.uniroma1.it/.

In order to build our program navigate to the folder

gossip_protocol_expectation/LNS/src

and type make.

---

### Run:
Navigate to the folder

gossip_protocol_expectation/LNS/src

and type ./lns [aMin] [aMax]

where aMin and aMax are the minimum and maximum number
of agents respectively. The current version of our
program will give results in reasonable time
(approx. 15 minutes) for aMax <= 7.
