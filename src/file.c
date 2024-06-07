#include <appimage/appimage.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>

char * get_local_appimage(char * name) {
    char * home = getenv("HOME");
    char * file = strrchr(name, '/') + 1;

    if (!home) {
        puts("Can't find home environment variable.");
        return NULL;
    }
    if (strlen(home) + strlen("/.local/bin") + strlen(file) >= PATH_MAX) {
        puts("The name of your AppImage folder plus the name of your AppImage exceeds the maximum allowed path limit on your system.");
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

    strcat(location, file);
    return location;
}

void sat_install(char * appimage) {
    char * path = realpath(appimage, NULL);
    if (!path) {
        puts("File does not exist");
        return;
    }
    else if (appimage_get_type(path, true) == -1) {
        printf("File %s is not an AppImage.\n", appimage);
        free(path);
        return;
    }

    char * newpath = get_local_appimage(path);
    if (appimage_is_registered_in_system(newpath) == true)
        printf("AppImage %s is already installed.\n", appimage);
    else if (rename(path, newpath) == -1)
        printf("Failed to move AppImage %s to %s\n", path, newpath);
    else if(appimage_register_in_system(newpath, true))
        puts("Something went wrong while installing AppImage.");
    
    free(path);
    free(newpath);
    return;
}

void sat_remove(char * appimage) {
    char * path = realpath(appimage, NULL);
    char * newpath = get_local_appimage(path);
    
    if (appimage_is_registered_in_system(newpath) == false)
        printf("AppImage %s is not installed.\n", appimage);
    else if(appimage_unregister_in_system(appimage, true))
        puts("Something went wrong while removing AppImage.");

    free(path);
    free(newpath);
    return;

}

void sat_list() {
    system("cd ~/.local/bin && du --block-size=MB * | sort -hr");
}