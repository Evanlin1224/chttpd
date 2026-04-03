#ifndef PATH_UTILS_H
#define PATH_UTILS_H

/**
 * @brief Normalize a file path by resolving "." and ".." components.
 * 
 * @param path The input file path to normalize (e.g., "/a/b/../c/./d").
 * @return char* A newly allocated string containing the normalized path (e.g., "/a/c/d").
 *                The caller is responsible for freeing the returned string.
 */
void get_full_path(char *path, char *full_path);

#endif // PATH_UTILS_H