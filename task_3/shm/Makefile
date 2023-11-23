CC=g++

CFLAGS=-Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE

OBJDIR=_objs/
DEPSDIR=_deps/

LIB_SRC=flog.cpp shm_lib.cpp
SRC_CLIENT=shm_client.cpp
SRC_SERVER=shm_server.cpp

all: $(DEPSDIR) $(OBJDIR) shm_client.out shm_server.out

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

shm_client.out: $(addprefix $(OBJDIR),$(SRC_CLIENT:.cpp=.o) $(LIB_SRC:.cpp=.o))
	$(CC) $(CFLAGS) $^ -o $@

shm_server.out: $(addprefix $(OBJDIR), $(SRC_SERVER:.cpp=.o) $(LIB_SRC:.cpp=.o))
	$(CC) $(CFLAGS) $^ -o $@

logs:
	for file in logs_out*.html; do xdg-open "$$file"; done

test:
	make all
	gnome-terminal --working-directory=$(CURDIR) -- watch -n 0.3 ls -l
	./shm_client.out >output.output 2> client.errors & ./shm_server.out 2> server.errors

test_small:
	make all
	gnome-terminal --working-directory=$(CURDIR) -- watch -n 0.3 ls -l
	./shm_client.out --small >output.output 2>client.errors & ./shm_server.out 2> server.errors

clean:
	rm -rf $(OBJDIR) $(DEPSDIR)
	rm -rf *.png *.dot *.o *.out *.lol* *.rng *.html *.errors
	clear

.PHONY: clean logs test test_small