CC=g++

CFLAGS=-Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE

OBJDIR=_objs/
DEPSDIR=_deps/

LIB_SRC=flog.cpp
FIFO_SRC=fifo.cpp fifo_lib.cpp

all: $(DEPSDIR) $(OBJDIR) fifo.out

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

fifo.out: $(addprefix $(OBJDIR),$(FIFO_SRC:.cpp=.o) $(LIB_SRC:.cpp=.o))
	$(CC) $(CFLAGS) $^ -o $@

test:
	make all
	gnome-terminal --working-directory=$(CURDIR) -- watch -n 0.3 ls -l
	./fifo.out > output.output

logs:
	for file in logs_out_*.html; do xdg-open "$$file"; done

clean:
	rm -rf $(OBJDIR) $(DEPSDIR)
	rm -rf *.png *.dot *.o *.out *.lol* *.rng *.html
	clear

.PHONY: clean logs test