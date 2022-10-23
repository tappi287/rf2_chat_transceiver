//‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
//›                                                                         ﬁ
//› Module: Internals Example Header File                                   ﬁ
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

#ifndef _INTERNALS_EXAMPLE_H
#define _INTERNALS_EXAMPLE_H

#include <string>
#include <windows.h>
#include <conio.h>

#include "InternalsPlugin.hpp"
bool open_shared_memory(HANDLE h_map_file);

// This is used for the app to use the plugin for its intended purpose
class ChatTransceiverPlugin : public InternalsPluginV07  // REMINDER: exported function GetPluginVersion() should return 1 if you are deriving from this InternalsPluginV01, 2 for InternalsPluginV02, etc.
{
 public:
  // Constructor/destructor
  ChatTransceiverPlugin() = default;
  ~ChatTransceiverPlugin() override = default;

  // These are the functions derived from base class InternalsPlugin
  // that can be implemented.
  void Startup( long version ) override;  // game startup
  void Shutdown() override;               // game shutdown

  void EnterRealtime() override;          // entering realtime
  void ExitRealtime() override;           // exiting realtime

  void StartSession() override;           // session has started
  void EndSession() override;             // session has ended
 
  // Methods
  bool WantsToDisplayMessage( MessageInfoV01 &msg_info) override;
 private:
  double m_et_;  // needed for the hardware example
  bool m_enabled_; // needed for the hardware example
  bool displayed_welcome_message_ = false;
 
  // Shared Memory
  HANDLE h_map_file_;
  std::string last_message_;

  // Methods
  void open_shared_memory();
  void close_shared_memory() const;
  std::string read_shared_memory() const;
  bool update_from_shared_memory(std::string& message, unsigned char& destination) const;
};


#endif // _INTERNALS_EXAMPLE_H
