#!/bin/sh

gcc -DGNOME -I/usr/lib/gnome-libs/include tester.c -I/usr/X11R6/include -I/usr/lib/glib/include -rdynamic -L/usr/lib -L/usr/X11R6/lib -rdynamic -lgnomeui -lart_lgpl -lgdk_imlib -lSM -lICE -lgtk -lgdk -lgmodule -lXext -lX11 -lgnome -lgnomesupport -lesd -laudiofile -lm -ldb -lglib -ldl -L/usr/lib -L/usr/X11R6/lib -lgtk -lgdk -rdynamic -lgmodule -lglib -ldl -lXext -lX11 -lm -L/usr/local/lib -lwv -lm
