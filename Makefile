build: nauty/nauty.a
	cd protocol/src/ && make protocol

nauty/nauty.a: nauty/*.c
	cd nauty/ && ./configure && make all

run:
	cd protocol/src/ && ./protocol
