#!/bin/bash
# tools/linux/install-desktop.sh: add Sky Recomp to the desktop's application menu (the "start menu"), with its icon.
# Run it from the portable folder (or from an extracted AppImage). No root: everything goes to the user's
# ~/.local/share. The icon is installed into the hicolor icon theme as "skyrecomp" (an absolute Icon= path is not
# themed and some desktops show a generic icon for it), and the menu caches are refreshed.
set -euo pipefail
HERE=$(dirname "$(readlink -f "$0")")
DATA=${XDG_DATA_HOME:-$HOME/.local/share}
APPS=$DATA/applications
ICONS=$DATA/icons/hicolor
NAME=skyrecomp

[ -f "$HERE/icon.png" ] || { echo "install-desktop: no icon.png next to $0"; exit 1; }

# Inside an AppImage the script runs from a temporary mount: the menu entry must start the .AppImage itself.
if [ -n "${APPIMAGE:-}" ]; then
    EXE=$APPIMAGE
    WORKDIR=$HOME
else
    EXE=$HERE/skyrecomp
    WORKDIR=$HERE
    [ -x "$EXE" ] || { echo "install-desktop: no skyrecomp next to $0"; exit 1; }
fi

mkdir -p "$APPS" "$ICONS/256x256/apps" "$ICONS/512x512/apps"
install -m644 "$HERE/icon.png" "$ICONS/256x256/apps/$NAME.png"
install -m644 "$HERE/icon.png" "$ICONS/512x512/apps/$NAME.png"

# Exec/Path are quoted: the folder may contain spaces
cat > "$APPS/$NAME.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=Sky Recomp
Comment=The Legend of Zelda: Skyward Sword HD, recompiled (bring your own game files)
Exec="$EXE"
Path=$WORKDIR
Icon=$NAME
StartupWMClass=skyrecomp
Categories=Game;
Terminal=false
EOF
chmod 644 "$APPS/$NAME.desktop"

command -v update-desktop-database >/dev/null && update-desktop-database "$APPS" >/dev/null 2>&1 || true
command -v gtk-update-icon-cache >/dev/null && gtk-update-icon-cache -f -t "$ICONS" >/dev/null 2>&1 || true

echo "Installed $APPS/$NAME.desktop"
echo "Icon: hicolor 256x256/512x512 ($NAME.png)"
echo "It appears in the application menu as \"Sky Recomp\" (log out and in if the desktop still shows the old icon)."
