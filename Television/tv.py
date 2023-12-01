# Server Imports
from server import InitiateServer

# Video Imports
import threading
import vlc
import keyboard

# Other rando imports
from multiprocessing import Process, Queue, Pipe

# Centeralize the directory where the videos are so its easy to change if we need to
VIDEO_DIR = "C:/Users/EscapeY2K/Videos/EscapeY2K"
SOUND_DIR = "C:/Users/EscapeY2K/Sounds/EscapeY2K"

# Media
dark = vlc.Media(f'{VIDEO_DIR}/dark.mp4')

timetravel = vlc.Media(f'{VIDEO_DIR}/timetravel.mp4')
timetravel_sound = f"{SOUND_DIR}/timetravel.mp3"

game = vlc.Media(f"{VIDEO_DIR}/game.mp4")
midnight = vlc.Media(f"{VIDEO_DIR}/midnight.mp4")
seek = vlc.Media(f"{VIDEO_DIR}/seek.mp4")
monster = vlc.Media(f"{VIDEO_DIR}/monster.mp4")
ending = vlc.Media(f"{VIDEO_DIR}/ending.mp4")

# The order of the playlist is crucial
# MAKE SURE 'game' IS ALWAYS BEFORE 'midnight'
playlist = vlc.MediaList([game, midnight, dark, timetravel, seek, monster, ending])
    
# Game timer setup
BEGINNING_TIME = 600
SECOND_UPDATE = 1
DEPLETION_UPDATE = 0.2
    
def play_video(video):
    global timer_update
    if (video == midnight or video == monster):
        timer_update = DEPLETION_UPDATE
    else:
        timer_update = SECOND_UPDATE
        
    list(map(lambda list_player: list_player.play_item(video), list_players))
    list(map(lambda list_player: set_loop(list_player, video != game), list_players))
    
def set_loop(player, shouldLoop):
    if (shouldLoop):
        player.set_playback_mode(vlc.PlaybackMode().repeat)
    else:
        player.set_playback_mode(vlc.PlaybackMode().default)
    
def set_video_game_time(parent_conn):
    isGamePlaying = (list_players[0].get_media_player().get_mrl()) == game.get_mrl()
    if(isGamePlaying):
        rotaryCounter = parent_conn.recv()
        list(map(lambda list_player: list_player.get_media_player().set_time(rotaryCounter*2000), list_players))
    
def update_timer():
    global game_timer
    global timer_update
    game_timer -= 1
    timer_string = str(int(game_timer / 60)) + ':' + "{0:0=2d}".format(game_timer % 60)
    list(map(lambda list_player: list_player.get_media_player().video_set_marquee_string(vlc.VideoMarqueeOption.Text, vlc.str_to_bytes(timer_string)), list_players))
    if (game_timer > 0):
        threading.Timer(timer_update, update_timer).start()
    
def start_timer():
    stop_timer() # Stop a timer if it is currently running
    global game_timer
    global timer_update
    game_timer = BEGINNING_TIME + 1
    timer_update = SECOND_UPDATE
    list(map(lambda list_player: list_player.get_media_player().video_set_marquee_int(vlc.VideoMarqueeOption.Enable, 1), list_players))
    update_timer()
    
def stop_timer():
    global game_timer
    global starting_thread_count
    game_timer = 0
    list(map(lambda list_player: list_player.get_media_player().video_set_marquee_int(vlc.VideoMarqueeOption.Enable, 0), list_players))
    while (threading.active_count() > starting_thread_count): pass # Wait for threads to close
    
def add_minute():
    global game_timer
    game_timer += 60
    
def deplete_timer(hotkey):
    while keyboard.is_pressed(hotkey): pass
    
