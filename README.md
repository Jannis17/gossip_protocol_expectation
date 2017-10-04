# Gossip Protocol Expectation 

Ioannis Kokkinis, ykokkinis@gmail.com

### Short Description:

The aim of this project is to compute the exact value
of the expected duration
of some sequential distributed gossip protocols. 
The current version of
this project computes the expected duration for the protocols
"Learn New Secrets" (LNS) and "Any Call" (ANY) that are
defined in the paper: https://arxiv.org/abs/1511.00867. 
The expected duration is computed by constructing the
Markov chain that corresponds to the execution of
the protocols and then by computing the average time that this Markov
chain needs in order to hit the absorption state (i.e. the state where
all the agents know all the secrets). A detailed explanation
of the algorithm can be found in section 4 of the following
paper: https://sites.google.com/site/ykokkinis/prima17.pdf
We use the nauty program for checking the graphs that we
generate for isomorphism. The files of this program are
in the folder /gossip_protocol_expectation/nauty26r7 and
can be build using the the installation instructions from
the page http://pallini.di.uniroma1.it/.

The downloading, building and running instructions are
for a UNIX system. The commands should be run in a terminal.

---

### Download:

git clone https://github.com/Jannis17/gossip_protocol_expectation

---

### Build:


Navigate to "gossip_protocol_expectation/protocol/src" and type "make".

---

### Run:
Navigate to "gossip_protocol_expectation/protocl/src" and type
"./protocol". Then the instructions will be typed in your terminal.
