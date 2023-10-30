import vlc
import keyboard
 
# Creating a vlc instance
vlc_instance = vlc.Instance()
 
# Media
earth = vlc_instance.media_new("C:/Users/EscapeY2K/Videos/Samples/Earth.mov")
rain = vlc_instance.media_new("C:/Users/EscapeY2K/Videos/Samples/Rain.mp4")
lions = vlc_instance.media_new("C:/Users/EscapeY2K/Videos/Samples/Lions.mp4")
yoga = vlc_instance.media_new("C:/Users/EscapeY2K/Videos/Samples/Yoga.mp4")

playlist = vlc.MediaList([earth, rain, lions, yoga])
    
# Creating a media list player
list_player = vlc.MediaListPlayer(vlc_instance)
list_player.set_media_list(playlist)

# Setting up regular media player
media_player = list_player.get_media_player()
media_player.toggle_fullscreen()
    
# Play the video
list_player.play()
list_player.set_playback_mode(vlc.PlaybackMode().repeat)

keyboard.add_hotkey('e', lambda: list_player.play_item_at_index(0))
keyboard.add_hotkey('r', lambda: list_player.play_item_at_index(1))
keyboard.add_hotkey('l', lambda: list_player.play_item_at_index(2))
keyboard.add_hotkey('y', lambda: list_player.play_item_at_index(3))

keyboard.add_hotkey('0', lambda: media_player.set_position(0))
keyboard.add_hotkey('1', lambda: media_player.set_position(0.1))
keyboard.add_hotkey('2', lambda: media_player.set_position(0.2))
keyboard.add_hotkey('3', lambda: media_player.set_position(0.3))
keyboard.add_hotkey('4', lambda: media_player.set_position(0.4))
keyboard.add_hotkey('5', lambda: media_player.set_position(0.5))
keyboard.add_hotkey('6', lambda: media_player.set_position(0.6))
keyboard.add_hotkey('7', lambda: media_player.set_position(0.7))
keyboard.add_hotkey('8', lambda: media_player.set_position(0.8))
keyboard.add_hotkey('9', lambda: media_player.set_position(0.9))

# Program exits when escape is pressed
keyboard.wait('esc')
list_player.stop()
