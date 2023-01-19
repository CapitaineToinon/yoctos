#include <assert.h>
#include <sys/stat.h>
#include "file.h"
#include "fs.h"

/// Return the size of a file.
/// @param path path of the file to retrieve the size of.
/// @return -1 if an error occured.
long file_size(char *path) {
	assert(path);
	
	struct stat stats;
	if (stat(path, &stats))
		return -1;
	return stats.st_size;
}

/// Check whether a file exist or not.
/// @param path path of the file to check.
/// @param itab inode table of the file system.
/// @return true if the file exists, false otherwise.
bool file_exists(char *path, inode_table_t *itab) {
	return (fs_get_file_inode(path, itab) != NULL);
}
