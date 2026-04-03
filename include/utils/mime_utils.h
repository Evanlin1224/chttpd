#ifndef MIME_UTILS_H
#define MIME_UTILS_H

/**
 * @brief Get the MIME type string based on a file path or extension.
 * 
 * @param path The file path or name (e.g., "index.html").
 * @return const char* The corresponding MIME type (e.g., "text/html"). 
 *         Returns "application/octet-stream" if unknown.
 */
const char* get_mime_type(const char *path);

#endif // MIME_UTILS_H
