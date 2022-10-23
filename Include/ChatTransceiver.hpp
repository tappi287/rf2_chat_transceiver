#ifndef _INTERNALS_EXAMPLE_H
#define _INTERNALS_EXAMPLE_H

#include <string>
#include <windows.h>
#include <conio.h>

#include "InternalsPlugin.hpp"

#undef ENABLE_LOG
#define LOG_FILE "UserData\\Log\\ChatTranceiver.log"

// This is used for the app to use the plugin for its intended purpose
class ChatTransceiverPlugin : public InternalsPluginV07  // REMINDER: exported function GetPluginVersion() should return 1 if you are deriving from this InternalsPluginV01, 2 for InternalsPluginV02, etc.
{
 public:
  // Constructor/destructor
  ChatTransceiverPlugin();
  ~ChatTransceiverPlugin() override;

  // These are the functions derived from base class InternalsPlugin
  // that can be implemented.
  void Startup( long version ) override;  // game startup
  void Shutdown() override;               // game shutdown

  void EnterRealtime() override;          // entering realtime
  void ExitRealtime() override;           // exiting realtime

  void StartSession() override;           // session has started
  void EndSession() override;             // session has ended
 
  // Methods
  bool WantsToDisplayMessage( MessageInfoV01 &msgInfo) override;
 private:
  double m_et_{};
  bool m_enabled_{};
  bool inside_realtime_ = false;
  bool displayed_welcome_message_ = false;
 
  // Shared Memory
  HANDLE h_map_file_{};
  std::string last_message_{};

  // Methods
  void open_shared_memory();
  void close_shared_memory() const;
  void clear_shared_memory() const;
  std::string read_shared_memory() const;
  bool update_from_shared_memory(std::string& message, unsigned char& destination) const;
  static void write_log(const char * const msg);
};


#endif // _INTERNALS_EXAMPLE_H
