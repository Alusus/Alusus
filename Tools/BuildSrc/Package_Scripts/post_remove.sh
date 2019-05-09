# Remove alusus symbolic links.

LINK_DIR=/usr/local/bin

if [ -L "${LINK_DIR}/alusus" ]; then
   unlink ${LINK_DIR}/alusus
fi

if [ -L "${LINK_DIR}/الأسس" ]; then
   unlink ${LINK_DIR}/الأسس
fi

if [ -L "${LINK_DIR}/apm" ]; then
   unlink ${LINK_DIR}/apm
fi

if [ -L "${LINK_DIR}/محا" ]; then
   unlink ${LINK_DIR}/محا
fi

# Remove GTK syntax highlighting specs.

if [ -e "/usr/share/gnome/gtksourceview-3.0/language-specs/alusus.lang" ]; then
  rm "/usr/share/gnome/gtksourceview-3.0/language-specs/alusus.lang"
elif [ -e "/usr/local/share/gtksourceview-3.0/language-specs/alusus.lang" ]; then
  rm "/usr/local/share/gtksourceview-3.0/language-specs/alusus.lang"
elif [ -e "/usr/share/gtksourceview-3.0/language-specs/alusus.lang" ]; then
  rm "/usr/share/gtksourceview-3.0/language-specs/alusus.lang"
fi
