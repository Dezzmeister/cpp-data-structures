SRC_DIR := src
INC_DIR := include
TEST_SRC_DIR := test/src
TEST_INC_DIR := test/include
TEST_BINARY := test_bin

CXX := clang++
CXXFLAGS := -Wall -Werror -std=gnu++23 

ifeq (${CXX}, g++)
	CXXFLAGS += -fconcepts-diagnostics-depth=2
endif

HEADERS = \
		${INC_DIR}/structures/trie.h \
		${INC_DIR}/structures/radix_trie.h \
		${INC_DIR}/structures/radix_trie_iterator.h \
		${INC_DIR}/structures/radix_trie_node.h \
		${INC_DIR}/structures/sorted_vec.h \
		${INC_DIR}/traits.h

OBJS = \
		${SRC_DIR}/main.o 

OBJS_NO_MAIN = $(filter-out ${SRC_DIR}/main.o, ${OBJS})

TEST_HEADERS = \
		${TEST_INC_DIR}/utils.h \
		${TEST_INC_DIR}/setup.h

TEST_OBJS = \
		${TEST_SRC_DIR}/main.o \
		${TEST_SRC_DIR}/trie.o \
		${TEST_SRC_DIR}/radix_trie.o \
		${TEST_SRC_DIR}/sorted_vec.o

.PHONY: clean

debug: CXXFLAGS += -Og -fsanitize=unreachable
release: CXXFLAGS += -O3 -march=native
test: CXXFLAGS += -DTEST
memtest: CXXFLAGS += -DTEST
invtest: CXXFLAGS += -DTEST -DINVERT_EXPECT

debug: ${OBJS}
	${CXX} -o $@ $^ ${CXXFLAGS}

release: ${OBJS}
	${CXX} -o $@ $^ ${CXXFLAGS}

test: ${OBJS_NO_MAIN} ${TEST_OBJS}
	${CXX} -o ${TEST_BINARY} $^ ${CXXFLAGS} && ./${TEST_BINARY} ; rm -f ./${TEST_BINARY}

memtest: ${OBJS_NO_MAIN} ${TEST_OBJS}
	${CXX} -o ${TEST_BINARY} $^ ${CXXFLAGS} && valgrind --track-origins=yes ./${TEST_BINARY} ; rm -f ./${TEST_BINARY}

invtest: ${OBJS_NO_MAIN} ${TEST_OBJS}
	${CXX} -o ${TEST_BINARY} $^ ${CXXFLAGS} && ./${TEST_BINARY} ; rm -f ./${TEST_BINARY}

%.o: %.cpp ${HEADERS} ${TEST_HEADERS}
	${CXX} -c -o $@ $< ${CXXFLAGS}

clean:
	find . -name '*.o' -delete
	rm -f debug
	rm -f release
