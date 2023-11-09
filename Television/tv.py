import sys
import vlc
import keyboard
 
# Creating a vlc instance
vlc_instance = vlc.Instance()
 
# Media
dark = vlc.Media("C:/Users/EscapeY2K/Videos/EscapeY2K/dark.mp4")

timetravel = vlc.Media("C:/Users/EscapeY2K/Videos/EscapeY2K/timetravel.mp4")
timetravel_sound = "C:/Users/EscapeY2K/Videos/EscapeY2K/timetravel.mp3"

game = vlc.Media("C:/Users/EscapeY2K/Videos/EscapeY2K/game.mp4")
midnight = vlc.Media("C:/Users/EscapeY2K/Videos/EscapeY2K/midnight.mp4")
monster = vlc.Media("C:/Users/EscapeY2K/Videos/EscapeY2K/monster.mp4")
ending = vlc.Media("C:/Users/EscapeY2K/Videos/EscapeY2K/ending.mp4")

# The order of the playlist is crucial
# ALWAYS MAKE SURE THAT 'game' IS BEFORE 'midnight'
playlist = vlc.MediaList([game, midnight, dark, timetravel, monster, ending])
    
# Creating a media list player
list_player = vlc.MediaListPlayer(vlc_instance)
list_player.set_media_list(playlist)

# Setting up regular media player
media_player = list_player.get_media_player()
media_player.toggle_fullscreen()
# media_player.video_set_aspect_ratio("16:9")

def play_video(video):
    list_player.play_item(video)

def switch_video(video, hotkey, videoRepeats):
    play_video(timetravel)
    list_player.set_playback_mode(vlc.PlaybackMode().repeat)
    while keyboard.is_pressed(hotkey): pass
    play_video(video)
    if (not videoRepeats): list_player.set_playback_mode(vlc.PlaybackMode().default)
    
# Play the video
list_player.play_item(dark)
list_player.set_playback_mode(vlc.PlaybackMode().repeat)

keyboard.add_hotkey('d', switch_video, args = (dark, 'd', True))
keyboard.add_hotkey('g', switch_video, args = (game, 'g', False))
keyboard.add_hotkey('m', switch_video, args = (monster, 'm', True))
keyboard.add_hotkey('f', switch_video, args = (midnight, 'f', False))
keyboard.add_hotkey('e', switch_video, args = (ending, 'e', False))

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

while (not keyboard.is_pressed('esc')):
    # The midnight video needs to loop further into the video
    if (media_player.get_media().get_mrl() == midnight.get_mrl()):
        if (media_player.get_position() > 0.98): media_player.set_position(0.1)
