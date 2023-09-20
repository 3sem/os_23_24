CC=g++

CFLAGS=-Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE

OBJDIR=_objs/
DEPSDIR=_deps/

WRAP_SRC=shell-wrapper-4.20.cpp shell-wrapper-4.20-lib.cpp
TEST_SRC=tester.cpp
TEST1_SRC=tester2.cpp
TEST2_SRC=tester3.cpp

all: $(DEPSDIR) $(OBJDIR) wrap.out test.out test1.out test2.out

-include $(addprefix $(DEPSDIR),*.d)

$(OBJDIR)%.o: %.cpp
	$(CC) -M $(CFLAGS) $< -o $(DEPSDIR)$(@F:.o=.d)
	echo "$(OBJDIR)" > line.tmp
	mv $(DEPSDIR)$(@F:.o=.d) input.tmp
	head -c -1 -q line.tmp input.tmp > $(DEPSDIR)$(@F:.o=.d)
	rm input.tmp line.tmp
	$(CC) -c $(CFLAGS) $< -o $@

wrap.out: $(addprefix $(OBJDIR),$(WRAP_SRC:.cpp=.o))
	$(CC) $(CFLAGS) $^ -o $@

test.out: $(addprefix $(OBJDIR), $(TEST_SRC:.cpp=.o))
	$(CC) $(CFLAGS) $^ -o $@

test1.out: $(addprefix $(OBJDIR), $(TEST1_SRC:.cpp=.o))
	$(CC) $(CFLAGS) $^ -o $@

test2.out: $(addprefix $(OBJDIR), $(TEST2_SRC:.cpp=.o))
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR):
	mkdir $(OBJDIR)

$(DEPSDIR):
	mkdir $(DEPSDIR)

clean:
	rm -rf $(OBJDIR) $(DEPSDIR)
	rm -rf *.png *.dot *.o *.out *.lol*
	clear

.PHONY: clean