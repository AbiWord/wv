#include <gnome.h>
#include <gtk/gtk.h>

gint delete_event( GtkWidget *widget, GdkEvent  *event, gpointer   data );
void destroy( GtkWidget *widget, gpointer   data );
static gint button_press (GtkWidget *widget, GdkEvent *event);

int main(int argc,char **argv)
	{
	GtkWidget *window;
	GtkWidget *menu_bar,*menu,*menu_items,*root_menu,*vbox,*ourcanvas;
	
	char buf[128];
	int i;

#ifndef GNOME
	gtk_init(&argc,&argv);
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
#else
	gnome_init("gwordview", "0.0.1", argc,argv);
	window = gnome_app_new("gwordview", "Gnome MSWord Viewer");
#endif
	

	gtk_signal_connect (GTK_OBJECT (window), "delete_event", GTK_SIGNAL_FUNC (delete_event), NULL);
	gtk_signal_connect (GTK_OBJECT (window), "destroy", GTK_SIGNAL_FUNC (destroy), NULL);

	menu = gtk_menu_new ();

	for (i = 0; i < 3; i++)
		{
		/* Copy the names to the buf. */
		sprintf (buf, "Test-undermenu - %d", i);

		/* Create a new menu-item with a name... */
		menu_items = gtk_menu_item_new_with_label (buf);

		/* ...and add it to the menu. */
		gtk_menu_append (GTK_MENU (menu), menu_items);

		/* Do something interesting when the menuitem is selected */
#if 0
		gtk_signal_connect_object (GTK_OBJECT (menu_items), "activate",
			GTK_SIGNAL_FUNC (menuitem_response), (gpointer) g_strdup (buf));
#endif

		/* Show the widget */
		gtk_widget_show (menu_items);
		}
	
	root_menu = gtk_menu_item_new_with_label ("File");
	gtk_widget_show (root_menu);

	gtk_menu_item_set_submenu (GTK_MENU_ITEM (root_menu), menu);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (window), vbox);
	gtk_widget_show (vbox);

	menu_bar = gtk_menu_bar_new();
	gtk_box_pack_start (GTK_BOX (vbox), menu_bar, FALSE, FALSE, 2);
	gtk_widget_show (menu_bar);

#if 0
	ourcanvas = gtk_text_new(NULL,NULL);
	gtk_box_pack_start (GTK_BOX (vbox), ourcanvas, FALSE, FALSE, 2);
	gtk_widget_show (ourcanvas);
#endif

	gtk_widget_set_usize (ourcanvas,  675,900);

	/* And finally we append the menu-item to the menu-bar -- this is the
	* "root" menu-item I have been raving about =) */
	gtk_menu_bar_append (GTK_MENU_BAR (menu_bar), root_menu);
	gtk_widget_show (window);
	gtk_main ();
	return(0);
	}

gint delete_event( GtkWidget *widget, GdkEvent  *event, gpointer   data )
	{
	return(FALSE);
	}

/* Another callback */
void destroy( GtkWidget *widget, gpointer   data )
	{
	gtk_main_quit();
	}


static gint button_press (GtkWidget *widget, GdkEvent *event)
	{
	GdkEventButton *bevent;

	if (event->type == GDK_BUTTON_PRESS) 
		{
		bevent = (GdkEventButton *) event; 
		gtk_menu_popup (GTK_MENU (widget), NULL, NULL, NULL, NULL,
						bevent->button, bevent->time);
		/* Tell calling code that we have handled this event; the buck
		* stops here. */
		return TRUE;
		}

	/* Tell calling code that we have not handled this event; pass it on. */
	return FALSE;
	}
