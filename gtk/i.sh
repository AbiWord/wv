#!/bin/sh
gcc -c -DGNOME tester.c -I/usr/local/lib/gnome-libs/include `glib-config --cflags`
gcc tester.o -rdynamic -L/usr/local/lib -L/usr/X11R6/lib -rdynamic -lgnomeui -lart_lgpl -lgdk_imlib -lSM -lICE -lgtk -lgdk -lgmodule -lintl -lXext -lX11 -lgnome -lgnomesupport -lesd -laudiofile -lm -ldb -lglib -ldl -rdynamic -L/usr/local/lib -lgdome -lxml -lz -lart_lgpl -lgmodule -lglib -ldl -lgdk_pixbuf -lm -lwv
