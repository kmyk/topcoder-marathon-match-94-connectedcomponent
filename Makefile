.PHONY: visualize

PROBLEM := ConnectedComponent
CXX := g++
CXXFLAGS := -std=c++11 -Wall -O2

RANDOM = $(shell bash -c 'echo $$RANDOM')
SEED ?= ${RANDOM}
SEED := ${SEED}
visualize:
	${CXX} ${CXXFLAGS} -g -DLOCAL ${PROBLEM}.cpp
	java -jar tester.jar -exec ./a.out -seed ${SEED}
