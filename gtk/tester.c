#include <gnome.h>
#include <gtk/gtk.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

#ifndef MAXPATH
#define MAXPATH         255
#endif


char *gwordviewversion="0.0.1";

gint delete_event( GtkWidget *widget, GdkEvent  *event, gpointer   data );
void destroy( GtkWidget *widget, gpointer   data );
static gint button_press (GtkWidget *widget, GdkEvent *event);

void file_open_cb(GtkWidget *widget, gpointer cbdata);
void file_close(GtkWidget *widget, gpointer cbdata);
void file_show_info(GtkWidget *widget, gpointer cbdata);
void callback_about (GtkWidget *widget, GdkEvent *event);

static void doc_open_filesel_destroy(GtkWidget *wgt, gpointer cbdata);
static void doc_open_filesel(GtkWidget *wgt, gpointer cbdata);
gint delete (GtkWidget *widget, GtkWidget *event, gpointer cbdata);

void create_menus (GnomeApp *app);


char * ttoa(time_t curtime, int append_nl);

#include "wv.h"
FILE *input=NULL;
char *mfilename=NULL;
wvParseStruct ps;


void callback_new (GtkWidget *widget, GdkEvent *event)
	{
	}

void callback_exit (GtkWidget *widget, GdkEvent *event)
	{
	gtk_main_quit();
	}

int main(int argc,char **argv)
	{
	GtkWidget *window;
	GtkWidget *menu_bar,*menu,*menu_items,*root_menu,*vbox,*ourcanvas,*scroller;
	GtkStyle *widget_style, *temp_style;
	GdkColormap *colourmap;
	GdkColor colour;
	GnomeCanvasGroup *root;
	GnomeCanvasItem *item;

	
	char buf[128];
	int i;

#ifndef GNOME
	gtk_init(&argc,&argv);
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
#else
	gnome_init("gwordview", gwordviewversion, argc,argv);
	window = gnome_app_new("gwordview", "Gnome MSWord Viewer");
#endif
	

	gtk_signal_connect (GTK_OBJECT (window), "delete_event", GTK_SIGNAL_FUNC (delete_event), NULL);
	gtk_signal_connect (GTK_OBJECT (window), "destroy", GTK_SIGNAL_FUNC (destroy), NULL);

	create_menus (GNOME_APP(window));

	vbox = gtk_vbox_new (FALSE, 0);

	gnome_app_set_contents (GNOME_APP (window), vbox);
	

	scroller = gtk_scrolled_window_new (NULL, NULL);
	
	gtk_container_set_border_width (GTK_CONTAINER (scroller), 10);

	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroller),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	
	gtk_widget_set_usize (scroller,  673,870);

	gtk_box_pack_start_defaults (GTK_BOX (vbox), scroller);
	gtk_widget_show (scroller);
	
	ourcanvas = gnome_canvas_new();

	gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW (scroller), ourcanvas);
	
	gtk_widget_set_usize (ourcanvas,  630,891);
	gtk_widget_show (ourcanvas);


	colourmap = gtk_widget_get_colormap(ourcanvas);
	gdk_color_parse ("white", &colour);

	gdk_color_alloc(colourmap, &colour);

	widget_style = gtk_widget_get_style (ourcanvas);
	temp_style = gtk_style_copy (widget_style);
	temp_style->bg[0] = colour;
	temp_style->bg[1] = colour;
	temp_style->bg[2] = colour;
	temp_style->bg[3] = colour;
	temp_style->bg[4] = colour;
	gtk_widget_set_style (ourcanvas, temp_style);

	root = GNOME_CANVAS_GROUP (GNOME_CANVAS (ourcanvas)->root);

	item = gnome_canvas_item_new 
		(
        root, gnome_canvas_text_get_type (),
        "text",     "fish",
        "x",        (double) 100,
        "y",        (double) 100, 
        "font_gdk", window->style->font,
        "anchor",   GTK_ANCHOR_NW,
        "fill_color", "black",
        NULL);
	
	gnome_canvas_update_now (GNOME_CANVAS(ourcanvas));
	

	/*
	ourcanvas = gtk_text_new(NULL,NULL);
	gtk_box_pack_start (GTK_BOX (vbox), ourcanvas, FALSE, FALSE, 2);
	gtk_widget_show (ourcanvas);

	*/

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


GnomeUIInfo main_toolbarinfo[] =
{
  GNOMEUIINFO_ITEM_STOCK(N_("Open"), N_("Open a new file."),
             file_open_cb, GNOME_STOCK_PIXMAP_OPEN),

  GNOMEUIINFO_ITEM_STOCK(N_("Exit"), N_("Quit."),
             callback_exit, GNOME_STOCK_PIXMAP_EXIT),

  GNOMEUIINFO_END
};


