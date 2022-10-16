//‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
//›                                                                         ﬁ
//› Module: Internals Example Source File                                   ﬁ
//›                                                                         ﬁ
//› Description: Declarations for the Internals Example Plugin              ﬁ
//›                                                                         ﬁ
//›                                                                         ﬁ
//› This source code module, and all information, data, and algorithms      ﬁ
//› associated with it, are part of CUBE technology (tm).                   ﬁ
//›                 PROPRIETARY AND CONFIDENTIAL                            ﬁ
//› Copyright (c) 1996-2014 Image Space Incorporated.  All rights reserved. ﬁ
//›                                                                         ﬁ
//›                                                                         ﬁ
//› Change history:                                                         ﬁ
//›   tag.2005.11.30: created                                               ﬁ
//›                                                                         ﬁ
//ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

#include "ChatTransceiver.hpp"  // corresponding header file

#include <SDKDDKVer.h>
#include "atlstr.h"
#include <iostream>
#include <cstdio>              // for sample output
#include <string>


// plugin information

extern "C" __declspec( dllexport )
const char * __cdecl GetPluginName()                   { return( "ChatTransceiverPlugin - 2022.10.16" ); }

extern "C" __declspec( dllexport )
PluginObjectType __cdecl GetPluginType()               { return( PO_INTERNALS ); }

extern "C" __declspec( dllexport )
int __cdecl GetPluginVersion()                         { return( 7 ); } // InternalsPluginV07

extern "C" __declspec( dllexport )
PluginObject * __cdecl CreatePluginObject()            { return( (PluginObject *) new ChatTransceiverPlugin ); }

extern "C" __declspec( dllexport )
void __cdecl DestroyPluginObject( PluginObject *obj )  { delete( (ChatTransceiverPlugin *) obj ); }

// Shared Memory buffer size
#define BUF_SIZE 256

// Constants
bool displayed_message = false;
bool inside_realtime = false;
TCHAR shared_memory_name[] = "chat_memory";

void ChatTransceiverPlugin::Startup( long version )
{
  // default HW control enabled to true
  mEnabled = true;
  
  open_shared_memory();
}


void ChatTransceiverPlugin::Shutdown()
{
  close_shared_memory();
  // WriteToAllExampleOutputFiles( "a", "-SHUTDOWN-" );
}


void ChatTransceiverPlugin::StartSession()
{
  // WriteToAllExampleOutputFiles( "a", "--STARTSESSION--" );
}


void ChatTransceiverPlugin::EndSession()
{
  // WriteToAllExampleOutputFiles( "a", "--ENDSESSION--" );
}


void ChatTransceiverPlugin::EnterRealtime()
{
  // start up timer every time we enter realtime
  mET = 0.0;
  inside_realtime = true;
}


void ChatTransceiverPlugin::ExitRealtime()
{
  inside_realtime = false;
}

bool create_shared_memory(HANDLE &h_map_file)
{
  h_map_file = ::CreateFileMapping(
              INVALID_HANDLE_VALUE,    // use paging file
              NULL,                    // default security
              PAGE_READWRITE,          // read/write access
              0,                       // maximum object size (high-order DWORD)
              BUF_SIZE,                // maximum object size (low-order DWORD)
              shared_memory_name);     // name of mapping object
  if (h_map_file == nullptr)
  {
    _tprintf(TEXT("Could not create file mapping object (%lu).\n"),
             GetLastError());
    return false;
  }
  return true;
}

void ChatTransceiverPlugin::open_shared_memory()
{
  const bool created = create_shared_memory(h_map_file);
  if (! created)
  {
    mEnabled = false;
    _tprintf(TEXT("Could not create shared memory.\n"));
  }
}

void ChatTransceiverPlugin::close_shared_memory()
{
  if (h_map_file == nullptr) { return; }
  CloseHandle(h_map_file);
}

const char* make_string_copy(std::string s) {
  char* p = new char[s.length() + 1];
  strcpy_s(p, s.length() + 1, s.c_str());
  const char* r = p;
  return r;
}

std::string ChatTransceiverPlugin::read_shared_memory()
{
  std::string result;
  if (! mEnabled) { return result; }
  
  char *buf = static_cast<char*>(
    ::MapViewOfFile(h_map_file,
      FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE)
  );
  
  if (buf == nullptr)
  {
    _tprintf(TEXT("Could not map view of file (%lu).\n"),
             GetLastError());
    return result;
  }
  result = buf;
  std::string copy_result = make_string_copy(result);
  
  UnmapViewOfFile(buf);
  return copy_result;
}

bool ChatTransceiverPlugin::WantsToDisplayMessage( MessageInfoV01 &msg_info)
{
  if (! mEnabled) { return false; }
  if (! inside_realtime) { return false; }
  
  if (! displayed_message)
  {
    msg_info.mDestination = 0;
    msg_info.mTranslate = 0;
    sprintf(msg_info.mText, "ChatTransceiver plugin started.");
    displayed_message = true;
    
    return ( true ); 
  }
  const std::string message = read_shared_memory();
  
  if (! message.empty())
  {
    msg_info.mDestination = 0;
    msg_info.mTranslate = 0;
    sprintf(msg_info.mText, message.c_str());
    
    return ( true ); 
  }
  return ( false );
}

extern "C" __declspec( dllexport )
bool __cdecl add_message(const char* messages_chars)
{
  HANDLE e_h_map_file = INVALID_HANDLE_VALUE;
  const bool result = create_shared_memory(e_h_map_file);
  if (! result) { return false; }
  
  std::cout << "Received:" << messages_chars << std::endl;
  
  TCHAR t_msg [BUF_SIZE];
  _tcscpy(t_msg, A2T(const_cast<LPSTR>(messages_chars)));
  
  const auto p_buf = MapViewOfFile(e_h_map_file, // handle to map object
                                  FILE_MAP_ALL_ACCESS, // read/write permission
                                  0,
                                  0,
                                  BUF_SIZE);
  if (p_buf == nullptr) {
    _tprintf(TEXT("Could not map view of file (%lu).\n"),
             GetLastError());
    return false;
  }
  
  // write shared memory
  CopyMemory((PVOID)p_buf, t_msg, (_tcslen(t_msg) * sizeof(TCHAR)));
  _getch();

  // release resources
  UnmapViewOfFile(p_buf);
  CloseHandle(e_h_map_file);
  
  return true;
}