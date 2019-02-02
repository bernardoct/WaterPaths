#!/bin/sh
# Fall back to a shell if default nix-shell is exited:
d "$HOME/WaterPaths/" || sh
"$HOME/.nix-profile/bin/nix-shell" nix/shell.nix; sh