void callback_about (GtkWidget *widget, GdkEvent *event)
{
  GtkWidget *about;
  gchar *authors[] = {
    N_("Caolan McNamara (Caolan.McNamara@ul.ie)."),
	NULL
  };

#ifdef ENABLE_NLS
#ifndef ELEMENTS
#define ELEMENTS(x) (sizeof(x) / sizeof(x[0]))
#endif /* ELEMENTS */
  {
    int i;

    for (i = 0; i < (ELEMENTS(authors) - 1); i++)
      authors[i] = _(authors[i]);
  }
#endif /* ENABLE_NLS */

  about = gnome_about_new (_("Gnome MSWord Viewer"), gwordviewversion,
               "(C) 1999 Caolan McNamara",
               (const char **)authors,
               _("MS Word Document Viewer."),
               "wvWare.png");
  gtk_widget_show (about);
}


GnomeUIInfo help_menu[] =
{
  GNOMEUIINFO_HELP("gwordview"),

  GNOMEUIINFO_MENU_ABOUT_ITEM(callback_about, NULL),

  GNOMEUIINFO_END
};


GnomeUIInfo file_menu [] = 
{
GNOMEUIINFO_MENU_OPEN_ITEM(file_open_cb, (gpointer) 0),

GNOMEUIINFO_SEPARATOR,

{ 
GNOME_APP_UI_ITEM, N_("File Info"), N_("Show File Information"),
file_show_info, (gpointer) 0, NULL,
GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_BLUE
},

GNOMEUIINFO_SEPARATOR,

GNOMEUIINFO_MENU_EXIT_ITEM(file_close, (gpointer) 0),

GNOMEUIINFO_END
};

GnomeUIInfo main_menu [] = 
{
	GNOMEUIINFO_MENU_FILE_TREE(file_menu),

/*
	GNOMEUIINFO_MENU_EDIT_TREE(gedit_edit_menu),

	GNOMEUIINFO_MENU_SETTINGS_TREE(gedit_settings_menu),

	GNOMEUIINFO_MENU_WINDOWS_TREE(gedit_window_menu),

*/
	GNOMEUIINFO_MENU_HELP_TREE(help_menu),

	GNOMEUIINFO_END
};



void create_menus (GnomeApp *app)
	{
	gnome_app_create_menus (app, main_menu);
	gnome_app_create_toolbar (app, main_toolbarinfo);
	}

void file_close(GtkWidget *widget, gpointer cbdata)
	{
	gtk_main_quit();
	}

