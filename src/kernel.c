#define WHITE_TXT 0x07
#define VIDEO_MEMORY 0xb8000

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

#define VGA_COMMAND_PORT 0x3D4
#define VGA_DATA_PORT 0x3D5

unsigned char keyboard_map[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*',
    0,
    ' ',
    0,
};

struct Screen {
    int screenrows;
    int screencols;
} screen = { 25, 80 };

int cursor_position = 0;
char *vidmem = (char *) 0xb8000;

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(unsigned short port, unsigned char data) {
    asm volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

void k_set_cursor(int position) {
    outb(VGA_COMMAND_PORT, 0x0F);
    outb(VGA_DATA_PORT, (unsigned char)(position & 0xFF));

    outb(VGA_COMMAND_PORT, 0x0E);
    outb(VGA_DATA_PORT, (unsigned char)((position >> 8) & 0xFF));
}

void k_print_char(char c) {
    if (c == '\n') {
        int current_row = cursor_position / (80 * 2);
        cursor_position = (current_row + 1) * 80 * 2;
    } else {
        vidmem[cursor_position] = c;
        vidmem[cursor_position + 1] = 0x07;
        cursor_position += 2;
    }

    k_set_cursor(cursor_position / 2);
}


void keyboard_handler() {
    unsigned char scancode = inb(KEYBOARD_DATA_PORT);

    if (scancode > 128) {
        return;
    }

    char key = keyboard_map[scancode];
    if (key != 0) {
        k_print_char(key);
    }
}

void k_print(const char *message) {
    char *vidmem = (char *) VIDEO_MEMORY;

    while (*message) {
        if (*message == '\n') {
            int current_row = cursor_position / (screen.screencols * 2);
            cursor_position = (current_row + 1) * screen.screencols * 2;
        } else {
            vidmem[cursor_position] = *message;
            vidmem[cursor_position + 1] = WHITE_TXT;
            cursor_position += 2;
        }
        message++;
    }

    k_set_cursor(cursor_position / 2);
}

void k_clean_screen() {
    char *vidmem = (char *) VIDEO_MEMORY;
    int screen_size = screen.screenrows * screen.screencols;

    for (int i = 0; i < screen_size; i++) {
        *vidmem++ = ' ';
        *vidmem++ = WHITE_TXT;
    }

    cursor_position = 0;
    k_set_cursor(cursor_position / 2);
}

void k_main() {
    k_clean_screen();

    k_print("Hello, world!\n");
    k_print("My own kernel!");

    while (1) {
        keyboard_handler();
    }
}
