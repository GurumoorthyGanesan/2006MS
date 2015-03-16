//////////////////////////////////////////////////////////////////
//
//
/*************************************											
//	裝置系統, DXInput_Engine.h - v0.1	
//    author : Y.Y.L Date : 07/01/03
//************************************/
//-----------------------------------------------------------------

#ifndef _DXINPUT_CORE_H_
#define _DXINPUT_CORE_H_

#define ReleaseCOM(x) if(x) { x->Release(); x = NULL; }

// 列舉裝置形態
enum InputDevices {
  NONE = 0,
  KEYBOARD,
  MOUSE
};

//
// 鍵盤按鍵定義
// 
#define KEY_RELEASED        FALSE
#define KEY_PRESSED         TRUE

#define KEY_UNLOCKED        FALSE
#define KEY_LOCKED          TRUE

#define KEY_ESC             DIK_ESCAPE
#define KEY_ESCAPE          DIK_ESCAPE

#define KEY_MINUS           DIK_MINUS
#define KEY_EQUALS          DIK_EQUALS

#define KEY_BACKSPACE       DIK_BACK

#define KEY_TAB             DIK_TAB

#define KEY_LBRACKET        DIK_LBRACKET
#define KEY_RBRACKET        DIK_RBRACKET

#define KEY_ENTER           DIK_RETURN
#define KEY_RETURN          DIK_RETURN

#define KEY_CTRL            DIK_LCONTROL
#define KEY_LCTRL           DIK_LCONTROL
#define KEY_RCTRL           DIK_RCTRL

#define KEY_SHIFT           DIK_LSHIFT
#define KEY_LSHIFT          DIK_LSHIFT
#define KEY_RSHIFT          DIK_RSHIFT

#define KEY_ALT             DIK_LMENU
#define KEY_LALT            DIK_LMENU
#define KEY_RALT            DIK_RMENU

#define KEY_0               DIK_0
#define KEY_1               DIK_1
#define KEY_2               DIK_2
#define KEY_3               DIK_3
#define KEY_4               DIK_4
#define KEY_5               DIK_5
#define KEY_6               DIK_6
#define KEY_7               DIK_7
#define KEY_8               DIK_8
#define KEY_9               DIK_9

#define KEY_A               DIK_A
#define KEY_B               DIK_B
#define KEY_C               DIK_C
#define KEY_D               DIK_D
#define KEY_E               DIK_E
#define KEY_F               DIK_F
#define KEY_G               DIK_G
#define KEY_H               DIK_H
#define KEY_I               DIK_I
#define KEY_J               DIK_J
#define KEY_K               DIK_K
#define KEY_L               DIK_L
#define KEY_M               DIK_M
#define KEY_N               DIK_N
#define KEY_O               DIK_O
#define KEY_P               DIK_P
#define KEY_Q               DIK_Q
#define KEY_R               DIK_R
#define KEY_S               DIK_S
#define KEY_T               DIK_T
#define KEY_U               DIK_U
#define KEY_V               DIK_V
#define KEY_W               DIK_W
#define KEY_X               DIK_X
#define KEY_Y               DIK_Y
#define KEY_Z               DIK_Z

#define KEY_SEMICOLON       DIK_SEMICOLON
#define KEY_APOSTROPHE      DIK_APOSTROPHE

#define KEY_TILDE           DIK_GRAVE
#define KEY_GRAVE           DIK_GRAVE

#define KEY_BACKSLASH       DIK_BACKSLASH

#define KEY_COMMA           DIK_COMMA
#define KEY_PERIOD          DIK_PERIOD
#define KEY_FORWARDSLASH    DIK_SLASH
#define KEY_SLASH           DIK_SLASH

#define KEY_SPACE           DIK_SPACE

#define KEY_CAPSLOCK        DIK_CAPITAL
#define KEY_CAPITAL         DIK_CAPITAL

#define KEY_F1              DIK_F1
#define KEY_F2              DIK_F2
#define KEY_F3              DIK_F3
#define KEY_F4              DIK_F4
#define KEY_F5              DIK_F5
#define KEY_F6              DIK_F6
#define KEY_F7              DIK_F7
#define KEY_F8              DIK_F8
#define KEY_F9              DIK_F9
#define KEY_F10             DIK_F10
#define KEY_F11             DIK_F11
#define KEY_F12             DIK_F12

#define KEY_SYSRQ           DIK_SYSRQ
#define KEY_SCROLLLOCK      DIK_SCROLL
#define KEY_PAUSE           DIK_PAUSE

