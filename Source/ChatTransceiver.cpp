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


// plugin information
extern "C" __declspec( dllexport )
const char * __cdecl GetPluginName()                   { return( "ChatTransceiverPlugin - 2022.10.23" ); }

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
TCHAR shared_memory_name[] = "rF2_ChatTransceiver_SM";


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


// Constants
bool displayed_message = false;

// Logging
#ifdef ENABLE_LOG
FILE* out_file = nullptr;
#endif


ChatTransceiverPlugin::ChatTransceiverPlugin()
{
  m_enabled_ = false;
  m_et_ = 0.0;
}


ChatTransceiverPlugin::~ChatTransceiverPlugin()
{
  m_enabled_ = false;
  m_et_ = 0.0;
#ifdef ENABLE_LOG
  if (out_file) {
    fclose(out_file);
    out_file = nullptr;
  }
#endif
}


void ChatTransceiverPlugin::Startup( long version )
{
#ifdef ENABLE_LOG
  WriteLog("--STARTUP--");
#endif
  
  // default HW control enabled to true
  m_enabled_ = true;
  open_shared_memory();
}


void ChatTransceiverPlugin::Shutdown()
{
  close_shared_memory();
#ifdef ENABLE_LOG
  WriteLog("--SHUTDOWN--");
#endif /* ENABLE_LOG */
}


void ChatTransceiverPlugin::StartSession()
{
#ifdef ENABLE_LOG
  WriteLog("--STARTSESSION--");
#endif /* ENABLE_LOG */
}


void ChatTransceiverPlugin::EndSession()
{
#ifdef ENABLE_LOG
  WriteLog("--ENDSESSION--");
  if (out_file) {
    fclose(out_file);
    out_file = nullptr;
  }
#endif /* ENABLE_LOG */
  displayed_message = false;
}


void ChatTransceiverPlugin::EnterRealtime()
{
  // start up timer every time we enter realtime
  m_et_ = 0.0;
}


void ChatTransceiverPlugin::ExitRealtime()
{
  // Exit Realtime
}

void ChatTransceiverPlugin::WriteLog(const char * const msg)
{
  #ifdef ENABLE_LOG
    if (out_file == nullptr)
    {
      out_file = fopen(LOG_FILE, "a");
    }

    if (out_file != nullptr)
    {
      fprintf(out_file, "%s\n", msg);
    }
  #endif
}
const char* make_string_copy(std::string s) {
  char* p = new char[s.length() + 1];
  strcpy_s(p, s.length() + 1, s.c_str());
  const char* r = p;
  return r;
}


void ChatTransceiverPlugin::open_shared_memory()
{
  const bool created = create_shared_memory(h_map_file_);
  if (! created)
  {
    m_enabled_ = false;
#ifdef ENABLE_LOG
    WriteLog("Could not create shared memory.\n");
#endif
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
  if (! m_enabled_) { return result; }

  const char *buf = static_cast<char*>(
    ::MapViewOfFile(h_map_file_,
      FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE)
  );
  
  if (buf == nullptr)
  {
#ifdef ENABLE_LOG
    char* log_msg{};
    sprintf(log_msg, "Could not map view of file (%lu).\n", GetLastError());
    WriteLog(log_msg);
#endif
    return result;
  }
  result = buf;
  std::string copy_result = make_string_copy(result);
  
  UnmapViewOfFile(buf);
  return copy_result;
}


bool ChatTransceiverPlugin::update_from_shared_memory(std::string& message, unsigned char& destination) const
{
  // Read shared memory content
  const std::string mem_content = read_shared_memory();
  if (mem_content.empty()) { return false; }

  // Get the first chr as destination
  const std::string mem_destination = mem_content.substr(0, 1);
  if (mem_destination != "0" || mem_destination != "1") { return false; }

  // Get message content
  const std::string mem_message = mem_content.substr(1, 129);
  if (mem_message.empty()) { return false; }

  // Update message and destination by reference
  message = mem_message;
  destination = static_cast<unsigned char>(*mem_destination.c_str());
  return true;
}


bool ChatTransceiverPlugin::WantsToDisplayMessage( MessageInfoV01 &msgInfo )
{
  if (! m_enabled_) { return false; }

  // Display Welcome Message
  if (! displayed_welcome_message_)
  {
    msgInfo.mDestination = 0;
    msgInfo.mTranslate = 0;
    std::strcpy(msgInfo.mText, "ChatTransceiver plugin started.");
    displayed_welcome_message_ = true;

#ifdef ENABLE_LOG
    WriteLog("Displayed welcome message.");
#endif 
    
    return true;
  }

  // Read message and destination from shared memory
  std::string message;
  unsigned char destination;
  if (! update_from_shared_memory(message, destination)) { return false; }
  if (message == last_message_) { return false; }
  
  // Display new message
  msgInfo.mDestination = destination;
  msgInfo.mTranslate = 0;
  sprintf(msgInfo.mText, message.c_str());
  last_message_ = message;

#ifdef ENABLE_LOG
  fprintf(out_file, "Displayed message: %s", message.c_str());
#endif 

  return true;
}


extern "C" __declspec( dllexport )
bool __cdecl send_message(const char* messages_chars)
{
  _tprintf("send_message: Creating and opening handle\n");
  HANDLE e_h_map_file = INVALID_HANDLE_VALUE;
  const bool shared_memory_created = create_shared_memory(e_h_map_file);
  if (! shared_memory_created) { return false; }

  _tprintf("send_message: writing to handle\n");
  const bool write_success = write_shared_memory(e_h_map_file, messages_chars);
  _tprintf("send_message: Closing Handle\n");
  CloseHandle(e_h_map_file);

  _tprintf("send_message: Handle closed\n");
  return write_success;
}