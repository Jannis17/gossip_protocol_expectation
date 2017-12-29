# Gossip Protocol Expectation

Ioannis Kokkinis, ykokkinis@gmail.com

### Short Description:

The aim of this project is to compute the expected duration of all the gossip protocols that are
defined in the paper: https://arxiv.org/abs/1511.00867.
Our implementation provides algorithms for:

a) computing the exact value of the expected duration of the protocols for at most 10
agents

b) computing the approximated value of the expectation for at most 200 agents

The exact value of the expected duration is computed by constructing the
Markov chain that corresponds to the execution of
the protocols and then by computing the average time that this Markov
chain needs in order to hit the absorption state (i.e. the state where
all the agents know all the secrets). A detailed explanation of our algorithm
can be found in section 4 of the following paper: https://sites.google.com/site/ykokkinis/prima17.pdf.
The isomorphism check that is needed for
our algorithm is done with the help of the software nauty (http://pallini.di.uniroma1.it/).
The approximated value is computed via simulations. 

The downloading, building and running instructions are
for a UNIX system. The commands should be run in a terminal.

---

### Download:

Navigate to a folder of your choosing an type
"git clone https://github.com/Jannis17/gossip_protocol_expectation".

---

### Build:


Navigate to "gossip_protocol_expectation/nauty26r7" and type "./configure" and then "make".
Then navigate to "gossip_protocol_expectation/protocol/src" and type "make".

---

### Run:
Navigate to "gossip_protocol_expectation/protocl/src" and type
"./protocol". Follow the instructions that will be type in your terminal.
