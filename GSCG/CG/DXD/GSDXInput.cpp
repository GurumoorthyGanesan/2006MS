//////////////////////////////////////////////////////////////////
//
//
/*************************************											
//	裝置系統, DXInput_Engine.cpp - v0.1	
//    author : Y.Y.L Date : 07/01/03
//************************************/
//-----------------------------------------------------------------


#include "..\..\Common\Global.h"

//	--------------------------------------------------------------------------------
//
// cInput Class
//
//	--------------------------------------------------------------------------------
cInput::cInput()
{
  m_pDI = NULL;
}

cInput::~cInput()
{
  Shutdown();
}

BOOL cInput::Init(HWND hWnd, HINSTANCE hInst)
{
  Shutdown();

	// 紀錄視窗的控制權
  m_hWnd = hWnd;

  // Create a DirectInput 介面
  if(FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDI, NULL)))
    return FALSE;

  return TRUE;
}

BOOL cInput::Shutdown()
{
  // Release IDirectInput8 COM 物件
  ReleaseCOM(m_pDI);

  // 歸還控制權
  m_hWnd = NULL;

  return TRUE;
}

//	--------------------------------------------------------------------------------
//
// cInputDevice Class
//
//	--------------------------------------------------------------------------------
cInputDevice::cInputDevice()
{
  // Clear cInput 物件
  m_Input = NULL;

  // 設定型態
  m_Type = NONE;

	// 設給視窗使用
  m_Windowed = TRUE;

  // Clear DirectInput 介面指標
  m_pDIDevice = NULL;

  // Point the mouse structures to the state buffer
  m_MouseState    = (DIMOUSESTATE*)&m_State;

  Clear();
}

cInputDevice::~cInputDevice()
{
  Free();
}

