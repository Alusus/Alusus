#!/bin/sh

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

# Remove GTK syntax highlighting specs from gtksourceview 3 folder.

if [ -e "/usr/share/gnome/gtksourceview-3.0/language-specs/alusus.lang" ]; then
  rm "/usr/share/gnome/gtksourceview-3.0/language-specs/alusus.lang"
elif [ -e "/usr/local/share/gtksourceview-3.0/language-specs/alusus.lang" ]; then
  rm "/usr/local/share/gtksourceview-3.0/language-specs/alusus.lang"
elif [ -e "/usr/share/gtksourceview-3.0/language-specs/alusus.lang" ]; then
  rm "/usr/share/gtksourceview-3.0/language-specs/alusus.lang"
fi

# Remove GTK styles from gtksourceview 3 folder.

if [ -e "/usr/share/gnome/gtksourceview-3.0/styles/alusus_light_style.xml" ]; then
  rm "/usr/share/gnome/gtksourceview-3.0/styles/alusus_light_style.xml"
elif [ -e "/usr/local/share/gtksourceview-3.0/styles/alusus_light_style.xml" ]; then
  rm "/usr/local/share/gtksourceview-3.0/styles/alusus_light_style.xml"
elif [ -e "/usr/share/gtksourceview-3.0/styles/alusus_light_style.xml" ]; then
  rm "/usr/share/gtksourceview-3.0/styles/alusus_light_style.xml"
fi

if [ -e "/usr/share/gnome/gtksourceview-3.0/styles/alusus_dark_style.xml" ]; then
  rm "/usr/share/gnome/gtksourceview-3.0/styles/alusus_dark_style.xml"
elif [ -e "/usr/local/share/gtksourceview-3.0/styles/alusus_dark_style.xml" ]; then
  rm "/usr/local/share/gtksourceview-3.0/styles/alusus_dark_style.xml"
elif [ -e "/usr/share/gtksourceview-3.0/styles/alusus_dark_style.xml" ]; then
  rm "/usr/share/gtksourceview-3.0/styles/alusus_dark_style.xml"
fi

# Remove GTK syntax highlighting specs from gtksourceview 4 folder.

if [ -e "/usr/share/gnome/gtksourceview-4/language-specs/alusus.lang" ]; then
  rm "/usr/share/gnome/gtksourceview-4/language-specs/alusus.lang"
elif [ -e "/usr/local/share/gtksourceview-4/language-specs/alusus.lang" ]; then
  rm "/usr/local/share/gtksourceview-4/language-specs/alusus.lang"
elif [ -e "/usr/share/gtksourceview-4/language-specs/alusus.lang" ]; then
  rm "/usr/share/gtksourceview-4/language-specs/alusus.lang"
fi

# Remove GTK styles from gtksourceview 4 folder.

if [ -e "/usr/share/gnome/gtksourceview-4/styles/alusus_light_style.xml" ]; then
  rm "/usr/share/gnome/gtksourceview-4/styles/alusus_light_style.xml"
elif [ -e "/usr/local/share/gtksourceview-4/styles/alusus_light_style.xml" ]; then
  rm "/usr/local/share/gtksourceview-4/styles/alusus_light_style.xml"
elif [ -e "/usr/share/gtksourceview-4/styles/alusus_light_style.xml" ]; then
  rm "/usr/share/gtksourceview-4/styles/alusus_light_style.xml"
fi

if [ -e "/usr/share/gnome/gtksourceview-4/styles/alusus_dark_style.xml" ]; then
  rm "/usr/share/gnome/gtksourceview-4/styles/alusus_dark_style.xml"
elif [ -e "/usr/local/share/gtksourceview-4/styles/alusus_dark_style.xml" ]; then
  rm "/usr/local/share/gtksourceview-4/styles/alusus_dark_style.xml"
elif [ -e "/usr/share/gtksourceview-4/styles/alusus_dark_style.xml" ]; then
  rm "/usr/share/gtksourceview-4/styles/alusus_dark_style.xml"
fi