# Make sure we have a main function :)
if __name__ == '__main__':
    # Creating a vlc instance
    tv_instance = vlc.Instance(["--sub-source=marq"])
    monitor_instance = vlc.Instance(["--sub-source=marq"])

    # Creating a media list player
    list_players = [vlc.MediaListPlayer(tv_instance), vlc.MediaListPlayer(monitor_instance)]

    # Set up instances
    for list_player in list_players:
        list_player.set_media_list(playlist)

        # Setting up regular media player
        media_player = list_player.get_media_player()
        media_player.video_set_marquee_int(vlc.VideoMarqueeOption.Size, 100)  # pixels
        media_player.video_set_marquee_int(vlc.VideoMarqueeOption.Position, 8) # 0=center, 1=left, 2=right, 4=top, 8=bottom
        media_player.video_set_marquee_int(vlc.VideoMarqueeOption.Color, 0xff0000)
        media_player.video_set_marquee_int(vlc.VideoMarqueeOption.Timeout, 0)  # millisec, 0==forever

    list_players[0].get_media_player().video_set_aspect_ratio("16:9")
    # Other player must be manually set to fullscreen
    # Click and press enter at the same time (click a little before) on the window to toggle
    list_players[1].get_media_player().toggle_fullscreen()
    
    # Start playing
    play_video(dark)
    
    # SERVER STUFF
    parent_conn, child_conn = Pipe()
    serverProcess = Process(target=InitiateServer, args=(child_conn,))
    serverProcess.start()

    keyboard.add_hotkey('d', lambda: play_video(dark))
    keyboard.add_hotkey('g', lambda: play_video(game))
    keyboard.add_hotkey('m', lambda: play_video(monster))
    keyboard.add_hotkey('f', lambda: play_video(midnight))
    keyboard.add_hotkey('s', lambda: play_video(seek))
    keyboard.add_hotkey('e', lambda: play_video(ending))
    keyboard.add_hotkey('w', lambda: play_video(timetravel))

    keyboard.add_hotkey('u', lambda: set_video_game_time(parent_conn))
    keyboard.add_hotkey('0', lambda: list(map(lambda list_player: list_player.get_media_player().set_position(0), list_players)))
    keyboard.add_hotkey('1', lambda: list(map(lambda list_player: list_player.get_media_player().set_position(0.1), list_players)))
    keyboard.add_hotkey('2', lambda: list(map(lambda list_player: list_player.get_media_player().set_position(0.2), list_players)))
    keyboard.add_hotkey('3', lambda: list(map(lambda list_player: list_player.get_media_player().set_position(0.3), list_players)))
    keyboard.add_hotkey('4', lambda: list(map(lambda list_player: list_player.get_media_player().set_position(0.4), list_players)))
    keyboard.add_hotkey('5', lambda: list(map(lambda list_player: list_player.get_media_player().set_position(0.5), list_players)))
    keyboard.add_hotkey('6', lambda: list(map(lambda list_player: list_player.get_media_player().set_position(0.6), list_players)))
    keyboard.add_hotkey('7', lambda: list(map(lambda list_player: list_player.get_media_player().set_position(0.7), list_players)))
    keyboard.add_hotkey('8', lambda: list(map(lambda list_player: list_player.get_media_player().set_position(0.8), list_players)))
    keyboard.add_hotkey('9', lambda: list(map(lambda list_player: list_player.get_media_player().set_position(0.9), list_players)))

    keyboard.add_hotkey('{', start_timer)
    keyboard.add_hotkey('}', stop_timer)
    keyboard.add_hotkey('+', add_minute)

    starting_thread_count = threading.active_count()

    check_player = list_players[0].get_media_player()
    while (not keyboard.is_pressed('esc')):
        # The midnight video needs to loop further into the video
        current_mrl = check_player.get_media().get_mrl()
        if (current_mrl == midnight.get_mrl()):
            if (check_player.get_position() > 0.98): list(map(lambda list_player: list_player.get_media_player().set_position(0.1), list_players))
        elif (current_mrl == seek.get_mrl()):
            if (check_player.get_position() > 0.5): play_video(game)
        elif (current_mrl == ending.get_mrl()):
            if (check_player.get_position() > 0.99): play_video(dark)
              
    serverProcess.terminate()