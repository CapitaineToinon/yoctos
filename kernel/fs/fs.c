#include "fs.h"

void fs_init() {

}

int file_stat(const char *filename, stat_t *stat) {
    return 0;
}

int file_open(const char *filename) {
    return -1;
}

int file_read(int fd, void *buf, uint_t count) {
    return 0;
}

int file_seek(int fd, uint_t offset) {
    return 0;
}

void file_close(int fd) {

}

file_iterator_t file_iterator() {
    file_iterator_t f;
    return f;
}

bool file_has_next(file_iterator_t *it) {
    return false;
}

void file_next(const char *filename, file_iterator_t *it) {

}
