.PHONY: visualize score

PROBLEM := ConnectedComponent
CXX := g++
CXXFLAGS := -std=c++11 -Wall -O2

RANDOM = $(shell bash -c 'echo $$RANDOM')
SEED ?= ${RANDOM}
SEED := ${SEED}
visualize:
	${CXX} ${CXXFLAGS} -g -DLOCAL ${PROBLEM}.cpp
	[ -e test/${SEED}.in ] || ./generate.py ${SEED} > test/${SEED}.in
	./a.out < test/${SEED}.in 2>&1 >/dev/null | ./visualize.py test/${SEED}.in
score:
	${CXX} ${CXXFLAGS} -g -DLOCAL ${PROBLEM}.cpp
	for i in `seq 10` ; do java -jar tester.jar -exec ./a.out -seed `expr ${SEED} + $$i` ; done | tee /dev/stderr | grep '^Score = ' | awk '{ a += $$3 } END { print "Total = " (a / 10) }'