BOOL cInputDevice::Create(cInput *Input, short Type, BOOL Windowed)
{
  DIDATAFORMAT *DataFormat;

  Free();

  if((m_Input = Input) == NULL)
    return FALSE;

  // Create the device and remember device data format
  switch(Type) {
    case KEYBOARD:
           if(FAILED(m_Input->GetDirectInputCOM()->CreateDevice(GUID_SysKeyboard, &m_pDIDevice, NULL))) 
             return FALSE;
           DataFormat = (DIDATAFORMAT*)&c_dfDIKeyboard;
           break;

    case MOUSE:
           if(FAILED(m_Input->GetDirectInputCOM()->CreateDevice(GUID_SysMouse, &m_pDIDevice, NULL))) 
             return FALSE;
           DataFormat = (DIDATAFORMAT*)&c_dfDIMouse;
           break;

    default: return FALSE;
  }

  // 確定使用
  m_Windowed = Windowed;

	// 設定keyboard的資料格式
  if(FAILED(m_pDIDevice->SetDataFormat(DataFormat)))
    return FALSE;

	// 類似direct 7.0的合作level 方式
  if(FAILED(m_pDIDevice->SetCooperativeLevel(m_Input->GethWnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
    return FALSE;

	// 要求裝置
  if(FAILED(m_pDIDevice->Acquire()))
    return FALSE;

  // 設定裝置形態 
  m_Type = Type;

  Clear();

  return FALSE;
}

BOOL cInputDevice::Free()
{
  if(m_pDIDevice != NULL) {
    m_pDIDevice->Unacquire();
    ReleaseCOM(m_pDIDevice);
  }

  // 設定未使用
  m_Type = NONE;
 
  Clear();

  return TRUE;
}

BOOL cInputDevice::Clear()
{
  short i;

  ZeroMemory(&m_State, 256);
  
  for(i=0;i<256;i++)
    m_Locks[i]  = FALSE;

  m_XPos = 0;
  m_YPos = 0;

  return TRUE;
}

BOOL cInputDevice::Read()
{
  HRESULT hr;
  long BufferSizes[2] = { 256, sizeof(DIMOUSESTATE) };
  short i;

  if(m_pDIDevice == NULL)
    return FALSE;

  if(m_Type < 1 || m_Type > 2)
    return FALSE;

  // Loop polling and reading until succeeded or unknown error
  // Also take care of lost-focus problems
  while(1) {
    // Poll
    m_pDIDevice->Poll();

		// 獲取狀態
    if(SUCCEEDED(hr = m_pDIDevice->GetDeviceState(BufferSizes[m_Type-1], (LPVOID)&m_State)))
      break;

    // 未知的錯誤
    if(hr != DIERR_INPUTLOST && hr != DIERR_NOTACQUIRED)
      return FALSE;

		// 再次請求新的裝置權
    if(FAILED(m_pDIDevice->Acquire()))
      return FALSE;
  }

  // Since only the mouse coordinates are relative, you'll
  // have to deal with them now
  if(m_Type == MOUSE) {
		// 取得視窗座標
    if(m_Windowed == TRUE) {
      POINT pt;
      GetCursorPos(&pt);
      ScreenToClient(m_Input->GethWnd(), &pt);
      m_XPos = pt.x;
      m_YPos = pt.y;
    } else {
      m_XPos += m_MouseState->lX;
      m_YPos += m_MouseState->lY;
    }
  }

  // Released keys and button need to be unlocked
  switch(m_Type) {
    case KEYBOARD:
      for(i=0;i<256;i++) {
        if(!(m_State[i] & 0x80))
          m_Locks[i] = FALSE;
      }
      break;
    case MOUSE:
      for(i=0;i<4;i++) {
        if(!(m_MouseState->rgbButtons[i]))
          m_Locks[i] = FALSE;
       }
       break;
  }

  return TRUE;
}

BOOL cInputDevice::Acquire(BOOL Active)
{
  if(m_pDIDevice == NULL)
    return FALSE;

  if(Active == TRUE)
    m_pDIDevice->Acquire();
  else
    m_pDIDevice->Unacquire();

  return TRUE;
}

BOOL cInputDevice::GetKeyState(char Num)
{
  // Check if key/button is pressed
  if(m_State[Num] & 0x80 && m_Locks[Num] == FALSE)
    return TRUE;
  return FALSE;
}

BOOL cInputDevice::SetKeyState(char Num, BOOL State)
{
  m_State[Num] = State;
  return TRUE;
}

BOOL cInputDevice::GetPureKeyState(char Num)
{
  return ((m_State[Num] & 0x80) ? TRUE : FALSE);
}

short cInputDevice::GetKeypress(long TimeOut)
{
  static HKL KeyboardLayout = GetKeyboardLayout(0);
  unsigned char WinKeyStates[256], DIKeyStates[256];
  unsigned short i, ScanCode, VirtualKey, Keys, Num;
  unsigned long EndTime;
  
  if((m_Type != KEYBOARD) || (m_pDIDevice == NULL))
    return 0;

  // Calculate end time for TimeOut
  EndTime = GetTickCount() + TimeOut;

  // Loop until timeout or key pressed
  while(1) {
    // Get Windows keyboard state
    GetKeyboardState(WinKeyStates);

    // Get DirectInput keyboard state
    m_pDIDevice->GetDeviceState(256, DIKeyStates);

    // Scan through looking for key presses
    for(i=0;i<256;i++) {
      // If one found, try to convert it
      if(DIKeyStates[i] & 0x80) {
        // Get virtual key code
        if((VirtualKey = MapVirtualKeyEx((ScanCode = i), 1, KeyboardLayout))) {
          // Get ASCII code of key and return it
          Num = ToAsciiEx(VirtualKey, ScanCode, WinKeyStates, &Keys, 0, KeyboardLayout);
          if(Num)
            return Keys;
        }
      }
    }

    // Check for TimeOut
    if(TimeOut) {
      if(GetTickCount() > EndTime)
        return 0;
    }
  }

  return 0;
}

long cInputDevice::GetNumKeyPresses()
{
  long i, Num = 0;

  for(i=0;i<256;i++) {
    if(m_State[i] & 0x80 && m_Locks[i] == FALSE)
      Num++;
  }

  return Num;
}

long cInputDevice::GetNumPureKeyPresses()
{
  long i, Num = 0;

  for(i=0;i<256;i++) {
    if(m_State[i] & 0x80)
      Num++;
  }

  return Num;
}

BOOL cInputDevice::GetButtonState(char Num)
{
  char State = 0;

  if(m_Type == MOUSE)
    State = m_MouseState->rgbButtons[Num];

  // Check if key/button is pressed
  if(State & 0x80 && m_Locks[Num] == FALSE)
    return TRUE;
  return FALSE;
}

BOOL cInputDevice::SetButtonState(char Num, BOOL State)
{
  if(m_Type == MOUSE) {
    m_MouseState->rgbButtons[Num] = State;
    return TRUE;
  }

  return FALSE;
}

BOOL cInputDevice::GetPureButtonState(char Num)
{
  if(m_Type == MOUSE)
    return m_MouseState->rgbButtons[Num];

  return FALSE;
}

long cInputDevice::GetNumButtonPresses()
{
  long i, Num = 0;
  
  if(m_Type == MOUSE) {
    for(i=0;i<4;i++) {
      if(m_MouseState->rgbButtons[i] & 0x80 && m_Locks[i] == FALSE)
        Num++;
    }
  }

  return Num;
}

long cInputDevice::GetNumPureButtonPresses()
{
  long i, Num = 0;
  
  if(m_Type == MOUSE) {
    for(i=0;i<4;i++) {
      if(m_MouseState->rgbButtons[i] & 0x80)
        Num++;
    }
  }

  return Num;
}

