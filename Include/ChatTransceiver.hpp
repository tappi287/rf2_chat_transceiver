//���������������������������������������������������������������������������
//�                                                                         �
//� Module: Internals Example Header File                                   �
//�                                                                         �
//� Description: Declarations for the Internals Example Plugin              �
//�                                                                         �
//�                                                                         �
//� This source code module, and all information, data, and algorithms      �
//� associated with it, are part of CUBE technology (tm).                   �
//�                 PROPRIETARY AND CONFIDENTIAL                            �
//� Copyright (c) 1996-2014 Image Space Incorporated.  All rights reserved. �
//�                                                                         �
//�                                                                         �
//� Change history:                                                         �
//�   tag.2005.11.30: created                                               �
//�                                                                         �
//���������������������������������������������������������������������������

#ifndef _INTERNALS_EXAMPLE_H
#define _INTERNALS_EXAMPLE_H

#include <string>

#include "InternalsPlugin.hpp"


// This is used for the app to use the plugin for its intended purpose
class ChatTransceiverPlugin : public InternalsPluginV07  // REMINDER: exported function GetPluginVersion() should return 1 if you are deriving from this InternalsPluginV01, 2 for InternalsPluginV02, etc.
{

 public:

  // Constructor/destructor
  ChatTransceiverPlugin() {}
  ~ChatTransceiverPlugin() {}

  // These are the functions derived from base class InternalsPlugin
  // that can be implemented.
  void Startup( long version );  // game startup
  void Shutdown();               // game shutdown

  void EnterRealtime();          // entering realtime
  void ExitRealtime();           // exiting realtime

  void StartSession();           // session has started
  void EndSession();             // session has ended

  // Shared Memory
  std::string next_message;
 
  //
  bool WantsToDisplayMessage( MessageInfoV01 &msg_info);

 private:
  double mET;  // needed for the hardware example
  bool mEnabled; // needed for the hardware example
};


#endif // _INTERNALS_EXAMPLE_H
