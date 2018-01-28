#include<windows.h>
#include<gl/gl.h>
#include<cstdio>
#include<cstdlib>
#include<cmath>
#include"SOIL.h"
#define PI 3.1415926535897932384626433832795
#define min(x,y) ((x)<(y)?(x):(y))
using namespace std;
int KPSClock=250,KPSMax=25,VS=1,KT,KT1,KB;
double KN,KN1,KA;
volatile bool KS[256],BGcolor=0;
volatile int FPSLIMIT=120;
volatile int KC=0,head=0,tail=0;
volatile double T0,que[1000];
DWORD g_main_tid=0;
HHOOK g_kb_hook=0;

GLuint NAtex,Num0tex,Num1tex,Num2tex,Num3tex,Num4tex,Num5tex,Num6tex,Num7tex,Num8tex,Num9tex,BG1tex,Ptex;

double CPUclock(){
	LARGE_INTEGER nFreq;
	LARGE_INTEGER t1;
	double dt;
 	QueryPerformanceFrequency(&nFreq);
 	QueryPerformanceCounter(&t1);
  	dt=(t1.QuadPart)/(double)nFreq.QuadPart;
  	return(dt*1000);
}
void addqueue(){
	KC++;
	que[(tail++)%1000]=CPUclock();
}
void popqueue(){
	while(1){		
		Sleep(10);
		while(head<tail&&que[head%1000]<(CPUclock()-1000)){
			head++;
			KC--;
		}
	}
}
BOOL CALLBACK con_handler(DWORD){
	PostThreadMessage(g_main_tid,WM_QUIT,0,0);
	return TRUE;
};
LRESULT CALLBACK kb_proc(int code,WPARAM w,LPARAM l){
	PKBDLLHOOKSTRUCT p=(PKBDLLHOOKSTRUCT)l;
	if((w==WM_KEYDOWN||w==WM_SYSKEYDOWN)&&(!KS[(p->vkCode)-1])){addqueue();KS[(p->vkCode)-1]=1;}
	if(w==WM_KEYUP||w==WM_SYSKEYUP)KS[(p->vkCode)-1]=0;
	return CallNextHookEx(g_kb_hook,code,w,l);
}
void KHK(){
	g_main_tid=GetCurrentThreadId();
	SetConsoleCtrlHandler(&con_handler,TRUE);
    g_kb_hook=SetWindowsHookEx(WH_KEYBOARD_LL,&kb_proc,GetModuleHandle(NULL),0);
	MSG msg;
    while(GetMessage(&msg,NULL,0,0)){
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}
	UnhookWindowsHookEx (g_kb_hook);
}
int read(FILE *fp){
	int x=0,f=1;char ch=fgetc(fp);
	while(ch<'0'||ch>'9'){if(ch=='-')f=-1;ch=fgetc(fp);}
	while(ch>='0'&&ch<='9'){x=x*10+ch-'0';ch=fgetc(fp);}
	return x*f;
}
void init(){
	FILE *fp;
	if(!(fp=fopen("config.ini","r"))){
		fp=fopen("config.ini","w");
		fprintf(fp,"[Config]\n");
		fprintf(fp,"Interval(ms)=%d\n",KPSClock);
		fprintf(fp,"Keys Limit=%d\n",KPSMax);
		fprintf(fp,"FPS Limit=%d\n",FPSLIMIT);
		fprintf(fp,"Vertical Sync=%d\n",VS);
		fprintf(fp,"Use Green Background=%d\n",BGcolor);
		fclose(fp);
	}else{
		KPSClock=read(fp);
		KPSMax=read(fp);
		FPSLIMIT=read(fp);
		VS=read(fp);
		BGcolor=read(fp);
		fclose(fp);
	}
}
GLuint Drawnumber(const int &x){
	switch(x){
		case 0:
			return Num0tex;
		break;
		case 1:
			return Num1tex;
		break;
		case 2:
			return Num2tex;
		break;
		case 3:
			return Num3tex;
		break;
		case 4:
			return Num4tex;
		break;
		case 5:
			return Num5tex;
		break;
		case 6:
			return Num6tex;
		break;
		case 7:
			return Num7tex;
		break;
		case 8:
			return Num8tex;
		break;
		case 9:
			return Num9tex;
		break;
	}
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
void EnableOpenGL(HWND hWnd,HDC *hDC,HGLRC *hRC);
void DisableOpenGL(HWND hWnd,HDC hDC,HGLRC hRC);
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int iCmdShow){
	init();
	T0=CPUclock();
	HANDLE h1=CreateThread(0,0,(LPTHREAD_START_ROUTINE)KHK,0,1,0);ResumeThread(h1);CloseHandle(h1);
	h1=CreateThread(0,0,(LPTHREAD_START_ROUTINE)popqueue,0,1,0);ResumeThread(h1);CloseHandle(h1);

    WNDCLASS wc;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;        
    MSG msg;
    BOOL bQuit=FALSE;
    
    wc.style=CS_OWNDC;
    wc.lpfnWndProc=WndProc;
    wc.cbClsExtra=0;
    wc.cbWndExtra=0;
    wc.hInstance=hInstance;
    wc.hIcon=LoadIcon(NULL,IDI_APPLICATION);
    wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName=NULL;
    wc.lpszClassName="Window";
    RegisterClass(&wc);
    hWnd=CreateWindow("Window","KPS DashBoard v2(OpenGL)",WS_CAPTION|WS_POPUPWINDOW|WS_VISIBLE,100,100,366,388,NULL,NULL,hInstance,NULL);
	
    EnableOpenGL(hWnd,&hDC,&hRC);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    
	BG1tex=SOIL_load_OGL_texture("SKIN/BG1.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_POWER_OF_TWO|SOIL_FLAG_DDS_LOAD_DIRECT);
	Num0tex=SOIL_load_OGL_texture("SKIN/0.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_POWER_OF_TWO|SOIL_FLAG_DDS_LOAD_DIRECT);
	Num1tex=SOIL_load_OGL_texture("SKIN/1.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_POWER_OF_TWO|SOIL_FLAG_DDS_LOAD_DIRECT);
	Num2tex=SOIL_load_OGL_texture("SKIN/2.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_POWER_OF_TWO|SOIL_FLAG_DDS_LOAD_DIRECT);
	Num3tex=SOIL_load_OGL_texture("SKIN/3.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_POWER_OF_TWO|SOIL_FLAG_DDS_LOAD_DIRECT);
	Num4tex=SOIL_load_OGL_texture("SKIN/4.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_POWER_OF_TWO|SOIL_FLAG_DDS_LOAD_DIRECT);
	Num5tex=SOIL_load_OGL_texture("SKIN/5.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_POWER_OF_TWO|SOIL_FLAG_DDS_LOAD_DIRECT);
	Num6tex=SOIL_load_OGL_texture("SKIN/6.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_POWER_OF_TWO|SOIL_FLAG_DDS_LOAD_DIRECT);
	Num7tex=SOIL_load_OGL_texture("SKIN/7.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_POWER_OF_TWO|SOIL_FLAG_DDS_LOAD_DIRECT);
	Num8tex=SOIL_load_OGL_texture("SKIN/8.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_POWER_OF_TWO|SOIL_FLAG_DDS_LOAD_DIRECT);
	Num9tex=SOIL_load_OGL_texture("SKIN/9.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_POWER_OF_TWO|SOIL_FLAG_DDS_LOAD_DIRECT);
	NAtex=SOIL_load_OGL_texture("SKIN/-.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_POWER_OF_TWO|SOIL_FLAG_DDS_LOAD_DIRECT);
	Ptex=SOIL_load_OGL_texture("SKIN/P.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_POWER_OF_TWO|SOIL_FLAG_DDS_LOAD_DIRECT);
	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
    while (!bQuit){
        if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
            if(msg.message==WM_QUIT){
                bQuit=TRUE;
            }
            else{
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }else{
        	const float constFps=(float)(VS?60:FPSLIMIT);
        	DWORD timeInPerFrame=1000.0f/constFps;
        	DWORD timeBegin=GetTickCount();
            /* OpenGL animation code goes here */
            if(BGcolor)glClearColor(0.0f,1.0f,0.0f,1.0f);else glClearColor(0.0f,0.0f,0.0f,0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            double x;
            KT1=min(100,KC);
	    	if(CPUclock()>KPSClock+T0)T0=CPUclock();
			if(KT!=KT1&&(CPUclock()>KPSClock/2.0+T0)){
				T0=CPUclock();
				KT=KT1;
				KN1=KN;
			}
			if(KN-KT>0.1||KN-KT<-0.1){
				x=CPUclock()-T0;
				KN=double(16*(KN1-KT))/double(9*KPSClock*KPSClock)*(x*3.0/4.0)*(x*3.0/4.0)+double(8*(KT-KN1))/double(3*KPSClock)*(x*3.0/4.0)+KN1;
			}else{KN1=KT;KN=KT;}
			KA=min(KN,(double)KPSMax);
			KB=min(round(KN),100);
            float y=(float)KA*4.375f/float(KPSMax)-2.22;
            
            glPushMatrix();
            
			glBindTexture(GL_TEXTURE_2D,BG1tex);
			
            glBegin (GL_QUADS);
            glTexCoord2f(0.0f,1.0f);
			glVertex2f (-1.0f,-1.0f);
            glTexCoord2f(0.0f,0.0f);
			glVertex2f (-1.0f,1.0f);
            glTexCoord2f(1.0f,0.0f);
			glVertex2f (1.0f,1.0f);
            glTexCoord2f(1.0f,1.0f);
			glVertex2f (1.0f,-1.0f);
			glEnd ();
            glPopMatrix ();
            
            if(KB==100){
            	glBindTexture(GL_TEXTURE_2D,NAtex);
           		glPushMatrix();
				glScaled(0.215f/1.5f,0.215f/1.5f,1.0f);
				glTranslatef(1.25-1.3812,0.5-0.935,0.0f); 
				
		        glBegin (GL_QUADS);
		        glTexCoord2f(0.0f,1.0f);
				glVertex2f (-1.0f,-1.0f);
		        glTexCoord2f(0.0f,0.0f);
				glVertex2f (-1.0f,1.0f);
		        glTexCoord2f(1.0f,0.0f);
				glVertex2f (1.0f,1.0f);
		        glTexCoord2f(1.0f,1.0f);
				glVertex2f (1.0f,-1.0f);
		        glEnd ();            
		        glPopMatrix ();
		            
				glPushMatrix();
		        glScaled(0.215f/1.5f,0.215f/1.5f,1.0f);
				glTranslatef(1.25-1.10322,0.5-0.935,0.0f); 
				
		        glBegin (GL_QUADS);
		        glTexCoord2f(0.0f,1.0f);
				glVertex2f (-1.0f,-1.0f);
		        glTexCoord2f(0.0f,0.0f);
				glVertex2f (-1.0f,1.0f);
		        glTexCoord2f(1.0f,0.0f);
				glVertex2f (1.0f,1.0f);
		    	glTexCoord2f(1.0f,1.0f);
				glVertex2f (1.0f,-1.0f);
		    	glEnd ();
		           
		        glPopMatrix ();
					
			}else{
				glBindTexture(GL_TEXTURE_2D,Drawnumber(KB/10));
				
				glPushMatrix();
				glScaled(0.215f/1.5f,0.215f/1.5f,1.0f);
				glTranslatef(-0.65,-2.05,0.0f); 
		        glBegin (GL_QUADS);
		        glTexCoord2f(0.0f,1.0f);
				glVertex2f (-1.0f,-1.0f);
		        glTexCoord2f(0.0f,0.0f);
				glVertex2f (-1.0f,1.0f);
		        glTexCoord2f(1.0f,0.0f);
				glVertex2f (1.0f,1.0f);
		        glTexCoord2f(1.0f,1.0f);
				glVertex2f (1.0f,-1.0f);
		        glEnd ();
		        glPopMatrix ();
				
				
				glBindTexture(GL_TEXTURE_2D,Drawnumber(KB%10));
				
				glPushMatrix();
				glScaled(0.215f/1.5f,0.215f/1.5f,1.0f);
				glTranslatef(+0.65,-2.05,0.0f);
		        glBegin (GL_QUADS);
		        glTexCoord2f(0.0f,1.0f);
				glVertex2f (-1.0f,-1.0f);
		        glTexCoord2f(0.0f,0.0f);
				glVertex2f (-1.0f,1.0f);
		    	glTexCoord2f(1.0f,0.0f);
				glVertex2f (1.0f,1.0f);
		        glTexCoord2f(1.0f,1.0f);
				glVertex2f (1.0f,-1.0f);
		        glEnd ();
		        glPopMatrix ();
			}
            glPushMatrix();
            glRotatef (180.0*(-0.025-y)/PI, 0.0f, 0.0f, 1.0f);
            glBindTexture(GL_TEXTURE_2D,Ptex);
            glBegin (GL_QUADS);
            glTexCoord2f(0.0f,1.0f);
			glVertex2f (-1.0f,-1.0f);
            glTexCoord2f(0.0f,0.0f);
			glVertex2f (-1.0f,1.0f);
            glTexCoord2f(1.0f,0.0f);
			glVertex2f (1.0f,1.0f);
            glTexCoord2f(1.0f,1.0f);
			glVertex2f (1.0f,-1.0f);
            glEnd ();
            
            glPopMatrix ();
            
            
            SwapBuffers (hDC);
            
            DWORD timePhase=GetTickCount()-timeBegin;
        	if(timePhase<timeInPerFrame)Sleep(DWORD(timeInPerFrame-timePhase));
        }
    }
    DisableOpenGL(hWnd,hDC,hRC);
    DestroyWindow(hWnd);
    return msg.wParam;
}
LRESULT CALLBACK WndProc (HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){
    switch (message){
    case WM_CREATE:
        return 0;
    case WM_CLOSE:
        PostQuitMessage (0);
        return 0;
    case WM_DESTROY:
        return 0;
    case WM_KEYDOWN:
        return 0;
    default:
        return DefWindowProc(hWnd,message,wParam,lParam);
    }
}

void EnableOpenGL(HWND hWnd,HDC *hDC,HGLRC *hRC){
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;
    *hDC=GetDC(hWnd);
    ZeroMemory(&pfd,sizeof(pfd));
    pfd.nSize=sizeof(pfd);
    pfd.nVersion=1;
    pfd.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
    pfd.iPixelType=PFD_TYPE_RGBA;
    pfd.cColorBits=24;
    pfd.cDepthBits=16;
    pfd.iLayerType=PFD_MAIN_PLANE;
    iFormat=ChoosePixelFormat(*hDC,&pfd);
    SetPixelFormat(*hDC,iFormat,&pfd);
    *hRC=wglCreateContext(*hDC);
    wglMakeCurrent(*hDC,*hRC);

}
void DisableOpenGL(HWND hWnd,HDC hDC,HGLRC hRC){
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd,hDC);
}
