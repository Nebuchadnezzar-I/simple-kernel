#define WHITE_TXT 0x07
#define VIDEO_MEMORY 0xb8000

#define KEYBOARD_DATA_PORT 0x60
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
char *vidmem = (char *) VIDEO_MEMORY;

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
        // Move to the start of the next line
        int current_row = cursor_position / (screen.screencols * 2);
        cursor_position = (current_row + 1) * screen.screencols * 2;

        // Ensure the cursor doesn't exceed the screen size
        if (cursor_position >= screen.screenrows * screen.screencols * 2) {
            cursor_position = 0;  // Reset cursor to the top
        }
    } else {
        // Write the character to video memory and advance the cursor
        vidmem[cursor_position] = c;
        vidmem[cursor_position + 1] = WHITE_TXT;  // Set attribute (color)
        cursor_position += 2; // Move to the next character cell

        // Ensure the cursor doesn't exceed the screen size
        if (cursor_position >= screen.screenrows * screen.screencols * 2) {
            cursor_position = 0;  // Reset cursor to the top
        }
    }

    // Update the hardware cursor
    k_set_cursor(cursor_position / 2);
}

void keyboard_handler() {
    unsigned char scancode = inb(KEYBOARD_DATA_PORT);

    if (scancode > 128) {
        // Key release, not handled in this simple version
        return;
    }

    char key = keyboard_map[scancode];
    if (key != 0) {
        k_print_char(key);  // Print the pressed character to the screen
    }
}

void k_print(const char *message) {
    while (*message) {
        k_print_char(*message);
        message++;
    }

    // Update the hardware cursor after the message is printed
    k_set_cursor(cursor_position / 2);
}

void k_clean_screen() {
    int screen_size = screen.screenrows * screen.screencols;

    // Clear the screen by writing spaces to all positions
    for (int i = 0; i < screen_size; i++) {
        vidmem[i * 2] = ' ';       // Blank character
        vidmem[i * 2 + 1] = WHITE_TXT; // White text on black background
    }

    cursor_position = 0;
    k_set_cursor(cursor_position / 2);  // Reset cursor to the top-left corner
}

void k_main() {
    k_clean_screen();

    k_print("Hello, world!\n");
    k_print("My own kernel!\n");

    while (1) {
        keyboard_handler();  // Handle keyboard input
    }
}

