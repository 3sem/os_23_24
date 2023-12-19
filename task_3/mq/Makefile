CC=g++

CFLAGS=-Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE -lrt

OBJDIR=_objs/
DEPSDIR=_deps/

LIB_SRC=mq.c
SRC_CLIENT=mq_client.c
SRC_SERVER=mq_server.c

all: $(DEPSDIR) $(OBJDIR) mq_client.out mq_server.out

# Part that generates all dependency files and object files using those
-include $(addprefix $(DEPSDIR),*.d)

$(OBJDIR)%.o: %.c
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

mq_client.out: $(addprefix $(OBJDIR),$(SRC_CLIENT:.c=.o) $(LIB_SRC:.c=.o))
	$(CC) $(CFLAGS) $^ -o $@

mq_server.out: $(addprefix $(OBJDIR), $(SRC_SERVER:.c=.o) $(LIB_SRC:.c=.o))
	$(CC) $(CFLAGS) $^ -o $@

logs:
	for file in logs_out*.html; do xdg-open "$$file"; done

test:
	rm -rf logs_out_*.html
	make all
	./mq_server.out 2> server.errors & ./mq_client.out >output.output 2> client.errors
	sha256sum *.rng >> output.output
	code output.output

test_small:
	rm -rf logs_out_*.html *.errors
	make all
	(sleep 3s ; pkill -f mq_server.out & pkill -f mq_client.out) &
	./mq_server.out 2> server.errors & ./mq_client.out --small >output.output 2>client.errors
	sha256sum *.rng >> output.output
	code output.output

clean:
	rm -rf $(OBJDIR) $(DEPSDIR)
	rm -rf *.png *.dot *.o *.out *.lol* *.rng *.html *.errors
	clear

.PHONY: clean logs test test_small check