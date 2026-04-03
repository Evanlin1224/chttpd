#include "utils/path_utils.h"
#include <string.h>
#include <stdio.h>

void get_full_path(char *path, char *full_path) {
    const char *root = "public";
    
    // 基礎安全檢查：禁止路徑中包含 ".."
    // 這是防止 Directory Traversal (目錄穿越攻擊) 的最簡單方法
    if (strstr(path, "..") != NULL) {
        // 如果偵測到攻擊，直接導向 404
        snprintf(full_path, 1024, "%s/404.html", root);
        return;
    }

    // 如果是根目錄，指向 index.html
    if (strcmp(path, "/") == 0) {
        snprintf(full_path, 1024, "%s/index.html", root);
    } else {
        // 直接拼接，確保路徑以 / 開頭
        if (path[0] == '/') {
            snprintf(full_path, 1024, "%s%s", root, path);
        } else {
            snprintf(full_path, 1024, "%s/%s", root, path);
        }
    }
}
