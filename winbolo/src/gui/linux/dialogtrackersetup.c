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
#include <gtk/gtk.h>

#include "../../bolo/global.h"
#include "gamefront.h"

GtkWidget *dialogTrackerUse;
GtkWidget *entry1;
GtkWidget *entry2;
GtkWidget *dialogTrackerUs;

gboolean dialogTrackerUseFunc(GtkWidget *widget,  GdkEventButton *event, gpointer user_data) {
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)) == TRUE) {
    gtk_widget_set_sensitive (entry2, TRUE);
    gtk_widget_set_sensitive (entry1, TRUE);
  } else {
    gtk_widget_set_sensitive (entry2, FALSE);
    gtk_widget_set_sensitive (entry1, FALSE);
  }
  return FALSE;
}


gboolean dialogTrackerOK(GtkWidget *widget,  GdkEventButton *event, gpointer user_data) {
  char addr[FILENAME_MAX]; /* Tracker address   */
  gchar *str;
  unsigned short port;     /* Port              */
  bool enabled;            /* Use of tracker enabled */

  enabled = FALSE;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(dialogTrackerUse)) == TRUE) {
    enabled = TRUE;
  }
  str = gtk_entry_get_text(GTK_ENTRY(entry2));
  port = atoi(str);
  str = gtk_entry_get_text(GTK_ENTRY(entry1));
  strcpy(addr, str);
  gameFrontSetTrackerOptions(addr, port, enabled);
  gtk_grab_remove(dialogTrackerUs);
  gtk_widget_destroy(dialogTrackerUs);
  gtk_main_quit();
  return FALSE;
}

gboolean dialogTrackerCancel(GtkWidget *widget,  GdkEventButton *event, gpointer user_data) {
  gtk_grab_remove(dialogTrackerUs);
  gtk_widget_destroy(dialogTrackerUs);
  gtk_main_quit();

  return FALSE;
}

GtkWidget* dialogTrackerCreate(void) {
  GtkWidget *dialogTracker;
  GtkWidget *vbox1;
  GtkWidget *dialogTrackerok;
  GtkWidget *dialogTrackercancel;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *hbox1;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *hbox2;
  char addr[FILENAME_MAX]; /* Tracker address   */
  unsigned short port;     /* Port              */
  bool enabled;            /* Use of tracker enabled */
      
  dialogTracker = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_object_set_data (GTK_OBJECT (dialogTracker), "dialogTracker", dialogTracker);
  gtk_container_set_border_width (GTK_CONTAINER (dialogTracker), 10);
  gtk_window_set_title (GTK_WINDOW (dialogTracker), "Tracker Config");
  gtk_window_set_position (GTK_WINDOW (dialogTracker), GTK_WIN_POS_CENTER);
  gtk_window_set_modal (GTK_WINDOW (dialogTracker), TRUE);
  gtk_window_set_policy (GTK_WINDOW (dialogTracker), FALSE, FALSE, FALSE);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (dialogTracker), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (dialogTracker), vbox1);

  table1 = gtk_table_new (2, 2, FALSE);
  gtk_widget_ref (table1);
  gtk_object_set_data_full (GTK_OBJECT (dialogTracker), "table1", table1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, TRUE, TRUE, 0);

  label1 = gtk_label_new ("Tracker Address: ");
  gtk_widget_ref (label1);
  gtk_object_set_data_full (GTK_OBJECT (dialogTracker), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_RIGHT);

  label2 = gtk_label_new ("Tracker Port: ");
  gtk_widget_ref (label2);
  gtk_object_set_data_full (GTK_OBJECT (dialogTracker), "label2", label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_RIGHT);

  entry1 = gtk_entry_new ();
  gtk_widget_ref (entry1);
  gtk_object_set_data_full (GTK_OBJECT (dialogTracker), "entry1", entry1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry1);
  gtk_table_attach (GTK_TABLE (table1), entry1, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry2 = gtk_entry_new ();
  gtk_widget_ref (entry2);
  gtk_object_set_data_full (GTK_OBJECT (dialogTracker), "entry2", entry2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry2);
  gtk_table_attach (GTK_TABLE (table1), entry2, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox1);
  gtk_object_set_data_full (GTK_OBJECT (dialogTracker), "hbox1", hbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 6);

  label3 = gtk_label_new ("");
  gtk_widget_ref (label3);
  gtk_object_set_data_full (GTK_OBJECT (dialogTracker), "label3", label3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label3);
  gtk_box_pack_start (GTK_BOX (hbox1), label3, TRUE, TRUE, 0);

  dialogTrackerUse = gtk_check_button_new_with_label ("Use Tracker");
  gtk_widget_ref (dialogTrackerUse);
  gtk_object_set_data_full (GTK_OBJECT (dialogTracker), "dialogTrackerUse", dialogTrackerUse,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (dialogTrackerUse);
  gtk_box_pack_start (GTK_BOX (hbox1), dialogTrackerUse, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dialogTrackerUse), TRUE);

  label4 = gtk_label_new ("");
  gtk_widget_ref (label4);
  gtk_object_set_data_full (GTK_OBJECT (dialogTracker), "label4", label4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label4);
  gtk_box_pack_start (GTK_BOX (hbox1), label4, TRUE, TRUE, 0);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox2);
  gtk_object_set_data_full (GTK_OBJECT (dialogTracker), "hbox2", hbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox2, FALSE, FALSE, 8);

  dialogTrackerok = gtk_button_new_with_label ("OK");
  gtk_widget_ref (dialogTrackerok);
  gtk_object_set_data_full (GTK_OBJECT (dialogTracker), "dialogTrackerok", dialogTrackerok,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (dialogTrackerok);
  gtk_box_pack_start (GTK_BOX (hbox2), dialogTrackerok, TRUE, TRUE, 10);

  dialogTrackercancel = gtk_button_new_with_label ("Cancel");
  gtk_widget_ref (dialogTrackercancel);
  gtk_object_set_data_full (GTK_OBJECT (dialogTracker), "dialogTrackercancel", dialogTrackercancel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (dialogTrackercancel);
  gtk_box_pack_start (GTK_BOX (hbox2), dialogTrackercancel, TRUE, TRUE, 10);
 
  
  
  gameFrontGetTrackerOptions(addr, &port, &enabled);
  /* Set the textboxes up */
  gtk_entry_set_text(GTK_ENTRY(entry1), addr);
  sprintf(addr, "%i", port);
  gtk_entry_set_text(GTK_ENTRY(entry2), addr);
  if (enabled == TRUE) {
    gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(dialogTrackerUse), TRUE);
    gtk_widget_set_sensitive (entry2, TRUE);
    gtk_widget_set_sensitive (entry1, TRUE);

  } else {
    gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(dialogTrackerUse), FALSE);
    gtk_widget_set_sensitive (entry2, FALSE);
    gtk_widget_set_sensitive (entry1, FALSE);
  }
  dialogTrackerUs = dialogTracker;
  gtk_signal_connect(GTK_OBJECT(dialogTracker), "delete_event", GTK_SIGNAL_FUNC (dialogTrackerCancel), NULL);
  gtk_signal_connect(GTK_OBJECT(dialogTrackerok), "clicked", GTK_SIGNAL_FUNC(dialogTrackerOK), 0);
  gtk_signal_connect(GTK_OBJECT(dialogTrackercancel), "clicked", GTK_SIGNAL_FUNC(dialogTrackerCancel), 0);
  gtk_signal_connect(GTK_OBJECT(dialogTrackerUse), "clicked", GTK_SIGNAL_FUNC(dialogTrackerUseFunc), 0);
  
  
  return dialogTracker;
}

