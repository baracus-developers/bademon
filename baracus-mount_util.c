#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/mount.h>
#undef dev_t
#define dev_t __kernel_dev_t
#include <linux/loop.h>
#undef dev_t

#include "baracus-systemd.h"

int baracus_mount(const char *source, const char *target,
                 const char *filesystemtype, unsigned long mountflags,
                 const void *data) {

        int rc;
        rc = mount(source, target, filesystemtype, mountflags, data);
        return rc;
}


int baracus_umount(const char *target) {

        int rc;
        rc = umount(target);
        return rc;
}


void util_umount_all_devices ()
    {
    FILE *fd;
    char buffer [1000];
    char dir [1000];
    char *dirs [1000];
    int  nr_dirs = 0;
    int  i, j;

    fd = fopen ("/proc/mounts", "r");
    if (fd)
        {
        while (fgets (buffer, sizeof (buffer), fd))
            if (!strncmp (buffer, "/dev/", 5))
                {
                i = j = 0;
                while (buffer [i] != ' ') i++;
                i++;
                while (buffer [i] != ' ') dir [j++] = buffer [i++];
                dir [j] = 0;
                dirs [nr_dirs++] = strdup (dir);
                }

        fclose (fd);

        if (!nr_dirs) return;

        fprintf (stderr, "Trying to unmount %d directories:\n", --nr_dirs);

        /* we need to unmount in reverse order */

        do
            {
            if (dirs [nr_dirs])
                {
                if (dirs [nr_dirs][1])
		    {
		    fprintf (stderr, "Unmounting %s...\n", dirs [nr_dirs]);
		    umount (dirs [nr_dirs]);
		    }
		free (dirs [nr_dirs]);
                }
            }
        while (nr_dirs--);
        }
    }


