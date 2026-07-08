long syscall(long number, ...);
extern char **environ;

static int slen(const char *s) {
  int n = 0;
  while (*s++) n++;
  return n;
}

static char *scpy(char *d, const char *s) {
  while (*s) *d++ = *s++;
  return d;
}

static char *scpyn(char *d, const char *s, int n) {
  while (n--) *d++ = *s++;
  return d;
}

static const char *sstr(const char *h, const char *n) {
  if (!*n) return h;
  for (; *h; h++) {
    const char *a = h, *b = n;
    while (*a && *b && *a == *b) a++, b++;
    if (!*b) return h;
  }
  return 0;
}

#define S(s) s, sizeof(s) - 1

static const char *get_env(const char *name) {
  for (char **e = environ; *e; e++) {
    const char *p = *e, *n = name;
    while (*n && *p && *p == *n) p++, n++;
    if (!*n && *p == '=') return p + 1;
  }
  return 0;
}

/* read one line from fd into buf (max n-1 chars). Returns length, 0 on EOF. */
static int read_line(int fd, char *buf, int n) {
  int i = 0;
  while (i < n - 1) {
    long r = syscall(0, fd, buf + i, 1);
    if (r <= 0) break;
    if (buf[i] == '\n') break;
    i++;
  }
  buf[i] = 0;
  return i;
}

void _start(void) {
  const char *home = get_env("HOME");
  if (!home) syscall(60, 1);

  /* Build path: $HOME/.cache/cliphist/.sensitive-ids */
  char path[256];
  char *w = path;
  w = scpy(w, home);
  w = scpy(w, "/.cache/cliphist/.sensitive-ids");
  *w = 0;

  int hlen = slen(home);
  char dir[256];
  scpyn(dir, home, hlen);
  scpy(dir + hlen, "/.cache/cliphist");
  syscall(83, dir, 0700); /* mkdir, ignore failure */

  /* touch+chmod the tag file */
  long fd = syscall(2, path, 0101, 0600); /* O_CREAT|O_WRONLY = 0x41 = 65 decimal... wait */
  /* O_CREAT=0100, O_WRONLY=1 → 0101 = 65 */
  /* Actually: O_WRONLY=1, O_CREAT=0100 → 0x41=65 */
  syscall(3, fd);
  syscall(90, path, 0600); /* chmod */

  /* Run: dbus-monitor "interface='org.freedesktop.Notifications',member='Notify'"
     | grep --line-buffered -A 5 'string "kdeconnect"'
     | grep --line-buffered -i 'copied to clipboard' 
     | while read line; do ... done */
  
  /* We run the grep pipeline and read its output */
  int p[2];
  syscall(22, p, 0);
  long pid = syscall(57);
  if (pid == 0) {
    /* Child: run the dbus-monitor pipeline, write to pipe */
    syscall(3, p[0]);
    syscall(33, p[1], 1);
    syscall(3, p[1]);
    syscall(59, "/bin/sh",
            (char *[]){(char *)"sh", (char *)"-c",
                       (char *)"exec dbus-monitor \"interface='org.freedesktop.Notifications',member='Notify'\" | grep --line-buffered -A 5 'string \"kdeconnect\"' | grep --line-buffered -i 'copied to clipboard'",
                       0},
            environ);
    syscall(60, 127);
  }
  syscall(3, p[1]);

  char buf[1024];
  while (read_line(p[0], buf, sizeof(buf)) > 0) {
    /* Sleep 0.5s (500ms = 500,000,000 ns) */
    long tv[] = {0, 500000000};
    syscall(35, tv, 0); /* nanosleep */

    /* Run: /usr/bin/cliphist list | head -n 1 | cut -f 1 >> path */
    char cmd[512];
    w = cmd;
    w = scpyn(w, S("exec /usr/bin/cliphist list | head -n 1 | cut -f 1 >> "));
    w = scpy(w, path);
    *w = 0;
    
    long cpid = syscall(57);
    if (cpid == 0) {
      syscall(59, "/bin/sh",
              (char *[]){(char *)"sh", (char *)"-c", cmd, 0},
              environ);
      syscall(60, 127);
    }
    while (syscall(61, cpid, 0, 0, 0) > 0);
  }

  syscall(3, p[0]);
  syscall(60, 0);
}
