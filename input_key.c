#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>

#define myerror(str) do { \
        perror(str); \
        return -1; \
    } while(0)

int UinputKeyInit(void)
{
  int                    fd;
  struct uinput_user_dev uidev;

  fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  if (fd < 0)
    myerror("error: open");

  if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
    myerror("error: ioctl");
  if (ioctl(fd, UI_SET_KEYBIT, KEY_ENTER) < 0)
    myerror("error: ioctl");
  if (ioctl(fd, UI_SET_KEYBIT, KEY_LEFT) < 0)
    myerror("error: ioctl");
  if (ioctl(fd, UI_SET_KEYBIT, KEY_RIGHT) < 0)
    myerror("error: ioctl");
  if (ioctl(fd, UI_SET_KEYBIT, KEY_DOWN) < 0)
    myerror("error: ioctl");
  if (ioctl(fd, UI_SET_KEYBIT, KEY_UP) < 0)
    myerror("error: ioctl");
  if (ioctl(fd, UI_SET_KEYBIT, KEY_ESC) < 0)
    myerror("error: ioctl");
  if (ioctl(fd, UI_SET_KEYBIT, KEY_BACKSPACE) < 0)
    myerror("error: ioctl");
  if (ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) < 0)
    myerror("error: ioctl");

  if (ioctl(fd, UI_SET_EVBIT, EV_REL) < 0)
    myerror("error: ioctl");
  if (ioctl(fd, UI_SET_RELBIT, REL_X) < 0)
    myerror("error: ioctl");
  if (ioctl(fd, UI_SET_RELBIT, REL_Y) < 0)
    myerror("error: ioctl");

  memset(&uidev, 0, sizeof(uidev));
  snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
  uidev.id.bustype = BUS_USB;
  uidev.id.vendor  = 0x1;
  uidev.id.product = 0x1;
  uidev.id.version = 1;

  if (write(fd, &uidev, sizeof(uidev)) < 0)
    myerror("error: write");

  if (ioctl(fd, UI_DEV_CREATE) < 0)
    myerror("error: ioctl");

  return fd;

}


static void termUinputKey(int fd)
{
  if (ioctl(fd, UI_DEV_DESTROY) < 0)
    perror("error: ioctl");
  close(fd);
}

int UinputKeyWriteKey(int fd, __u16 code) {
  struct input_event     ev;

  // KEY DOWN
  memset(&ev, 0, sizeof(struct input_event));
  ev.type = EV_KEY;
  ev.code = code;
  ev.value = 1;
  if (write(fd, &ev, sizeof(struct input_event)) < 0)
    myerror("error: write");
  memset(&ev, 0, sizeof(struct input_event));
  ev.type = EV_SYN;
  ev.code = SYN_REPORT;
  ev.value = 0;
  if (write(fd, &ev, sizeof(struct input_event)) < 0)
    myerror("error: write");

  // KEY UP
  memset(&ev, 0, sizeof(struct input_event));
  ev.type = EV_KEY;
  ev.code = code;
  ev.value = 0;
  if (write(fd, &ev, sizeof(struct input_event)) < 0)
    myerror("error: write");
  memset(&ev, 0, sizeof(struct input_event));
  ev.type = EV_SYN;
  ev.code = SYN_REPORT;
  ev.value = 0;
  if (write(fd, &ev, sizeof(struct input_event)) < 0)
    myerror("error: write");

  return 0;

}

static void usage(void)
{
  printf("usage\n");
  printf("left  : l\n");
  printf("right : r\n");
  printf("down  : d\n");
  printf("up    : u\n");
  printf("enter : o\n");
  printf("ESC   : e\n");
  printf("back  : b\n");
  printf("quit  : q\n");
}

int main(int argc, char**argv)
{
  int fd;
  char line[10] = {0};
  (void)argc;
  (void)argv;

  usage();

  if ((fd = UinputKeyInit()) < 0)
    exit(-1);

  while (1) {
    /*       printf("-> write key\n");
           UinputKeyWriteKey(fd, KEY_U);
           UinputKeyWriteKey(fd, KEY_LEFT);
           sleep(1);
           UinputKeyWriteKey(fd, KEY_RIGHT);

           sleep(1);*/

    printf( "Enter a value :");
    fgets(line, sizeof(line), stdin);


    switch (line[0]) {
    case 'r': // r
      UinputKeyWriteKey(fd, KEY_RIGHT);
      break;
    case 'l': // l
      UinputKeyWriteKey(fd, KEY_LEFT);
      break;
    case 'u': // u
      UinputKeyWriteKey(fd, KEY_UP);
      break;
    case 'd': // d
      UinputKeyWriteKey(fd, KEY_DOWN);
      break;
    case 'o': // o (OK)
      UinputKeyWriteKey(fd, KEY_ENTER);
      break;
    case 'e': // e (ESC)
      UinputKeyWriteKey(fd, KEY_ESC);
      break;
    case 'b': // b (BACK)
      UinputKeyWriteKey(fd, KEY_BACKSPACE);
      break;
    case 'q': // quit
      termUinputKey(fd);
      exit(0);
      break;
    default:
      printf("unsuported key %c\n", line[0]);
      usage();
      continue;
    }
    printf( "\nYou entered: %c\n", line[0]);
  }
  return 0;
}