#define KEY_NUMLOCK         DIK_NUMLOCK
#define KEY_NUMPAD0         DIK_NUMPAD0
#define KEY_NUMPAD1         DIK_NUMPAD1
#define KEY_NUMPAD2         DIK_NUMPAD2
#define KEY_NUMPAD3         DIK_NUMPAD3
#define KEY_NUMPAD4         DIK_NUMPAD4
#define KEY_NUMPAD5         DIK_NUMPAD5
#define KEY_NUMPAD6         DIK_NUMPAD6
#define KEY_NUMPAD7         DIK_NUMPAD7
#define KEY_NUMPAD8         DIK_NUMPAD8
#define KEY_NUMPAD9         DIK_NUMPAD9
#define KEY_ADD             DIK_ADD
#define KEY_SUBTRACT        DIK_SUBTRACT
#define KEY_DIVIDE          DIK_DEVICE
#define KEY_MULTIPLY        DIK_MULTIPLY
#define KEY_DECIMAL         DIK_DECIMAL
#define KEY_NUMPADENTER     DIK_NUMPADENTER

#define KEY_INSERT          DIK_INSERT
#define KEY_DELETE          DIK_DELETE
#define KEY_HOME            DIK_HOME
#define KEY_END             DIK_END
#define KEY_PAGEUP          DIK_PRIOR
#define KEY_PAGEDOWN        DIK_NEXT

#define KEY_UP              DIK_UP
#define KEY_DOWN            DIK_DOWN
#define KEY_LEFT            DIK_LEFT
#define KEY_RIGHT           DIK_RIGHT

#define KEY_LWIN            DIK_LWIN
#define KEY_RWIN            DIK_RWIN
#define KEY_APPS            DIK_APPS


//
// mouse 定義
//
#define BUTTON_RELEASED    FALSE
#define BUTTON_PRESSED      TRUE

#define BUTTON_UNLOCKED    FALSE
#define BUTTON_LOCKED       TRUE

#define MOUSE_LBUTTON          0
#define MOUSE_RBUTTON          1
#define MOUSE_MBUTTON          2

// 
// 裝置物件引擎系統
//

class cInput  
{
  protected:
      HWND           m_hWnd;
      IDirectInput8 *m_pDI;

  public:
    cInput();
    ~cInput();

    IDirectInput8 *GetDirectInputCOM(void) const {	return m_pDI;}
    HWND           GethWnd(void)           const {	return m_hWnd; }
    
    BOOL Init(HWND hWnd, HINSTANCE hInst);
    BOOL Shutdown();
};

class cInputDevice
{
  public:
    cInput               *m_Input;

    short                 m_Type;
    IDirectInputDevice8  *m_pDIDevice;

    BOOL                  m_Windowed;

    char                  m_State[256];
    DIMOUSESTATE         *m_MouseState;

    BOOL                  m_Locks[256];

    long                  m_XPos, m_YPos;
      

  public:
    cInputDevice();
    ~cInputDevice();

    IDirectInputDevice8 *DeviceCOM(void) const {	return m_pDIDevice;}


    // Generic functions - all devices
    BOOL Create(cInput *Input, short Type, BOOL Windowed = TRUE);
    BOOL Free();

    BOOL Clear();
    BOOL Read();
    BOOL Acquire(BOOL Active = TRUE);

    BOOL GetLock(char Num)	const										{		return m_Locks[Num];	}
    BOOL SetLock(char Num, BOOL State = TRUE)				{		m_Locks[Num] = State;		return TRUE;}

    long GetXPos(void)			const										{		return m_XPos;}
    BOOL SetXPos(long XPos)													{		m_XPos = XPos;					return TRUE;}

    long GetYPos(void)			const										{		return m_YPos;}
    BOOL SetYPos(long YPos)													{		m_YPos = YPos;					return TRUE;}

    long GetXDelta(void)		const										{		return m_MouseState->lX;}
    long GetYDelta(void)		const										{		return m_MouseState->lY;}

    // Keyboard 專用
    BOOL  GetKeyState(char Num);
    BOOL  SetKeyState(char Num, BOOL State);
    BOOL  GetPureKeyState(char Num);
    short GetKeypress(long TimeOut = 0);
    long  GetNumKeyPresses();
    long  GetNumPureKeyPresses();

    // Mouse 專用
    BOOL  GetButtonState(char Num);
    BOOL  SetButtonState(char Num, BOOL State);
    BOOL  GetPureButtonState(char Num);
    long  GetNumButtonPresses();
    long  GetNumPureButtonPresses();
};

#endif
