#include "ulibc.h"

static term_colors_t color_ori = (term_colors_t){LIGHT_GREY,BLACK};

static void cat(const char *filename) {
	int fd = file_open(filename);
	if (fd == -1) {
		printf("cat: failed reading \"%s\"\n", filename);
		return;
	}
	int sz = 37;
	char buf[sz+1];
	int n;
	do {
		n = file_read(fd, buf, sz);
		buf[n] = 0;
		puts(buf);
		
	} while (n);
	file_close(fd);
}

static void list_files() {
	file_iterator_t it = file_iterator();

	while (file_has_next(&it)) {
		char name[256];
		file_next(name, &it);
		stat_t stat;
		file_stat(name, &stat);
		printf("%s (inode %d, %d bytes)\n", name, stat.inode, stat.size);
	}
}

static void seek_read_test() {
    const char *filename = "big";
	int fd = file_open(filename);
	if (fd == -1) {
		printf("Failed reading \"%s\"\n", filename);
		return;
	}
    file_seek(fd, 877193);
    int count = 512;
    uint8_t buf[count];
	file_read(fd, buf, count);
	file_close(fd);

    for (int i = 0; i < count; i++) {
        printf("%d ", buf[i]);
    }
    printf("\n");
}

static void simult_reads_test() {
    const int FD_COUNT = 3;
    int fd[FD_COUNT];
    const char *filename = "plan9.txt";

    for (int i = 0; i < FD_COUNT; i++) {
        fd[i] = file_open(filename);
        if (fd[i] == -1) {
            printf("Failed reading \"%s\"\n", filename);
            return;
        }
    }

    uint8_t buf[128];
    // 1: The single most important feature of Plan 9
    // 2: Plan 9 was integrating graphics and ubiquitous networking
    // 3: Why didn't it take over the world?
    int offsets[] = { 1191, 297, 2711 };
    int counts[] = { 43, 57, 34 };

    for (int i = 0; i < FD_COUNT; i++) {
        file_seek(fd[i], offsets[i]);
        file_read(fd[i], buf, counts[i]);
        buf[counts[i]] = 0;
        printf("%c%s%c\n", 179, buf, 179);
    }

    for (int i = 0; i < FD_COUNT; i++) {
        file_close(fd[i]);
    }
}

static void invalid_seek_test() {
    const char *filename = "big";
	int fd = file_open(filename);
	if (fd == -1) {
		printf("Failed reading \"%s\"\n", filename);
		return;
	}
    int pos = file_seek(fd, 1024*1024*50);
	file_close(fd);

    printf("Position = %d (should be %d)\n", pos, -1);
}

// splash = 360 bytes
static void invalid_read_test() {
    const char *filename = "splash";  // MUST use splash
	int fd = file_open(filename);
	if (fd == -1) {
		printf("Failed reading \"%s\"\n", filename);
		return;
	}
    uint8_t buf[1000];
    buf[360] = 137;
	int bytes_read = file_read(fd, buf, 1000);
	file_close(fd);

    printf("Bytes read = %d (should be %d)\n", bytes_read, 360);
    printf("Guard value = %d (should be %d)\n", buf[360], 137);
}

static void wait_key() {
    set_colors((term_colors_t){RGB(255,100,100),BLACK});
    printf("\nPress a key to continue...\n\n");
    set_colors(color_ori);
    while (!getc());
}

static void print_section(const char *msg) {
    set_colors((term_colors_t){YELLOW,RGB(80,50,80)});
    printf("%s", msg);
    set_colors(color_ori);
    printf("\n");
}

void main() {
    print_section("Welcome to Filesystem Tester!");
    wait_key();

    print_section("[1. File iterator test]");
    list_files();
    wait_key();

    print_section("[2. Basic read test1]");
    cat("splash");
    wait_key();

    print_section("[3. Basic read test2]");
    cat("plan9.txt");
    wait_key();

    print_section("[4. Seek and read test]");
    seek_read_test();
    wait_key();

    print_section("[5. Simultaneous reads test]");
    simult_reads_test();
    wait_key();

    print_section("[6. Invalid seek test]");
    invalid_seek_test();
    wait_key();

    print_section("[7. Invalid read test]");
    invalid_read_test();

    printf("\n");
}
