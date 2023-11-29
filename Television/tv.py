# Video Imports
import threading
import vlc
import keyboard

# Centeralize the directory where the videos are so its easy to change if we need to
VIDEO_DIR = "C:/Users/EscapeY2K/Videos/EscapeY2K"
SOUND_DIR = "C:/Users/EscapeY2K/Sounds/EscapeY2K"

# Creating a vlc instance
vlc_instance = vlc.Instance(["--sub-source=marq"])

# Media
dark = vlc_instance.media_new(f'{VIDEO_DIR}/dark.mp4')

timetravel = vlc_instance.media_new(f'{VIDEO_DIR}/timetravel.mp4')
timetravel_sound = f"{SOUND_DIR}/timetravel.mp3"

game = vlc_instance.media_new(f"{VIDEO_DIR}/game.mp4")
midnight = vlc_instance.media_new(f"{VIDEO_DIR}/midnight.mp4")
seek = vlc_instance.media_new(f"{VIDEO_DIR}/seek.mp4")
monster = vlc_instance.media_new(f"{VIDEO_DIR}/monster.mp4")
ending = vlc_instance.media_new(f"{VIDEO_DIR}/ending.mp4")

# The order of the playlist is crucial
# ALWAYS MAKE SURE THAT 'game' IS BEFORE 'midnight'
playlist = vlc_instance.media_list_new([game, midnight, dark, timetravel, seek, monster, ending])
    
# Game timer setup
BEGINNING_TIME = 600
SECOND_UPDATE = 1
DEPLETION_UPDATE = 0.2
    
# Creating a media list player
list_player = vlc.MediaListPlayer(vlc_instance)
list_player.set_media_list(playlist)

# Setting up regular media player
media_player = list_player.get_media_player()
media_player.toggle_fullscreen()
media_player.video_set_marquee_int(vlc.VideoMarqueeOption.Size, 100)  # pixels
media_player.video_set_marquee_int(vlc.VideoMarqueeOption.Position, 8) # 0=center, 1=left, 2=right, 4=top, 8=bottom
media_player.video_set_marquee_int(vlc.VideoMarqueeOption.Color, 0xff0000)
media_player.video_set_marquee_int(vlc.VideoMarqueeOption.Timeout, 0)  # millisec, 0==forever

# media_player.video_set_aspect_ratio("16:9")

def play_video(video):
    list_player.play_item(video)

def switch_video(video, hotkey, videoRepeats):
    play_video(timetravel)
    list_player.set_playback_mode(vlc.PlaybackMode().repeat)
    while keyboard.is_pressed(hotkey): pass
    play_video(video)
    if (not videoRepeats): list_player.set_playback_mode(vlc.PlaybackMode().default)
    
def update_timer():
    global game_timer
    global timer_update
    game_timer -= 1
    timer_string = str(int(game_timer / 60)) + ':' + "{0:0=2d}".format(game_timer % 60)
    media_player.video_set_marquee_string(vlc.VideoMarqueeOption.Text, vlc.str_to_bytes(timer_string))
    if (game_timer > 0):
        threading.Timer(timer_update, update_timer).start()
    
def start_timer():
    stop_timer() # Stop a timer if it is currently running
    global game_timer
    global timer_update
    game_timer = BEGINNING_TIME + 1
    timer_update = SECOND_UPDATE
    media_player.video_set_marquee_int(vlc.VideoMarqueeOption.Enable, 1)
    update_timer()
    
def stop_timer():
    global game_timer
    global starting_thread_count
    game_timer = 0
    media_player.video_set_marquee_int(vlc.VideoMarqueeOption.Enable, 0)
    while (threading.active_count() > starting_thread_count): pass # Wait for threads to close
    
def add_minute():
    global game_timer
    game_timer += 60
    
def deplete_timer(hotkey):
    global timer_update
    timer_update = DEPLETION_UPDATE
    while keyboard.is_pressed(hotkey): pass
    timer_update = SECOND_UPDATE
    
# Play the video
list_player.play_item(dark)
list_player.set_playback_mode(vlc.PlaybackMode().repeat)

keyboard.add_hotkey('d', switch_video, args = (dark, 'd', True))
keyboard.add_hotkey('g', switch_video, args = (game, 'g', False))
keyboard.add_hotkey('m', switch_video, args = (monster, 'm', True))
keyboard.add_hotkey('f', switch_video, args = (midnight, 'f', False))
keyboard.add_hotkey('s', switch_video, args = (seek, 's', False))
keyboard.add_hotkey('e', switch_video, args = (ending, 'e', False))
keyboard.add_hotkey('w', switch_video, args = (timetravel, 'w', True))

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

keyboard.add_hotkey('{', start_timer)
keyboard.add_hotkey('}', stop_timer)
keyboard.add_hotkey('+', add_minute)
keyboard.add_hotkey('-', deplete_timer, args = ('-'))

starting_thread_count = threading.active_count()

while (not keyboard.is_pressed('esc')):
    # The midnight video needs to loop further into the video
    if (media_player.get_media().get_mrl() == midnight.get_mrl()):
        if (media_player.get_position() > 0.98): media_player.set_position(0.1)
    elif (media_player.get_media().get_mrl() == seek.get_mrl()):
        if (media_player.get_position() > 0.5): switch_video(game, 'g', False)