void file_show_info(GtkWidget *widget, gpointer cbdata)
	{
	int i;

	char *titles[] = {"OLE2 Summary","Filesystem Stats","MS Word Internals"};

	char *thing[3][18] = {
		{
		"Title", "Subject", "Author", "Keywords", "Comments", "Template", "Last Revised By",
		"Revision No", "Total Edit Time", "Last Printed Time", "Creation Time", "Last Saved Time",
		"No of Pages", "No of Words", "No of Characters", "Thumbnail", "App Name", "Security"
		},
		{
		"Pathname", "Inode", "Mode", "No of Links", "UID", "GID", "Last Accessed",
		"Last Modified", "Last Status Change", "Size", "File Block Size", "No Of Blocks"
		},
		{
		"Version","Complex (Fastsaved)", "Encrypted", "Has Macro Allocation", "Last Saved under", "Template", "Title", "Subject", "Keywords", "Comments",
		"Author", "Last Revised By", "Data document filename", "Header document filename"
		}
	};

	char lens[3] = {18,12,14};
	char *bufferl;

	GtkWidget *window;
	GtkWidget *button;
	GtkWidget *table;
	GtkWidget *notebook;
	GtkWidget *frame;
	GtkWidget *label;
	GtkWidget *checkbutton;
	GtkWidget *tmptable;
	GdkBitmap *mask;
	GtkWidget *pixmapwid;
	GdkPixmap *my_pix;
	GtkStyle *style;
	int row,col,ret=1,test;
	SummaryInfo si;
	char szTemp[256];
	char *str;
	struct stat buf;
	int statret=-1;



	
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width (GTK_CONTAINER (window), 10);
	table = gtk_table_new(3,6,FALSE);
	gtk_container_add (GTK_CONTAINER (window), table);

	/* Create a new notebook, place the position of the tabs */
	notebook = gtk_notebook_new ();
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
	gtk_table_attach_defaults(GTK_TABLE(table), notebook, 0,6,0,1);
	gtk_widget_show(notebook);

	if (input)
		{
		if (mfilename)
			{
			wvError(("going in %s\n",mfilename));
			statret = stat(mfilename, &buf);
			}
		
		ret = wvSumInfoOpenStream(&si,ps.summary);

		if (ret)
			fprintf(stderr,"open stream failed\n");
		}
	else
		{
		wvError(("no input\n"));
		}

	for (i=0; i < 3; i++) 
		{
		frame = gtk_frame_new (titles[i]);
		gtk_container_set_border_width (GTK_CONTAINER (frame), 10);

		tmptable = gtk_table_new(2,18,FALSE);

		/*we will need the table of names to answer questions like the name of the doc*/
		if ( (wvQuerySupported(&ps.fib,NULL) == 2) || (wvQuerySupported(&ps.fib,NULL) == 3) )
			wvGetSTTBF6(&ps.anSttbfAssoc,ps.fib.fcSttbfAssoc,ps.fib.lcbSttbfAssoc,ps.tablefd);
		else /*word 97*/
			wvGetSTTBF(&ps.anSttbfAssoc,ps.fib.fcSttbfAssoc,ps.fib.lcbSttbfAssoc,ps.tablefd);

		for (row=0;row<lens[i];row++)
			{
			for (col=0;col<1;col++)
				{
				label = gtk_label_new (thing[i][row]);
				gtk_table_attach_defaults(GTK_TABLE(tmptable), label, col,col+1,row,row+1);
				gtk_widget_show(label);
				}
			if (ret == 0)
				{
				switch (i)
					{
					case 0:
						for (col=1;col<2;col++)
							{
							test = wvSumInfoForceString(szTemp, 256, row+2, &si);
							if (test == -1)
								{
								style = gtk_widget_get_style(window);
								my_pix = gdk_pixmap_create_from_xpm(window->window, &mask,
								&style->bg[GTK_STATE_NORMAL], "test.xpm");

								pixmapwid = gtk_pixmap_new( my_pix, mask );
								gtk_widget_show( pixmapwid );

								label = gtk_button_new();
								gtk_container_add( GTK_CONTAINER(button), pixmapwid );
								}
							else if (test != 1)
								{
								label = gtk_label_new (szTemp);
								gtk_table_attach_defaults(GTK_TABLE(tmptable), label, col,col+1,row,row+1);
								}
							gtk_widget_show(label);
							}
						break;
					case 1:
						if (statret != 0)
							{
							wvError(("shite\n"));
							break;
							}
						switch (row)
							{
							case 0:
								label = gtk_label_new (mfilename);
								break;
							case 1:
								sprintf(szTemp,"%d",buf.st_ino);
								label = gtk_label_new (szTemp);
								break;
							case 2:
								sprintf(szTemp,"%c%c%c%c%c%c%c%c%c",
								(buf.st_mode & S_IRUSR) ? 'r' : '-',
								(buf.st_mode & S_IWUSR) ? 'w' : '-',
								(buf.st_mode & S_IXUSR) ? 'x' : '-',
								(buf.st_mode & S_IRGRP) ? 'r' : '-',
								(buf.st_mode & S_IWGRP) ? 'w' : '-',
								(buf.st_mode & S_IXGRP) ? 'x' : '-',
								(buf.st_mode & S_IROTH) ? 'r' : '-',
								(buf.st_mode & S_IWOTH) ? 'w' : '-',
								(buf.st_mode & S_IXOTH) ? 'x' : '-'
								);
								label = gtk_label_new (szTemp);
								break;
							case 3:
								sprintf(szTemp,"%d",buf.st_nlink);
                                label = gtk_label_new (szTemp);
                                break;
							case 4:
								sprintf(szTemp,"%d",buf.st_uid);
                                label = gtk_label_new (szTemp);
                                break;
							case 5:
								sprintf(szTemp,"%d",buf.st_gid);
                                label = gtk_label_new (szTemp);
                                break;
							case 6:
                                label = gtk_label_new (ttoa(buf.st_atime, FALSE));
                                break;
							case 7:
                                label = gtk_label_new (ttoa(buf.st_mtime, FALSE));
                                break;
							case 8:
                                label = gtk_label_new (ttoa(buf.st_ctime, FALSE));
                                break;
							case 9:
								sprintf(szTemp,"%d",buf.st_size);
                                label = gtk_label_new (szTemp);
                                break;
							case 10:
								sprintf(szTemp,"%ld",buf.st_blksize);
                                label = gtk_label_new (szTemp);
                                break;
							case 11:
								sprintf(szTemp,"%ld",buf.st_blocks);
                                label = gtk_label_new (szTemp);
                                break;
							}
						gtk_table_attach_defaults(GTK_TABLE(tmptable), label, col,col+1,row,row+1);
						gtk_widget_show(label);
						break;
					case 2:
						switch (row)
							{
							case 1:
								if (ps.fib.fComplex)
									label = gtk_label_new ("Yes");
								else
									label = gtk_label_new ("No");
								break;
							case 2:
								if (ps.fib.fEncrypted)
									label = gtk_label_new ("Yes");
								else
									label = gtk_label_new ("No");
								break;
							case 3:
								if (ps.fib.lcbCmds)
									label = gtk_label_new ("Yes");
								else
									label = gtk_label_new ("No");
								break;
							case 4:
								if (ps.fib.fMac)
									label = gtk_label_new ("MacOS");
								else
									label = gtk_label_new ("Windows");
								break;
							default:
								label = gtk_label_new ("not imp");
								break;
							}
						if ( (row >4) && (row < 4+ps.anSttbfAssoc.nostrings) )
							{
							if (ps.anSttbfAssoc.extendedflag == 0xFFFF)
								{
								str = wvWideStrToMB(ps.anSttbfAssoc.u16strings[row-4]);
								label = gtk_label_new (str);
								wvFree(str);
								}
							else
								label = gtk_label_new (ps.anSttbfAssoc.s8strings[row-4]);
							}
							gtk_table_attach_defaults(GTK_TABLE(tmptable), label, col,col+1,row,row+1);
							gtk_widget_show(label);
						break;
					}
				}
			}

	

		gtk_container_add (GTK_CONTAINER (frame), tmptable);

		gtk_widget_show(tmptable);
	
		label = gtk_label_new (titles[i]);
		gtk_widget_show (label);

		gtk_notebook_append_page (GTK_NOTEBOOK (notebook), frame, label);
		gtk_widget_show (frame);
		}



	button = gtk_button_new_with_label ("Close");

	gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
	GTK_SIGNAL_FUNC (gtk_widget_destroy),
	GTK_OBJECT (window));


	gtk_table_attach_defaults(GTK_TABLE(table), button, 0,1,1,2);
	gtk_widget_show(button);

	gtk_widget_show(table);
	gtk_widget_show(window);
	}




