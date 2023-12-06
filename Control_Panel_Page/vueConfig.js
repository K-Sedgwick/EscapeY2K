const SECONDS_UNTIL_STATUS_REFRESH = 5;
const SOUND_EFFECTS = {
  Scream : 1,
  Footsteps : 2,
  "Monster Footsteps":3,
  "Scary Whispering":4
}
const LOCKBOXES = [
  { // PCB-1
    puzzleName:"dial",
    ip:"192.168.1.127:1234",
    status: "Unknown",
    loading: false
  },
  { // PCB-2   192.168.1.242 on EscapeY2K and 10.0.0.155 at Jakes house
    puzzleName:"bust",
    ip:"192.168.1.242:1234",
    status: "Unknown",
    loading: false
  }, 
  { // PCB-3
    puzzleName:"rando",
    ip:"192.168.1.150:1234",
    status: "Unknown",
    loading: false
  }, 
  { // PCB-4
    puzzleName:"plugboard",
    ip:"192.168.1.143:1234",
    status: "Unknown",
    loading: false
  }, 
  { // PCB-5
    puzzleName:"potentiometer",
    ip:"192.168.1.54:1234",
    status: "Unknown",
    loading: false
  }
]

const vue = new Vue({
  el: "#vue-root",
  data() {
    return {
      // This is the status of the room, tracked by the Mini PC
      status: {
        selectedTapeEffect: 1,
        selectedPhoneEffect: 1,
        currentTime: 0,
        clockmode: "Unknown",
      },
      //ESP Modules
      espModules: {
        clock: {
          ipAddress: "192.168.1.50", //10.0.0.121 at Jakes house, 192.168.1.50 on EscapeY2K
          port: "1234",
          status: "Starting",
          loading: false,
        },
        tapePlayer: {
          ipAddress: "192.168.1.212", //10.0.0.197 at Jakes house, 192.168.1.212 on the escapey2k wifi
          port: "1234",
          status: "Paused",
          loading: false,
        },
        phoneBase: {
          ipAddress: "192.168.1.151", //10.0.0.197 at Jakes house, 192.168.1.151 on the escapey2k wifi
          port: "1234",
          status: "Paused",
          loading: false,
        },
        dialPuzzle: {
          ipAddress: "192.168.1.225", //10.0.0.225, 192.168.1.225
          port: "1234",
          status: "LEDS OFF",
          loading: false,
        },
        miniPC: {
          ipAddress: "192.168.1.211", // 192.168.1.211
          port: "8001",
          status: "Off",
          loading: false,
        },
        irReceiver: {
          ipAddress: "192.168.1.181", //10.0.0.181 at Jakes house, 192.168.1.181 on EscapeY2K
          port: "1234",
          status: "Off",
          loading: false,
        },
        circlePlug: {
          ipAddress: "192.168.1.202", //idk at Jakes house, 192.168.1.202 on EscapeY2K
          port: "1234",
          status: "Off",
          loading: false,
        }
      },

      //This is just in relation to the clock
      adminMode: false,

      //General Stuff
      loading: false,
      resetting: false,
      lockboxes:LOCKBOXES,
      lockboxesToShow: [],
      lockboxesNotInRotation: [],
      puzzlesToBeSolved: [],
      solvedPuzzles: [],

      //Sound effect stuff
      soundEffects: SOUND_EFFECTS,
      soundEffectsLinked: false,

      //Status Stuff
      statusLoading: false,
      timeUntilStatusRefresh: SECONDS_UNTIL_STATUS_REFRESH,
      statusIntervalRunning: false,
      statusIntervalTimer: null,
    };
  },

  watch: {
    adminMode(newMode) {
      const onOrOff = newMode ? "on" : "off";
      this.changeClockMode("manualOverride", onOrOff);
    },
    statusIntervalRunning(newStatus) {
      //Yes, these if statements should be this way
      if (newStatus) {
        if (!this.statusIntervalTimer) {
          this.statusIntervalTimer = setInterval(this.getRoomStatus, 1000);
        }
      } else {
        clearInterval(this.statusIntervalTimer);
        this.statusIntervalTimer = null;
        this.timeUntilStatusRefresh = SECONDS_UNTIL_STATUS_REFRESH;
      }
    }
  },

  methods: {
    getESPPathData(module, path) {
      const vm = this;
      const esp = this.espModules[module];
      esp.loading = true;

      fetch(`http://${esp.ipAddress}:${esp.port}/${path}`)
        .then((resp) => resp.json())
        .then((apiObj) => {
          if (apiObj) {
            if (apiObj.error) {
              console.log("An unexpected error occured.");
            }

            // console.log(`Response from ${module}`, apiObj);
            esp.loading = false;
          }
        });
    },
    sendESPMessage(module, message) {
      const vm = this;
      const esp = this.espModules[module];

      fetch(`http://${esp.ipAddress}:${esp.port}/${message}`)
        .then((resp) => resp.json())
        .then((apiObj) => {
          if (apiObj) {
            if (apiObj.error) {
              console.log("An unexpected error occured.");
            }

            // console.log(`Response from ${module}`, apiObj);
            vm.loading = false;
          }
        });
    },
    playTapePlayerSoundEffect(){
      if(this.soundEffectsLinked === true){
        this.sendESPMessage("phoneBase", `?play=${this.status.selectedTapeEffect}`)
      }
      
      this.sendESPMessage("tapePlayer", `?play=${this.status.selectedTapeEffect}`)
    },
    playPhoneSoundEffect(){
      if(this.soundEffectsLinked === true){
        this.sendESPMessage("phoneBase", `?play=${this.status.selectedTapeEffect}`)
        this.sendESPMessage("tapePlayer", `?play=${this.status.selectedTapeEffect}`)
      }
      else{
        this.sendESPMessage("phoneBase", `?play=${this.status.selectedPhoneEffect}`)
      }
    },
    sendLockboxMessage(lockbox, message) {
      // console.log(lockbox, message)
      lockbox.loading = true;

      fetch(`http://${lockbox.ip}?${message}`)
        .then((resp) => resp.json())
        .then((apiObj) => {
          if (apiObj) {
            if (apiObj.error) {
              console.log("An unexpected error occured.");
            }
            else{
              // console.log(`Response from ${module}`, apiObj);
              lockbox.status = apiObj.status ?? "Unknown"
            }
          }
        }).finally(_ => lockbox.loading = false);
    },
    getRoomStatus() {
      if (this.timeUntilStatusRefresh <= 0) {
        const vm = this;
        const esp = this.espModules.miniPC;
        this.statusLoading = true;

        this.statusIntervalRunning = false;
        
        //SIMULATE API CALL
        // setTimeout(() => {
        //   vm.statusLoading = false;
        //   vm.timeUntilStatusRefresh = SECONDS_UNTIL_STATUS_REFRESH + 1;
        // }, 2000);

        fetch(`http://${esp.ipAddress}:${esp.port}/status`)
          .then((resp) => resp.json())
          .then((apiObj) => {
            if (apiObj) {
              if (apiObj.error) {
                console.log("An unexpected error occured.");
              }

              // console.log("apiObj status", apiObj.solved)
              vm.puzzlesToBeSolved = apiObj.puzzlesToSolve
              vm.solvedPuzzles = apiObj.solved ?? []
              vm.statusLoading = false;
            }
          })
          .finally(() => vm.statusIntervalRunning = true);
      } else if (this.timeUntilStatusRefresh > 0) {
        this.timeUntilStatusRefresh--;
      }
    },
    showResetRoomModal(){
      $(this.$refs.resetRoomModal).modal('show')
    },
    resetRoom(){
      const vm = this;
      const esp = this.espModules.miniPC;
      this.resetting = true;

      this.solvedPuzzles = []
      this.puzzlesToBeSolved = []

      fetch(`http://${esp.ipAddress}:${esp.port}/resetAndShuffle`)
      .then((resp) => resp.json())
      .then((apiObj) => {
        if (apiObj) {
          if (apiObj.error) {
            console.log("An unexpected error occured.");
          }

          // console.log('apiObj reset', apiObj)
          
          vm.puzzlesToBeSolved = apiObj.puzzlesToSolve
          vm.resetting = false;

          //Then sort the puzzles that are to be solved
          vm.buildLockboxArrayForUser()
        }
      })

      $(this.$refs.resetRoomModal).modal('hide')
    },
    buildLockboxArrayForUser(a, b){
      //First, build the array of lockboxes that are in this rotation
      this.lockboxesToShow = []
      this.puzzlesToBeSolved.forEach((el, index) => {
        // console.log(el)
        const lockbox = this.lockboxes.find(element => element.puzzleName == el)
        // lockbox.orderNum = index
        this.lockboxesToShow.push(lockbox)
      })
      //Then, build the array of boxes that are not in this rotation
      this.lockboxesNotInRotation = this.lockboxes.filter(box => !this.puzzlesToBeSolved.includes(box.puzzleName))
    },
    capitalizeFirstLetter(string) {
      return string.charAt(0).toUpperCase() + string.slice(1);
    }
  },
  mounted(){
    this.lockboxesNotInRotation = this.lockboxes.filter(box => !this.puzzlesToBeSolved.includes(box.puzzleName))
  }
});

document.vm = vue
