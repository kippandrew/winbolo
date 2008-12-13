/*
 * $Id$
 *
 * Copyright (c) 1998-2008 John Morrison.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "smalllogo.xpm"
#include "../../bolo/global.h"
#include "gamefront.h"
#include "messagebox.h"
#include "dialogwinbolonet.h"
#include "boloicon.xpm"


GtkWidget *idc_opentutorial;
GtkWidget *idc_opensingle;
GtkWidget *idc_opentcp;
GtkWidget *idc_openlocal;
GtkWidget *idc_openinternet;
GtkWidget *us;

GtkRequisition req;
gint opening_configure_event (GtkWidget *window, GdkEventConfigure *event) {
  GdkPixmap *pixmap;
  GdkBitmap *mask;

  pixmap = gdk_pixmap_create_from_xpm_d (window->window, &mask, &window->style->bg[GTK_STATE_NORMAL], boloicon_xpm);
  gdk_window_set_icon (window->window, NULL, pixmap, mask);
  return TRUE;
}



gboolean dialogOpeningClose(GtkWidget *widget,  GdkEventButton *event, gpointer user_data) {
  gtk_widget_destroy(us);
  gtk_main_quit();
  return FALSE;
}

void dialogOpeningWinbolonet(GtkWidget *widget, gpointer user_data) {
  GtkWidget *wbn;

  wbn = dialogWinboloNetCreate();
  gtk_grab_add(wbn);
  gtk_widget_show(wbn);
  gtk_main();

}

void dialogOpeningOK(GtkWidget *widget, gpointer user_data) {
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(idc_opentcp)) == TRUE) {
    gameFrontSetDlgState(us, openUdp);
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(idc_openlocal)) == TRUE) {
    gameFrontSetDlgState(us, openLan);
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(idc_openinternet)) == TRUE) {
    gameFrontSetDlgState(us, openInternet);
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(idc_opentutorial)) == TRUE) {
    gameFrontSetDlgState(us, openTutorial);
  } else {
    gameFrontSetDlgState(us, openSetup);
  }
}

void dialogOpeningQuit(GtkWidget *widget, gpointer user_data) {
  gdk_key_repeat_restore();
  exit(0);
}

void dialogOpeningCloseBox(GtkWidget *widget, gpointer user_data) {
  gdk_key_repeat_restore();
  exit(0);
}


void dialogOpeningHelp(GtkWidget *widget, gpointer user_data) {
  MessageBox("Please consult the manual.pdf file in the LinBolo directory.", DIALOG_BOX_TITLE);
}

GtkWidget* dialogOpeningCreate(void) {
  GtkWidget *dialogOpening;
  GtkWidget *vbox1;
  GtkWidget *pixmap1;
  GtkWidget *label1;
  GSList *vbox1_group = NULL;
  GtkWidget *idc_openskip;
  GtkWidget *table1;
  GtkWidget *idc_openquit;
  GtkWidget *idc_openhelp;
  GtkWidget *idc_openlanguages;
  GtkWidget *idc_openok;
  GdkPixmap *pixmap_data;
  GdkPixmap *pixmap_mask;
  GtkWidget *pixw;

  
  dialogOpening = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_object_set_data (GTK_OBJECT (dialogOpening), "dialogOpening", dialogOpening);
  gtk_container_set_border_width (GTK_CONTAINER (dialogOpening), 20);
  gtk_window_set_title (GTK_WINDOW (dialogOpening), "Network Selection");
  gtk_window_set_policy (GTK_WINDOW (dialogOpening), FALSE, FALSE, FALSE);
  gtk_window_set_position (GTK_WINDOW (dialogOpening), GTK_WIN_POS_CENTER);
  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (dialogOpening), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (dialogOpening), vbox1);
  
  pixmap_data = gdk_pixmap_colormap_create_from_xpm_d(NULL, gtk_widget_get_colormap(dialogOpening), &pixmap_mask, NULL, smalllogo_xpm);
  pixmap1 = gtk_pixmap_new(pixmap_data, pixmap_mask); 
  gtk_widget_ref (pixmap1);
  gdk_pixmap_unref(pixmap_data);
  gdk_pixmap_unref(pixmap_mask);
  gtk_object_set_data_full (GTK_OBJECT (dialogOpening), "pixmap1", pixmap1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pixmap1);
  gtk_box_pack_start (GTK_BOX (vbox1), pixmap1, TRUE, TRUE, 0);

  label1 = gtk_label_new ("Welcome to LinBolo, the multiplayer tank game.\nPlease choose a game type from the list below:\n");
  gtk_widget_ref (label1);
  gtk_object_set_data_full (GTK_OBJECT (dialogOpening), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox1), label1, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  idc_opentutorial = gtk_radio_button_new_with_label (vbox1_group, "Tutorial (Instruction for first-time player)");
  vbox1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (idc_opentutorial));
  gtk_widget_ref (idc_opentutorial);
  gtk_object_set_data_full (GTK_OBJECT (dialogOpening), "idc_opentutorial", idc_opentutorial,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (idc_opentutorial);
  gtk_box_pack_start (GTK_BOX (vbox1), idc_opentutorial, FALSE, FALSE, 0);

  idc_opensingle = gtk_radio_button_new_with_label (vbox1_group, "Practise (Single Player; No Network)");
  vbox1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (idc_opensingle));
  gtk_widget_ref (idc_opensingle);
  gtk_object_set_data_full (GTK_OBJECT (dialogOpening), "idc_opensingle", idc_opensingle, (GtkDestroyNotify) gtk_widget_unref);
  gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(idc_opensingle), TRUE);
  gtk_widget_show (idc_opensingle);
  gtk_box_pack_start (GTK_BOX (vbox1), idc_opensingle, FALSE, FALSE, 0);
  idc_opentcp = gtk_radio_button_new_with_label (vbox1_group, "TCP/IP");
  vbox1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (idc_opentcp));
  gtk_widget_ref (idc_opentcp);
  gtk_object_set_data_full (GTK_OBJECT (dialogOpening), "idc_opentcp", idc_opentcp,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (idc_opentcp);
  gtk_box_pack_start (GTK_BOX (vbox1), idc_opentcp, FALSE, FALSE, 0);

  idc_openlocal = gtk_radio_button_new_with_label (vbox1_group, "Local Network (Broadcast Search)");
  vbox1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (idc_openlocal));
  gtk_widget_ref (idc_openlocal);
  gtk_object_set_data_full (GTK_OBJECT (dialogOpening), "idc_openlocal", idc_openlocal,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (idc_openlocal);
  gtk_box_pack_start (GTK_BOX (vbox1), idc_openlocal, FALSE, FALSE, 0);

  idc_openinternet = gtk_radio_button_new_with_label (vbox1_group, "Internet (Tracker Search)");
  vbox1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (idc_openinternet));
  gtk_widget_ref (idc_openinternet);
  gtk_object_set_data_full (GTK_OBJECT (dialogOpening), "idc_openinternet", idc_openinternet,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (idc_openinternet);
  gtk_box_pack_start (GTK_BOX (vbox1), idc_openinternet, FALSE, FALSE, 0);

  idc_openskip = gtk_check_button_new_with_label ("Skip this dialog next time");
  gtk_widget_ref (idc_openskip);
  gtk_object_set_data_full (GTK_OBJECT (dialogOpening), "idc_openskip", idc_openskip,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (idc_openskip);
  gtk_box_pack_start (GTK_BOX (vbox1), idc_openskip, FALSE, FALSE, 10);
  gtk_widget_set_sensitive (idc_openskip, FALSE);

  table1 = gtk_table_new (2, 2, FALSE);
  gtk_widget_ref (table1);
  gtk_object_set_data_full (GTK_OBJECT (dialogOpening), "table1", table1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, TRUE, TRUE, 0);

  idc_openquit = gtk_button_new_with_label ("Quit");
  gtk_widget_ref (idc_openquit);
  gtk_object_set_data_full (GTK_OBJECT (dialogOpening), "idc_openquit", idc_openquit,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (idc_openquit);
  gtk_table_attach (GTK_TABLE (table1), idc_openquit, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  idc_openhelp = gtk_button_new_with_label ("Help");
  gtk_widget_ref (idc_openhelp);
  gtk_object_set_data_full (GTK_OBJECT (dialogOpening), "idc_openhelp", idc_openhelp,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (idc_openhelp);
  gtk_table_attach (GTK_TABLE (table1), idc_openhelp, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  idc_openlanguages = gtk_button_new_with_label ("WinBolo.net");
  gtk_widget_ref (idc_openlanguages);
  gtk_object_set_data_full (GTK_OBJECT (dialogOpening), "idc_openwinbolonet", idc_openlanguages, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_set_sensitive(idc_openlanguages, TRUE);
  gtk_widget_show (idc_openlanguages);
  gtk_table_attach (GTK_TABLE (table1), idc_openlanguages, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  idc_openok = gtk_button_new_with_label ("OK");
  gtk_widget_ref (idc_openok);
  gtk_object_set_data_full (GTK_OBJECT (dialogOpening), "idc_openok", idc_openok,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (idc_openok);
  gtk_table_attach (GTK_TABLE (table1), idc_openok, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  gtk_signal_connect(GTK_OBJECT(dialogOpening), "delete_event", GTK_SIGNAL_FUNC (dialogOpeningClose), NULL);
  gtk_signal_connect(GTK_OBJECT(idc_openquit), "clicked", GTK_SIGNAL_FUNC(dialogOpeningQuit), 0);
  gtk_signal_connect(GTK_OBJECT(idc_openok), "clicked", GTK_SIGNAL_FUNC(dialogOpeningOK), 0);
  gtk_signal_connect(GTK_OBJECT(idc_openhelp), "clicked", GTK_SIGNAL_FUNC(dialogOpeningHelp), 0);  
  gtk_signal_connect(GTK_OBJECT(idc_openlanguages), "clicked", GTK_SIGNAL_FUNC(dialogOpeningWinbolonet), 0);  
  gtk_signal_connect(GTK_OBJECT(dialogOpening), "destroy", GTK_SIGNAL_FUNC(dialogOpeningCloseBox), 0);  
  us = dialogOpening;

  gtk_signal_connect(GTK_OBJECT(us), "configure_event", GTK_SIGNAL_FUNC(opening_configure_event), 0);

  
  return dialogOpening;
}

