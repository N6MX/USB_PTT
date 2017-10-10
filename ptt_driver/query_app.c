#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <dirent.h>

#define RTS_ON(fd)  { int stuff; ioctl (fd, TIOCMGET, &stuff); stuff |= TIOCM_RTS;  ioctl (fd, TIOCMSET, &stuff); }
#define RTS_OFF(fd) { int stuff; ioctl (fd, TIOCMGET, &stuff); stuff &= ~TIOCM_RTS; ioctl (fd, TIOCMSET, &stuff); }
#define DTR_ON(fd)  { int stuff; ioctl (fd, TIOCMGET, &stuff); stuff |= TIOCM_DTR;  ioctl (fd, TIOCMSET, &stuff); }
#define DTR_OFF(fd) { int stuff; ioctl (fd, TIOCMGET, &stuff); stuff &= ~TIOCM_DTR; ioctl (fd, TIOCMSET, &stuff); }
 
int main(int argc, char *argv[]) {
  char file_name[80] = "/dev/";
  FILE *fp;
  int fd;

  //Get list of pttX devices
  DIR           *d;
  struct dirent *dir;
  d = opendir("/dev/");
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
      if(strstr(dir->d_name,"ptt") != NULL)
      {
        strcat(file_name,dir->d_name);
        break;
      }
    }
    closedir(d);
  }

  fp = fopen(file_name, "w+");
  if (fp == NULL) {
    perror("query_apps open");
    return 2;
  }

  fd = fileno(fp); //Get file descriptor

  if (argc == 2) {
    if      (strcmp(argv[1], "-rc") == 0) {RTS_OFF(fd);}
    else if (strcmp(argv[1], "-rs") == 0) {RTS_ON(fd);}
    else if (strcmp(argv[1], "-dc") == 0) {DTR_OFF(fd);}
    else if (strcmp(argv[1], "-ds") == 0) {DTR_ON(fd);}
    else {
      fprintf(stderr, "Usage: %s [-rc | -rs | -dc | -ds]\n", argv[0]);
      return 1;
    }
  }
  else {
    fprintf(stderr, "Usage: %s [-rc | -rs | -dc | -ds]\n", argv[0]);
    return 1;
  }

  fclose(fp);
  return 0;
}
