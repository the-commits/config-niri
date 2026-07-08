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

static long run_sh(const char *cmd) {
  long pid = syscall(57);
  if (pid == 0) {
    syscall(59, "/bin/sh",
            (char *[]){(char *)"sh", (char *)"-c", (char *)cmd, 0}, environ);
    syscall(60, 127);
  }
  while (syscall(61, pid, 0, 0, 0) > 0)
    ;
  return pid;
}

void _start(void) {
  char buf[256];
  int p[2];

  syscall(22, p, 0);
  long pid = syscall(57);
  if (pid == 0) {
    syscall(3, p[0]);
    syscall(33, p[1], 1);
    syscall(3, p[1]);
    syscall(59, "/bin/sh",
            (char *[]){(char *)"sh", (char *)"-c",
                       (char *)"dconf read /org/gnome/desktop/interface/color-scheme",
                       0},
            environ);
    syscall(60, 127);
  }
  syscall(3, p[1]);
  int n = syscall(0, p[0], buf, 255);
  syscall(3, p[0]);
  while (syscall(61, pid, 0, 0, 0) > 0)
    ;

  const char *val;
  const char *text;
  if (n <= 0 || !sstr(buf, "prefer-dark")) {
    val = "prefer-dark";
    text = "Mörkt läge";
  } else {
    val = "prefer-light";
    text = "Ljust läge";
  }

  char cmd[256];
  char *w = cmd;
  w = scpyn(w, S("dconf write /org/gnome/desktop/interface/color-scheme \"'"));
  w = scpy(w, val);
  w = scpyn(w, "'\"", 2);
  *w = 0;
  run_sh(cmd);

  w = cmd;
  w = scpyn(w, S("notify-send -t 5000 System 'Ändrat till "));
  w = scpy(w, text);
  *w++ = '\'';
  *w = 0;
  run_sh(cmd);

  syscall(60, 0);
}
