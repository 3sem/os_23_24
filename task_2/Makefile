CC=g++

CFLAGS=-Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE

OBJDIR=_objs/
DEPSDIR=_deps/

LIB_SRC=flog.cpp
DUPLEX_SRC=duplex.cpp duplex-lib.cpp
RNG_FILE_NAME=rng_5gb_file.rng

all: $(DEPSDIR) $(OBJDIR) duplex.out $(RNG_FILE_NAME)

-include $(addprefix $(DEPSDIR),*.d)

$(OBJDIR)%.o: %.cpp
	$(CC) -M $(CFLAGS) $< -o $(DEPSDIR)$(@F:.o=.d)
	echo "$(OBJDIR)" > line.tmp
	mv $(DEPSDIR)$(@F:.o=.d) input.tmp
	head -c -1 -q line.tmp input.tmp > $(DEPSDIR)$(@F:.o=.d)
	rm input.tmp line.tmp
	$(CC) -c $(CFLAGS) $< -o $@

duplex.out: $(addprefix $(OBJDIR),$(DUPLEX_SRC:.cpp=.o) $(LIB_SRC:.cpp=.o))
	$(CC) $(CFLAGS) $^ -o $@

$(RNG_FILE_NAME):
	dd if=/dev/urandom of=$@ bs=1G count=5

small:
	dd if=/dev/urandom of=$(RNG_FILE_NAME) bs=128 count=128

$(OBJDIR):
	mkdir $(OBJDIR)

$(DEPSDIR):
	mkdir $(DEPSDIR)

clean:
	rm -rf $(OBJDIR) $(DEPSDIR)
	rm -rf *.png *.dot *.o *.out *.lol* *.rng
	clear

.PHONY: clean