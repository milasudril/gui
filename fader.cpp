#ifdef __WAND__
target[name[fader.o] type[object] dependency[comctl32;external] dependency[gdi32;external]platform[;Windows]]
#endif

#include "fader.h"

#define _WIN32_IE 0x5500
#include <windows.h>
#include <commctrl.h>
#include <herbs/stringsys/stringsys.h>
#include <herbs/floatformat/floatformat.h>

namespace
	{
//	TODO: The largest relevant message code found in commctrl.h is WM_USER+33.
//	To make sure we do not interfear with subsequent versions of the DLL, add 64.
	static const uint32_t SIZE_UPDATE=WM_USER+64;
	}

void Gui::Fader::init()
	{
	INITCOMMONCONTROLSEX a;
	a.dwSize=sizeof(a);
	a.dwICC=ICC_WIN95_CLASSES;
	InitCommonControlsEx(&a);
	}

Gui::Fader::Fader(Gui& gui_obj,uint32_t style_0,uint32_t style_1,Window* parent):
	WindowSystem(gui_obj,TRACKBAR_CLASS,style_0,style_1|TBS_NOTICKS|TBS_BOTH,parent)
	{
	font_sys_current=GetStockObject(DEFAULT_GUI_FONT);
	PostMessage((HWND)handle,SIZE_UPDATE,0,0);
	}

double Gui::Fader::valueGet() const
	{
	pos_t v=SendMessage((HWND)handle,TBM_GETPOS,0,0);
	pos_t max=SendMessage((HWND)handle,TBM_GETRANGEMAX,0,0);
	return positionSet(v,max);
	}

void Gui::Fader::valueSet(double y)
	{
	pos_t max=SendMessage((HWND)handle,TBM_GETRANGEMAX,0,0);
	pos_t v=positionGet(y,max);
	SendMessage((HWND)handle,TBM_SETPOS,1,(LPARAM)v);
	}

void Gui::Fader::nDivsSet(pos_t n_divs)
	{
	double val=valueGet();
	SendMessage((HWND)handle,TBM_SETRANGEMAX, 0,n_divs);
	valueSet(val);
	}
	
size_t Gui::Fader::onEvent(uint32_t event_type,size_t param_0,size_t param_1)
	{
	switch(event_type)
		{
		case MessagePaint:
			{
			WindowSystem::onEvent(event_type,param_0,param_1);
			
			HDC dc=GetDC((HWND)handle);
			RECT knob;
			SendMessage((HWND)handle,TBM_GETTHUMBRECT,0,(LPARAM)(&knob));
			double v=valueGet();
			
			Herbs::FloatFormat<15> str_out(v);
			auto size=sizeClientGet();
			RECT r={2,knob.bottom,size.x,size.y};
			FillRect(dc,&r,GetSysColorBrush(COLOR_BTNFACE));
			SetBkMode(dc, TRANSPARENT);
			
			SelectObject(dc,(HGDIOBJ)font_sys_current);
			SetTextColor(dc,GetSysColor(COLOR_BTNTEXT));
			TextOut(dc, 2, knob.bottom
				,Herbs::bufferSysPtr(Herbs::stringsys(str_out))
				,Herbs::String::count(str_out)
				); 
				
			ReleaseDC((HWND)handle,dc);
			PostMessage(GetParent((HWND)handle),MessageCommand,idGet(),(LPARAM)handle);
			}
			break;
			
		case MessageSize:
			{
			double v=valueGet();
			size_text=sizeLine(Herbs::FloatFormat<15>(v));
			
			RECT knob;
			SendMessage((HWND)handle,TBM_GETTHUMBRECT,0,(LPARAM)(&knob));
			auto size=sizeWindowGet();
			sizeAbsolute(size.x,size_text.y+knob.bottom);
			}
			break;
	
		case MessageSetfont:
			font_sys_current=(void*)(param_0);
			PostMessage((HWND)handle,SIZE_UPDATE,0,0);
			break;
			
		case SIZE_UPDATE:
			size_text=sizeLine(Herbs::FloatFormat<15>(valueGet()));
			return 0;
		}
	return WindowSystem::onEvent(event_type,param_0,param_1);
	}