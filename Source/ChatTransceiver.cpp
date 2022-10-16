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

#include "atlstr.h"
#include <iostream>
#include <cstdio>              // for sample output
#include <string>

// Shared Memory buffer size
#define BUF_SIZE 256

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

// Constants
TCHAR shared_memory_name[] = "rF2_ChatTransceiver_SM";

void ChatTransceiverPlugin::Startup( long version )
{
  // default HW control enabled to true
  mEnabled = true;
  open_shared_memory();
}


void ChatTransceiverPlugin::Shutdown()
{
  close_shared_memory();
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
  inside_realtime_ = true;
}


void ChatTransceiverPlugin::ExitRealtime()
{
  inside_realtime_ = false;
}

const char* make_string_copy(std::string s) {
  char* p = new char[s.length() + 1];
  strcpy_s(p, s.length() + 1, s.c_str());
  const char* r = p;
  return r;
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
  _tprintf("Opened shared memory: %s\n", shared_memory_name);
  return true;
}

bool write_shared_memory(const HANDLE &h_map_file, const char* message_to_write)
{
  // Check size
  const size_t size = strlen(message_to_write);
  if (size > BUF_SIZE)
  {
    // Skip large messages
    _tprintf("write_shared_memory: Message of length %llu too large!\n", size);
    return false;
  }
  
  // Convert const char*
  TCHAR t_msg [BUF_SIZE];
  _tcscpy(t_msg, A2T(const_cast<LPSTR>(message_to_write)));
  const auto write_size = _tcslen(t_msg) * sizeof(TCHAR);
  
  // Open
  const auto p_buf = MapViewOfFile(h_map_file,   // handle to map object
                                  FILE_MAP_ALL_ACCESS, // read/write permission
                                  0,
                                  0,
                                  BUF_SIZE);
  if (p_buf == nullptr) {
    _tprintf("write_shared_memory: Could not map view of file (%lu).\n",
             GetLastError());
    return false;
  }

  // write shared memory
  _tprintf("write_shared_memory: Writing to memory %llu\n", write_size);
  CopyMemory(p_buf, t_msg, write_size);
  // _getch(); // will block!?

  // release resources
  _tprintf("write_shared_memory: Unmapping view\n");
  UnmapViewOfFile(p_buf);
  return true;
}

void ChatTransceiverPlugin::open_shared_memory()
{
  const bool created = create_shared_memory(h_map_file_);
  if (! created)
  {
    mEnabled = false;
    _tprintf(TEXT("Could not create shared memory.\n"));
  }
}

void ChatTransceiverPlugin::close_shared_memory() const
{
  if (h_map_file_ == nullptr) { return; }
  CloseHandle(h_map_file_);
}

std::string ChatTransceiverPlugin::read_shared_memory() const
{
  std::string result;
  if (! mEnabled) { return result; }
  
  char *buf = static_cast<char*>(
    ::MapViewOfFile(h_map_file_,
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
  if (! inside_realtime_) { return false; }

  // Display Welcome Message
  if (! displayed_welcome_message_)
  {
    msg_info.mDestination = 0;
    msg_info.mTranslate = 0;
    sprintf(msg_info.mText, "ChatTransceiver plugin started.");
    displayed_welcome_message_ = true;
    
    return true;
  }

  // Read message from shared memory
  const std::string message = read_shared_memory();
  if (message == last_message_) { return false; }

  // Display new message
  if (! message.empty())
  {
    msg_info.mDestination = 0;
    msg_info.mTranslate = 0;
    sprintf(msg_info.mText, message.c_str());
    last_message_ = message;
    
    return true; 
  }
  return false;
}

extern "C" __declspec( dllexport )
bool __cdecl send_message(const char* messages_chars)
{
  _tprintf("add_message: Creating and opening handle\n");
  HANDLE e_h_map_file = INVALID_HANDLE_VALUE;
  const bool shared_memory_created = create_shared_memory(e_h_map_file);
  if (! shared_memory_created) { return false; }

  _tprintf("add_message: writing to handle\n");
  const bool write_success = write_shared_memory(e_h_map_file, messages_chars);
  _tprintf("add_message: Closing Handle\n");
  CloseHandle(e_h_map_file);

  _tprintf("add_message: Handle closed\n");
  return write_success;
}