void file_open_cb(GtkWidget *widget, gpointer cbdata)
	{
	GtkWidget *open_fileselector;
	open_fileselector = gtk_file_selection_new(_("Open File..."));

	gtk_signal_connect(GTK_OBJECT( GTK_FILE_SELECTION(open_fileselector)->cancel_button), "clicked", 
		(GtkSignalFunc)doc_open_filesel_destroy, open_fileselector);

	gtk_signal_connect(GTK_OBJECT( GTK_FILE_SELECTION(open_fileselector)->ok_button), "clicked", 
		(GtkSignalFunc)doc_open_filesel, open_fileselector);

	gtk_widget_show(open_fileselector);
	}

static void doc_open_filesel(GtkWidget *wgt, gpointer cbdata)
	{
	GtkFileSelection *fs = (GtkFileSelection *)cbdata;
	int ret;

	g_print ("%s\n", gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs)));
	mfilename = strdup(gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs)));
	input = fopen(gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs)) ,"rb");

	ret = wvInitParser(&ps,input);
	if (ret)
		{
		wvError(("startup error with file %s\n",gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs))));
		wvOLEFree();
		input=NULL;
		}

	doc_open_filesel_destroy(wgt,cbdata);
	} 

static void doc_open_filesel_destroy(GtkWidget *wgt, gpointer cbdata)
	{
	GtkWidget *w = (GtkWidget *)cbdata;

	g_assert(w != NULL);
	gtk_widget_destroy(w);
	w = NULL;
	} 


/*
 * PUBLIC: file_full_pathname_make
 *
 * given a filename, tries to construct the full pathname to the file.  if
 * successfuly, returns a newly allocated buffer with the full pathname.
 * if the filename is already a full pathname, the pointer to the filename
 * is returned as is (so no new buffer is allocated).
 */
/*nicked from gnotepad C. */
char *file_full_pathname_make(char *fname)
{
        char *full, *cwd;
        int len;

        if (fname[0] == '/')
                return g_strdup(fname);

        if ((cwd = getcwd(NULL, MAXPATH)) == NULL)
                return g_strdup(fname);

        if (cwd[strlen(cwd) - 1] == '/')
                cwd[strlen(cwd) - 1] = '\0';

        len = strlen(cwd) + strlen(fname) + 2;
        full = (char *)g_malloc(len);
        if ((strlen(fname) > 2) && fname[0] == '.' && fname[1] == '/')
                g_snprintf(full, len, "%s/%s", cwd, fname + 2);
        else
                g_snprintf(full, len, "%s/%s", cwd, fname);
        g_free(cwd);

        return full;
} /* file_full_pathname_make */


/*
 * PUBLIC: ttoa
 *
 * converts time_t to an ascii string.  non-reentrant because ctime() is
 * non-reentrant.
 */
/*nicked from gnotepad C. */
char * ttoa(time_t curtime, int append_nl)
	{
	char *timestr;

	timestr = ctime(&curtime);
	if (!append_nl) {
			if (timestr[strlen(timestr) - 1] == '\n')
					timestr[strlen(timestr) - 1] = '\0';
	}

	return timestr;
	} /* ttoa */

