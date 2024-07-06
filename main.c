#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curses.h"


#define MAX_LINES 1000
#define MAX_LINE_LENGTH 1024
#define FILENAME_MAX_LENGTH 256

char *lines[MAX_LINES];


void init_lines() {
    for (int i = 0; i < MAX_LINES; i++) {
        lines[i] = (char *)malloc(MAX_LINE_LENGTH);
        memset(lines[i], 0, MAX_LINE_LENGTH);
    }
}


void get_filename(char *filename) {
    echo();
    mvprintw(LINES - 1, 0, "Enter filename: ");
    getnstr(filename, FILENAME_MAX_LENGTH - 1);
    noecho();
}


void save_file(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printw("Error: Unable to open file for writting\n");
        return;
    }
    for (int i = 0; i < MAX_LINES; i++) {
        if (strlen(lines[i]) > 0) fprintf(file, "%s\n", lines[i]);
    }
    fclose(file);
    mvprintw(LINES - 1, 0, "File saved successfully. Press any key to continue...");
    getch();
    move(LINES - 1, 0);
    clrtoeol();
    refresh();
}


void load_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printw("Error: Unable to open file for reading\n");
        return;
    }
    for (int i = 0; i < MAX_LINES; i++) {
        if (fgets(lines[i], MAX_LINE_LENGTH, file) == NULL) break;
        lines[i][strcspn(lines[i], "\n")] = '\0';
    }
    fclose(file);
    clear();
    for (int i = 0; i < MAX_LINES; i++) mvprintw(i, 0, "%s", lines[i]);
    refresh();
}


int main(void){
    initscr();  // Start of curses mode
    noecho();   // Don't echo user input
    cbreak();   // Disable line buffering
    keypad(stdscr, TRUE);   // Enable special keys (Arrow keys)

    init_lines();   // Initialize lines array

    int ch;
    int x = 0, y = 0; 
    move(y, x); // Move cursor to inital position

    char filename[FILENAME_MAX_LENGTH];

    while (1) {
        ch = getch();
        if (ch == KEY_F(1)) break;  // Exit on F1 key press
        if(ch == KEY_F(2)) {        // Save on F2 key press 
            get_filename(filename);
            save_file(filename);
            continue;
        }
        if (ch == KEY_F(3)){        // Load on F3 key press
            get_filename(filename);
            load_file(filename);
            x = 0;
            y = 0;
            move(y, x);
            continue;
        }

        switch (ch) {

            // Moving cursor with arrows
            case KEY_UP:
                if (y > 0) y--;
                break;
            case KEY_DOWN:
                if (y < LINES - 1) y++;
                break;
            case KEY_LEFT:
                if (x > 0) x--;
                break;
            case KEY_RIGHT:
                if (x < COLS - 1) x++;
                break;
            
            // Backspace implementation
            case KEY_BACKSPACE:
            case 127:
            case 8:
                if (x > 0) {
                    x--;
                    mvaddch(y, x, ' '); // Clear character at current pos
                }
                else if (y > 0){
                    y--;
                    x = COLS - 1;
                    mvaddch(y, x, ' '); // Clear character at current pos
                }
                break;

            default:
                if (ch != '\n') { // Next line only with ENTER
                    mvaddch(y, x, ch);
                    x++;
                    if (x >= COLS) {
                        x = 0;
                        if (y < LINES - 1) y++;
                    }
                } else {
                    y++;
                    x = 0;
                }
                break;
        }

        move(y, x); // Move cursor to new pos
        refresh();  // Refresh screen
    }

    // Free allocated memory
    for (int i = 0; i < MAX_LINES; i++) free(lines[i]);

    endwin();    // End curses mode
    return 0;
}