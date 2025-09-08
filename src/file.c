#include <appimage/appimage.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <math.h>

char * get_path(char * path) {
    char * home = getenv("HOME");

    if (!home) {
        puts("Can't find home environment variable.");
        return NULL;
    }
    else if (strlen(home) + strlen("/.local/bin/") >= PATH_MAX) {
        puts("Path to app folder exceeds maximum path capacity.");
        return NULL;
    }

    char * location = malloc(PATH_MAX);
    strcpy(location, home);
    strcat(location, "/.local/bin/");

    if (access(location, F_OK) == -1)
        if(mkdir(location, 0700) == -1) {
            printf("Could not create directory %s", location);
            free(location);
            return NULL;
        }

    if (path) {
        char * name = (strrchr(path, '/')) ? strrchr(path, '/') + 1: path;

        if (strlen(location) + strlen(name) >= PATH_MAX) {
            puts("Path to app exceeds maximum path capacity.");
            free(location);
            return NULL;
        }
        else {
            strcat(location, name);
        }
    }

    return location;
}

int nep_install(char * appimage) {
    char * path = realpath(appimage, NULL);
    if (!path) {
        puts("File does not exist");
        return 1;
    }
    else if (appimage_get_type(path, true) == -1) {
        printf("File %s is not an AppImage.\n", appimage);
        free(path);
        return 1;
    }

    char * newpath = get_path(path);
    if (appimage_is_registered_in_system(newpath) == true)
        printf("AppImage %s is already installed.\n", appimage);
    else if (rename(path, newpath) == -1)
        printf("Failed to move AppImage %s to %s\n", path, newpath);
    else if(appimage_register_in_system(newpath, true))
        puts("Something went wrong while installing AppImage.");
    
    free(path);
    free(newpath);
    return 0;
}

int nep_remove(char * appimage) {
    char * path = get_path(appimage);
    
    if (appimage_is_registered_in_system(path) == false)
        printf("AppImage %s is not installed.\n", appimage);
    else if(appimage_unregister_in_system(path, true))
        puts("Something went wrong while removing AppImage.");
    else if(remove(path))
        puts("Could not remove AppImage");
    else {
        free(path);
        return 0;
    }

    free(path);
    return 1;
}

void human_readable_print(char * name, off_t filesize) {
    if (!filesize) {
        printf("0B\t%s\n", name);
        return;
    }

    char * fmt[] = {"%.1lf%cB\t%s\n", "%.0lf%cB\t%s\n", ">999QB\t%s\n"};
    char suffix[] = {0, 'K', 'M', 'B', 'T', 'Q'};

    // All 32 bit integers can be represented in a double, 
    // but also accuracy doesn't matter as only measuring digits 
    double value = (double)filesize;
    int digits = (int)ceil(log10(value));
    int choice = (digits - 1) / 3;
    
    if (choice >= sizeof(suffix))
        printf(fmt[2], name);
    value /= pow(1000.0, choice);

    if (choice == 1)
        printf(fmt[0], value, suffix[choice], name);
    else
        printf(fmt[1], value, suffix[choice], name);

    return;
}

int nep_list() {

    char * location = get_path(NULL);
    DIR * apps = opendir(location);
    free(location);
    int dir_fd = dirfd(apps);

    if (!apps) {
        puts("Could not open ~/.local/bin");
        return 1;
    }

    struct dirent * app;
    struct stat buf;

    while ((app = readdir(apps)) != NULL) {
        if (app->d_name[0] == '.')
            continue;
        if (fstatat(dir_fd, app->d_name, &buf, 0) == -1) {
            printf("Cannot open %s\n", app->d_name);
            continue;
        }
        human_readable_print(app->d_name, buf.st_size);
    }

    return 0;
}