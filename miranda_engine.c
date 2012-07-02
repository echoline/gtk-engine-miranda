/* Miranda - a GTK+ engine
 *
 * Copyright (C) 2011 Carlos Garnacho <carlosg@gnome.org>
 * Copyright (C) 2011 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Authors: Carlos Garnacho <carlosg@gnome.org>
 *          Cosimo Cecchi <cosimoc@gnome.org>
 *          Eli Cohen <eli.neoturbine.net>
 */

#include <gtk/gtk.h>
#include <gmodule.h>
#include <math.h>
#include <cairo-gobject.h>

#define MIRANDA_NAMESPACE "miranda"

typedef struct _MirandaEngine MirandaEngine;
typedef struct _MirandaEngineClass MirandaEngineClass;

struct _MirandaEngine
{
  GtkThemingEngine parent_object;
};

struct _MirandaEngineClass
{
  GtkThemingEngineClass parent_class;
};

#define MIRANDA_TYPE_ENGINE		 (miranda_engine_get_type ())
#define MIRANDA_ENGINE(object)		 (G_TYPE_CHECK_INSTANCE_CAST ((object), MIRANDA_TYPE_ENGINE, MirandaEngine))
#define MIRANDA_ENGINE_CLASS(klass)	 (G_TYPE_CHECK_CLASS_CAST ((klass), MIRANDA_TYPE_ENGINE, MirandaEngineClass))
#define MIRANDA_IS_ENGINE(object)	 (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIRANDA_TYPE_ENGINE))
#define MIRANDA_IS_ENGINE_CLASS(klass)	 (G_TYPE_CHECK_CLASS_TYPE ((klass), MIRANDA_TYPE_ENGINE))
#define MIRANDA_ENGINE_GET_CLASS(obj)	 (G_TYPE_INSTANCE_GET_CLASS ((obj), MIRANDA_TYPE_ENGINE, MirandaEngineClass))

GType miranda_engine_get_type	    (void) G_GNUC_CONST;
void  miranda_engine_register_types (GTypeModule *module);

G_DEFINE_DYNAMIC_TYPE (MirandaEngine, miranda_engine, GTK_TYPE_THEMING_ENGINE)

void
miranda_engine_register_types (GTypeModule *module)
{
  miranda_engine_register_type (module);
}

cairo_surface_t *pool = NULL;
gint pool_width = 0;
gint pool_height = 0;

static void
miranda_engine_init (MirandaEngine *self)
{
}

static void
pool_gen (gint width, gint height)
{
  gdouble xs, ys, inc, end;
  gint color = 0;

  if (pool)
    cairo_surface_destroy (pool);

  pool = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
  cairo_t *cr = cairo_create(pool);

  xs = width / 2.0;
  ys = height / 2.0;

  end = sqrt(width * width + height * height);

  cairo_set_line_width (cr, 10.0);
  for (inc = 0; inc < end; inc += 9.0) {
    switch (color) {
    case 0:
      cairo_set_source_rgba (cr, 1.0, 0.75, 0.75, 1.0);
      break;
    case 1:
      cairo_set_source_rgba (cr, 1.0, 0.8125, 0.75, 1.0);
      break;
    case 2:
      cairo_set_source_rgba (cr, 1.0, 1.0, 0.75, 1.0);
      break;
    case 3:
      cairo_set_source_rgba (cr, 0.75, 1.0, 0.75, 1.0);
      break;
    case 4:
      cairo_set_source_rgba (cr, 0.75, 0.75, 1.0, 1.0);
      break;
    case 5:
      cairo_set_source_rgba (cr, 1.0, 0.75, 1.0, 1.0);
      break;
    }
    color++;
    color %= 6;

    cairo_arc (cr, xs, ys, inc, 0.0, 2.0 * M_PI);
    cairo_stroke (cr);
  }

  cairo_destroy (cr);
}

static void
miranda_engine_render_background (GtkThemingEngine *engine,
                                  cairo_t          *cr,
                                  gdouble           x,
                                  gdouble           y,
                                  gdouble           width,
                                  gdouble           height)
{
  GdkRGBA color;
  GtkStateFlags flags = gtk_theming_engine_get_state (engine);
  gboolean generate = FALSE;

  gtk_theming_engine_get_background_color (engine, flags, &color);

  cairo_save (cr);

  if (color.red == color.green == color.blue == 0) {
    if (width > pool_width) {
      generate = TRUE;
      pool_width = width;
    }
    if (height > pool_height) {
      generate = TRUE;
      pool_height = height;
    }
    if (generate) {
      pool_gen (width, height);
    }
    cairo_set_source_surface (cr, pool, 0, 0);
  }
  else
    gdk_cairo_set_source_rgba (cr, &color);

  cairo_rectangle (cr, x, y, width, height);
  cairo_fill (cr);

  cairo_restore (cr);
}

static void
miranda_engine_class_init (MirandaEngineClass *klass)
{
  GtkThemingEngineClass *engine_class = GTK_THEMING_ENGINE_CLASS (klass);

  engine_class->render_background = miranda_engine_render_background;
}

static void
miranda_engine_class_finalize (MirandaEngineClass *klass)
{
}

G_MODULE_EXPORT void
theme_init (GTypeModule *module)
{
  miranda_engine_register_types (module);
}

G_MODULE_EXPORT void
theme_exit (void)
{
}

G_MODULE_EXPORT GtkThemingEngine *
create_engine (void)
{
  return GTK_THEMING_ENGINE (g_object_new (MIRANDA_TYPE_ENGINE,
                                           "name", "miranda",
                                           NULL));
}
