# niri Config

My [niri](https://github.com/YaLTeR/niri) Wayland compositor configuration.

## Scripts

### `scripts/color-scheme.c`

Toggles the GNOME color-scheme between `prefer-dark` and `prefer-light` via
`dconf`. Bound to `MOD+SHIFT+C` in `config.kdl`.

Run directly with `tcc -run` (Tiny C Compiler):

```
tcc -run ~/.config/niri/scripts/color-scheme.c
```

## Debugging

When something doesn't work, start by isolating the problem:

1. **Check dependencies** – Are the required tools installed?
   ```
   which dconf notify-send tcc
   ```
2. **Read the current value** – Verify `dconf` is working:
   ```
   dconf read /org/gnome/desktop/interface/color-scheme
   ```
3. **Check the script syntax** – Run `tcc` with verbose errors:
   ```
   tcc -run ~/.config/niri/scripts/color-scheme.c
   ```
   Or compile with GCC for more warnings:
   ```
   gcc -Wall -Wextra -o /tmp/test ~/.config/niri/scripts/color-scheme.c && /tmp/test
   ```
4. **Insert debug prints** – Add `fprintf(stderr, "debug: ...\n");` to trace
   execution flow and variable values.
5. **Test commands manually** – Run the `dconf` or `notify-send` commands the
   script would execute to see their exact output/errors.
6. **Check exit codes** – If the script returns early (`return 1`), follow up
   with `echo $?` to confirm.
