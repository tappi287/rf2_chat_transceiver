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

// Constants
bool displayed_message = false;
bool inside_realtime = false;


void ChatTransceiverPlugin::Startup( long version )
{
  char temp[80];
  sprintf( temp, "-STARTUP- (version %.3f)", (float) version / 1000.0f );

  // default HW control enabled to true
  mEnabled = true;
}


void ChatTransceiverPlugin::Shutdown()
{
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


bool ChatTransceiverPlugin::WantsToDisplayMessage( MessageInfoV01 &msg_info)
{
  if(! inside_realtime)
    return( false );
  
  if (! displayed_message)
  {
    msg_info.mDestination = 0;
    msg_info.mTranslate = 0;
    sprintf(msg_info.mText, "ChatTransceiverPlugIn example text message.");
    displayed_message = true;
    
    return ( true ); 
  }
  if (! next_message.empty())
  {
    msg_info.mDestination = 0;
    msg_info.mTranslate = 0;
    sprintf(msg_info.mText, next_message.c_str());
    next_message = "";
    
    return ( true ); 
  }
  return ( false );
}

extern "C" __declspec( dllexport )
bool __cdecl add_message(const char* chr_ptr)
{
  const std::string message = chr_ptr;
  std::cout << "Received:" << message.c_str();
  
  return true;
}