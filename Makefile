.PHONY: bild visualize score

PROBLEM := ConnectedComponent
CXX := g++
CXXFLAGS := -std=c++11 -Wall -O2

RANDOM = $(shell bash -c 'echo $$RANDOM')
SEED ?= ${RANDOM}
SEED := ${SEED}
build:
	${CXX} ${CXXFLAGS} -g -DLOCAL ${PROBLEM}.cpp
visualize:
	${CXX} ${CXXFLAGS} -g -DLOCAL ${PROBLEM}.cpp -DVISUALIZE
	[ -e test/${SEED}.in ] || ./generate.py ${SEED} | sponge test/${SEED}.in
	./a.out < test/${SEED}.in 2>&1 >/dev/null | tee /dev/stderr | ./visualize.py test/${SEED}.in
test: build
	java -jar tester.jar -exec ./a.out -seed ${SEED} -vis
TIMESTAMP := $(shell date +%s)
score: build
	unbuffer ./evaluate.py | tee log/${TIMESTAMP}.log
	./plot log/${TIMESTAMP}.log
