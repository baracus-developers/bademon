#include <stdio.h>
#include "baracus-systemd.h"


int main(int argc, char * argv) 
{
	char * source;
	char * target;
	char * fstype;
	unsigned long mountflags;
        const void *data;
	int rc;

	source = "/dev/sdd1";
	target = "/media/nfsroot/root2";
	fstype = "ext4";
	mountflags = 0;
	data = NULL;

	rc = baracus_mount(source, target, fstype, mountflags, data);
	printf ("mount returns %d\n", rc);

	rc = baracus_umount(target);
	printf ("umount returns %d\n", rc);

	return 0;
}


