.PHONY: bild visualize test score

PROBLEM := ConnectedComponent
CXX := g++
CXXFLAGS := -std=c++11 -Wall -O2

RANDOM = $(shell bash -c 'echo $$RANDOM')
SEED ?= ${RANDOM}
SEED := ${SEED}

build: a.out
a.out: ${PROBLEM}.cpp
	${CXX} ${CXXFLAGS} -g -DLOCAL $<
a.out.visualize: ${PROBLEM}.cpp
	${CXX} ${CXXFLAGS} -g -DLOCAL $< -o $@ -DVISUALIZE
test/${SEED}.in:
	./generate.py ${SEED} | sponge test/${SEED}.in
visualize: a.out.visualize test/${SEED}.in
	./a.out.visualize < test/${SEED}.in 2>&1 >/dev/null | tee /dev/stderr | ./visualize.py test/${SEED}.in
test: build
	java -jar tester.jar -exec ./a.out -seed ${SEED} -vis
TIMESTAMP := $(shell date +%s)
score: a.out
	unbuffer ./evaluate.py -j2 -s5 | tee log/${TIMESTAMP}.log
	./plot.py log/${TIMESTAMP}.log
