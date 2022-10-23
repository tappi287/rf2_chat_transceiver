## rFactor 2 Chat Transceiver Plugin
Plugin that allows to forward messages from external applications to the rF2 in-game message center and the multiplayer chat.


### Installation
- grab the [latest release](https://github.com/tappi287/rf2_chat_transceiver/releases/latest)
- copy the `ChatTransceiver.dll` to `rFactor 2\Bin64\Plugins`


#### Note to Users
This plugin won't do anything without a client application feeding it with messages.
For a Twitch chat to in-game message center bridge, try the [rF2-Settings-Widget](https://github.com/tappi287/rf2_video_settings/releases/latest). 


### Development
The plugin will open a Windows shared memory handle, read message and destination from it and erase the memory with zeros once the message was processed.

There is no messaging queue as the in-game message center will already queue the messages.

There are two methods you could use programmatically to trigger a message:
- call the dll method `send_message` and pass a `const char*` to it, take a look at the [source from line 330](/Source/ChatTransceiver.cpp#L331)
- access shared memory with name `rF2_ChatTransceiver_SM` and write characters is to it respecting Message Format

A Python client example can be found in [Examples](/Example/send_message_example.py)

#### Message Format
First character should be an unsigned char indicating the destination: 0 - message center, 1 - multiplayer chat. Followed by up to 128 characters.

Example: `0Hello World.` - will display `Hello World.` in the in-game message center.
