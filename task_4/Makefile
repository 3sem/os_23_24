CC=g++

CFLAGS=-Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE -lrt

OBJDIR=_objs/
DEPSDIR=_deps/

SRC=calculator.cpp
SRC_TEST=tester.cpp

all: $(DEPSDIR) $(OBJDIR) $(SRC:.cpp=.out) $(SRC_TEST:.cpp=.out)

# Part that generates all dependency files and object files using those
-include $(addprefix $(DEPSDIR),*.d)

$(OBJDIR)%.o: %.cpp
	$(CC) -M $(CFLAGS) $< -o $(DEPSDIR)$(@F:.o=.d)
	echo "$(OBJDIR)" > line.tmp
	mv $(DEPSDIR)$(@F:.o=.d) input.tmp
	head -c -1 -q line.tmp input.tmp > $(DEPSDIR)$(@F:.o=.d)
	rm input.tmp line.tmp
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR):
	mkdir $(OBJDIR)

$(DEPSDIR):
	mkdir $(DEPSDIR)

# End of this part (kinda standart part (i definitely should create a proj template))

$(SRC:.cpp=.out): $(addprefix $(OBJDIR),$(SRC:.cpp=.o))
	$(CC) $(CFLAGS) $^ -o $@

$(SRC_TEST:.cpp=.out): $(addprefix $(OBJDIR), $(SRC_TEST:.cpp=.o))
	$(CC) $(CFLAGS) $^ -o $@

test:
	make all
	./$(SRC_TEST:.cpp=.out) $(SRC:.cpp=.out) 100
	gnuplot plot_script.gp

small_test:
	make all
	./$(SRC_TEST:.cpp=.out) $(SRC:.cpp=.out) 20
	gnuplot plot_script.gp

clean:
	rm -rf $(OBJDIR) $(DEPSDIR)
	rm -rf *.o *.out *.lol* *.rng *.html *.errors
	clear

.PHONY: clean logs test test_small check