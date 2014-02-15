/** 
 * Copyright (c) 2014, Fabian Knorr
 * 
 * This file is part of libgtk3-opengl.
 *
 * libgtk3-opengl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libgtk3-opengl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libgtk3-opengl. If not, see <http://www.gnu.org/licenses/>.
 */

#include "canvas.h"
#include "canvas_impl.h"
#include <stdlib.h>
#include <windows.h>
#include <gtk/gtk.h>
#include <gdk/gdkwin32.h>
#include <glib-object.h>


struct _GtkGLCanvas_NativePriv 
{	
	HDC dc;
    HGLRC gl;
};


GtkGLCanvas_NativePriv*
gtk_gl_canvas_native_new()
{
	GtkGLCanvas_NativePriv *native = malloc(sizeof(GtkGLCanvas_NativePriv));
    native->gl = native->dc = NULL;
	return native;
}


void 
gtk_gl_canvas_native_create_context(GtkGLCanvas *canvas, const GtkGLAttributes *attrs)
{
	GtkGLCanvas_Priv *priv = GTK_GL_CANVAS_GET_PRIV(canvas);
	GtkGLCanvas_NativePriv *native = priv->native;

	PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
        PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
        32,                        //Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                        //Number of bits for the depthbuffer
        8,                        //Number of bits for the stencilbuffer
        0,                        //Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };
	HWND hwnd;
	HDC dc;
	int pf;
	
    hwnd = GDK_WINDOW_HWND(gtk_widget_get_window(GTK_WIDGET(canvas)));
	dc = GetDC(hwnd);
	native->dc = dc;
	pf = ChoosePixelFormat(dc, &pfd);
	SetPixelFormat(dc, pf, &pfd);
	native->gl = wglCreateContext(dc);
	printf("hwnd=%lu\ndc=%lu\npf=%lu\ngl=%lu\n", hwnd, dc, pf, native->gl);
	
}


void 
gtk_gl_canvas_native_attach_context(GtkGLCanvas_Priv *priv)
{
	GtkGLCanvas_NativePriv *native = priv->native;
}


void 
gtk_gl_canvas_native_destroy_context(GtkGLCanvas_Priv *priv)
{
	GtkGLCanvas_NativePriv *native = priv->native;
	
    if (native->dc && native->gl) {
		wglMakeCurrent(native->dc, NULL);
		wglDeleteContext(native->gl);
		native->gl = native->dc = NULL;
    }
}


void
gtk_gl_canvas_native_make_current(GtkGLCanvas_Priv *priv)
{
    GtkGLCanvas_NativePriv *native = priv->native;
    wglMakeCurrent(native->dc, native->gl);
}


void
gtk_gl_canvas_native_swap_buffers(GtkGLCanvas_Priv *priv)
{
    GtkGLCanvas_NativePriv *native = priv->native;
    wglSwapBuffers(native->gl);
}

