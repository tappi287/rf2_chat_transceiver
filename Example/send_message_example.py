import ctypes
import time
from multiprocessing import shared_memory
from pathlib import Path, WindowsPath

SHARED_MEMORY_NAME = "rF2_ChatTransceiver_SM"
PLUGIN_NAME = "ChatTransceiver"
PLUGIN_LOCATION = r'C:\SteamLibrary\steamapps\common\rFactor 2\Bin64\Plugins'
PLUGIN_PATH = Path(PLUGIN_LOCATION) / f'{PLUGIN_NAME}.dll'


def _prepare_message(message: str, destination: int=0) -> bytes:
    """ Prepare and encode message to be sent

    :param message: Message String to send
    :param destination: 0 - message center, 1 - multiplayer chat
    :return:
    """
    # Note that encoding could raise exceptions with non-european languages
    return f'{destination}{message}'.encode('cp1252')


def call_c_lib_method(message: str):
    if not PLUGIN_PATH.exists() and not PLUGIN_PATH.is_file():
        return

    # load c lib
    plugin_lib = ctypes.cdll.LoadLibrary(str(WindowsPath(PLUGIN_PATH)))

    # define c method
    send_message = plugin_lib.send_message
    send_message.argtypes = [ctypes.c_char_p]
    send_message.restype = ctypes.c_bool

    mem_bytes = _prepare_message(message[:128])

    # call c method
    result = send_message(ctypes.c_char_p(mem_bytes))

    # should be true if sim was running, plugin loaded and car on track
    print("Result:", result)


def use_py_shared_memory(message):
    """ This method requires Python >= 3.8 """
    # open shared memory
    try:
        shm = shared_memory.SharedMemory(SHARED_MEMORY_NAME, create=False)
    except FileNotFoundError:
        return

    # make sure message does not exceed message size
    mem_bytes = _prepare_message(message[:128])

    # write to shared memory
    shm.buf[:len(mem_bytes)] = mem_bytes

    # close shared memory handle
    shm.close()


if __name__ == '__main__':
    c_message = "Hello World from clib - @äöüßéè#"
    call_c_lib_method(c_message)

    time.sleep(0.1)

    shm_message = "Hello World from shared memory - @äöüßéè#"
    use_py_shared_memory(shm_message)
