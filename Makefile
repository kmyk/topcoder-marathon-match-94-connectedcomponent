.PHONY: bild visualize score

PROBLEM := ConnectedComponent
CXX := g++
CXXFLAGS := -std=c++11 -Wall -O2

RANDOM = $(shell bash -c 'echo $$RANDOM')
SEED ?= ${RANDOM}
SEED := ${SEED}
build:
	${CXX} ${CXXFLAGS} -g -DLOCAL ${PROBLEM}.cpp
visualize: build
	[ -e test/${SEED}.in ] || ./generate.py ${SEED} | sponge test/${SEED}.in
	./a.out < test/${SEED}.in 2>&1 >/dev/null | tee /dev/stderr | ./visualize.py test/${SEED}.in
test: build
	java -jar tester.jar -exec ./a.out -seed ${SEED} -vis
score: build
	for i in `seq 10` ; do seed=`expr ${SEED} + $$i` ; echo Seed = $$seed ; java -jar tester.jar -exec ./a.out -seed $$seed ; done | tee /dev/stderr | grep '^MESSAGE: ratio = ' | awk '{ a += $$4 } END { print "Total = " (a / 10) }'
