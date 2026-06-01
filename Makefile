CC     = gcc
CFLAGS = -Wall -Wextra -O2 -I"Battlesips final version"
SRCDIR = Battlesips final version
SRCS   = "$(SRCDIR)/Source.c" \
         "$(SRCDIR)/gameplay.c" \
         "$(SRCDIR)/graphics_and_ui.c" \
         "$(SRCDIR)/enemy_behavior.c" \
         "$(SRCDIR)/save_load.c" \
         "$(SRCDIR)/platform.c"
OUT    = plundercells

all:
	$(CC) $(CFLAGS) $(SRCS) -o $(OUT)

run: all
	./$(OUT)

clean:
	rm -f $(OUT)
