#!/bin/sh

ALUSUS_ROOT=/opt/Alusus

# Copy symbolic link to PATH reachable dir.

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

ln -sf ${ALUSUS_ROOT}/Bin/alusus ${LINK_DIR}/alusus
ln -sf ${ALUSUS_ROOT}/Bin/alusus ${LINK_DIR}/الأسس
ln -sf ${ALUSUS_ROOT}/Bin/apm ${LINK_DIR}/apm
ln -sf ${ALUSUS_ROOT}/Bin/محا ${LINK_DIR}/محا


# Copy GTK syntax highlighting specs to gtksourceview 3 folder.

IS_GTKSOURCEVIEW_EXISTS=1
if [ -e "/usr/share/gnome/gtksourceview-3.0/language-specs" ]; then
  LANG_SPEC_DIR="/usr/share/gnome/gtksourceview-3.0/language-specs"
elif [ -e "/usr/local/share/gtksourceview-3.0/language-specs" ]; then
  LANG_SPEC_DIR="/usr/local/share/gtksourceview-3.0/language-specs"
elif [ -e "/usr/share/gtksourceview-3.0/language-specs" ]; then
  LANG_SPEC_DIR="/usr/share/gtksourceview-3.0/language-specs"
else
  IS_GTKSOURCEVIEW_EXISTS=0
fi

if [ "$IS_GTKSOURCEVIEW_EXISTS" -eq 1 ]; then
  cp -f ${ALUSUS_ROOT}/Tools/Gtk_Syntax_Highlighting/alusus.lang ${LANG_SPEC_DIR}/
fi

# Copy GTK styles to gtksourceview 3 folder.

IS_GTKSOURCEVIEW_EXISTS=1
if [ -e "/usr/share/gnome/gtksourceview-3.0/styles" ]; then
  LANG_SPEC_DIR="/usr/share/gnome/gtksourceview-3.0/styles"
elif [ -e "/usr/local/share/gtksourceview-3.0/styles" ]; then
  LANG_SPEC_DIR="/usr/local/share/gtksourceview-3.0/styles"
elif [ -e "/usr/share/gtksourceview-3.0/styles" ]; then
  LANG_SPEC_DIR="/usr/share/gtksourceview-3.0/styles"
else
  IS_GTKSOURCEVIEW_EXISTS=0
fi

if [ "$IS_GTKSOURCEVIEW_EXISTS" -eq 1 ]; then
  cp -f ${ALUSUS_ROOT}/Tools/Gtk_Syntax_Highlighting/alusus_light_style.xml ${LANG_SPEC_DIR}/
  cp -f ${ALUSUS_ROOT}/Tools/Gtk_Syntax_Highlighting/alusus_dark_style.xml ${LANG_SPEC_DIR}/
fi

# Copy GTK syntax highlighting specs to gtksourceview 4 folder.

IS_GTKSOURCEVIEW_EXISTS=1
if [ -e "/usr/share/gnome/gtksourceview-4/language-specs" ]; then
  LANG_SPEC_DIR="/usr/share/gnome/gtksourceview-4/language-specs"
elif [ -e "/usr/local/share/gtksourceview-4/language-specs" ]; then
  LANG_SPEC_DIR="/usr/local/share/gtksourceview-4/language-specs"
elif [ -e "/usr/share/gtksourceview-4/language-specs" ]; then
  LANG_SPEC_DIR="/usr/share/gtksourceview-4/language-specs"
else
  IS_GTKSOURCEVIEW_EXISTS=0
fi

if [ "$IS_GTKSOURCEVIEW_EXISTS" -eq 1 ]; then
  cp -f ${ALUSUS_ROOT}/Tools/Gtk_Syntax_Highlighting/alusus.lang ${LANG_SPEC_DIR}/
fi

# Copy GTK styles to gtksourceview 4 folder.

IS_GTKSOURCEVIEW_EXISTS=1
if [ -e "/usr/share/gnome/gtksourceview-4/styles" ]; then
  LANG_SPEC_DIR="/usr/share/gnome/gtksourceview-4/styles"
elif [ -e "/usr/local/share/gtksourceview-4/styles" ]; then
  LANG_SPEC_DIR="/usr/local/share/gtksourceview-4/styles"
elif [ -e "/usr/share/gtksourceview-4/styles" ]; then
  LANG_SPEC_DIR="/usr/share/gtksourceview-4/styles"
else
  IS_GTKSOURCEVIEW_EXISTS=0
fi

if [ "$IS_GTKSOURCEVIEW_EXISTS" -eq 1 ]; then
  cp -f ${ALUSUS_ROOT}/Tools/Gtk_Syntax_Highlighting/alusus_light_style.xml ${LANG_SPEC_DIR}/
  cp -f ${ALUSUS_ROOT}/Tools/Gtk_Syntax_Highlighting/alusus_dark_style.xml ${LANG_SPEC_DIR}/
fi
