# Gossip Protocol Expectation 

Ioannis Kokkinis, ykokkinis@gmail.com

### Short Description:

The aim of this project is to compute the expected duration
of some sequential gossip protocols. The current version of
this project computes the expected duration of the protocol
"Learn New Secrets" (LNS) that is defined in the paper
https://arxiv.org/abs/1511.00867. The expected duration
is computed by constructing the
Markov chain that corresponds to the LNS protocol and then
by computing the average time that this Markov chain needs
in order to hit the absorption state (i.e. the state where
all the agents know all the secrets).

The downloading, building and running instructions are
for a UNIX system. The commands should be run in the
command line.

---

### Download:

git clone https://github.com/Jannis17/gossip_protocol_expectation

---

### Build:

We use the nauty program for checking the graphs that we
generate for isomorphism. The files of this program are
in the folder /gossip_protocol_expectation/nauty26r7 and
can be build using the the installation instructions from
the page http://pallini.di.uniroma1.it/.

In order to build our program navigate to the folder

gossip_protocol_expectation/LNS/src

and type make.

---

### Run:
Navigate to the folder

gossip_protocol_expectation/protocl/src

and type ./protocol [protocol_name] [aMin] [aMax]

where aMin and aMax are the minimum and maximum number
of agents respectively. Protocol_name should be ANY or LNS. The current version of our
program will give results in reasonable time (approx. 5 minutes) for aMax <= 7.

The results will be generated in a file with timestamp in the folder gossip_protocol_expectation/results.

---

### Examples

The Markoc Chain that corresponds to the execution of LNS with 4 agents can be found here: https://sites.google.com/site/ykokkinis/MarkovChain4agents.pdf.
The states of the Markov chain are tuples that describe the configuration of secrets.
The transition matrix and the expected duration of the protocol can be found here: https://sites.google.com/site/ykokkinis/transMatrix4agents.pdf. The states in the
transition matrix are numbered bottom to top, left to right as they appear in the
Markov Chain.
