#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../util.h"
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>


const char *
custom_volume(void) {
	const char *homedir;
	char buf[5];
	FILE *volume_fp;
	FILE *muted_fp;

	/* FOR DEBUGGiNG HOME DiRECTORY FUNCTiONALiTY */
	/* if ((homedir = getenv("HOME")) == NULL) { */
	/* 	homedir = getpwuid(getuid())->pw_dir; */
	/* } */
	/* if (chdir(homedir) != 0) */
	/* 	perror("Error changing working directory"); */
	/* char cwd[256]; */
	/* getcwd(cwd, 256); */
	/* printf("%s\n", cwd); */

	static const char cmd0[] = "pacmd list-sinks | \
				    awk '/muted/ { print $2 }' > .muted";
	static const char cmd1[] = "pactl list sinks | \
				    grep '^[[:space:]]Volume:' | \
				    head -n $(($SINK +1)) | \
				    tail -n 1 | \
				    sed -e \'s,.* \\([0-9][0-9]*\\)%.*,\\1%,\ ' > .volume";

	system(cmd0); system(cmd1);
	volume_fp = fopen(".volume", "r");
	muted_fp = fopen(".muted", "r");
	if (volume_fp == NULL)
		perror ("Error opening volume file");
	if (muted_fp == NULL)
		perror("Error opening mute file");
	else {
		if ( fgets(buf, 4, muted_fp) != NULL && !strcmp(buf, "yes")){
			fclose(muted_fp); fclose(volume_fp);
			return bprintf("muted");
		}
		else if ( fgets (buf, 4, volume_fp) != NULL ){
			fclose(muted_fp); fclose(volume_fp);
			if (atoi(buf) >= 100)
				strcpy(buf, "100%");
			return bprintf("%s", buf);
		}
	}
	return "n/a";
}
