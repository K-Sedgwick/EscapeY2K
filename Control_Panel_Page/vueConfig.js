const tapeSongOptions = [
  { title: "Kyle First Song", value: 1 },
  { title: "Kyle Second Song", value: 2 },
  { title: "Space - Jvke", value: 3 },
  { title: "RICK ROLL", value: 4 },
];

const SECONDS_UNTIL_STATUS_REFRESH = 5;

const vue = new Vue({
  el: "#vue-root",
  data() {
    return {
      // This is the status of the room, tracked by the Mini PC
      status: {
        selectedSong: 1,
        currentTime: 0,
        clockmode: "Unknown",
      },
      //ESP Modules
      espModules: {
        clock: {
          ipAddress: "10.0.0.121", //10.0.0.121 at Jakes house, 192.168.1.50 on EscapeY2K
          port: "1234",
          status: "Starting",
          loading: false,
        },
        lockbox: {
          ipAddress: "192.168.1.127", //192.168.1.127 on EscapeY2K
          port: "1234",
          status: "Closed",
          loading: false,
        },
        testModule: {
          //This is essentially just a module for test
          ipAddress: "10.0.0.225",
          port: "1234",
          status: "Off",
          loading: false,
        },
        tapePlayer: {
          ipAddress: "10.0.0.197", //10.0.0.197 at Jakes house, 192.168.1.211 on the escapey2k wifi
          port: "1234",
          status: "Paused",
          loading: false,
        },
        dialPuzzle: {
          ipAddress: "10.0.0.225", //10.0.0.225, 192.168.1.225
          port: "1234",
          status: "LEDS OFF",
          loading: false,
        },
        pingModule: {
          ipAddress: "10.0.0.94", // 10.0.0.94
          port: "1234",
          status: "pinging, I guess",
          loading: false,
        },
        miniPC: {
          ipAddress: "10.0.0.64",
          port: "8001",
          status: "Off",
          loading: false,
        },
        irReceiver: {
          ipAddress: "10.0.0.181",
          port: "1234",
          status: "Off",
          loading: false,
        },
      },

      //Tape Player extras
      tapeSongOptions: tapeSongOptions,

      //This is just in relation to the clock
      adminMode: false,

      //General Stuff
      loading: false,

      //Status Stuff
      statusLoading: false,
      timeUntilStatusRefresh: SECONDS_UNTIL_STATUS_REFRESH,
      statusInvervalRunning: false,
      statusIntervalTimer: null,
    };
  },

  watch: {
    adminMode(newMode) {
      const onOrOff = newMode ? "on" : "off";
      this.changeClockMode("manualOverride", onOrOff);
    },
    statusInvervalRunning(newStatus) {
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
    },
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

            console.log(`Response from ${module}`, apiObj);
            esp.loading = false;
          }
        });
    },
    sendESPMessage(module, message, state) {
      const vm = this;
      const esp = this.espModules[module];

      fetch(`http://${esp.ipAddress}:${esp.port}?${message}=${state}`)
        .then((resp) => resp.json())
        .then((apiObj) => {
          if (apiObj) {
            if (apiObj.error) {
              console.log("An unexpected error occured.");
            }

            console.log(`Response from ${module}`, apiObj);
            vm.loading = false;
          }
        });
    },
    getRoomStatus() {
      if (this.timeUntilStatusRefresh <= 0) {
        const vm = this;
        const esp = this.espModules.miniPC;
        this.statusLoading = true;

        //SIMULATE API CALL
        setTimeout(() => {
          vm.statusLoading = false;
          vm.timeUntilStatusRefresh = SECONDS_UNTIL_STATUS_REFRESH + 1;
        }, 2000);

        // fetch(`http://${esp.ipAddress}:${esp.port}/status`)
        //   .then((resp) => resp.json())
        //   .then((apiObj) => {
        //     if (apiObj) {
        //       if (apiObj.error) {
        //         console.log("An unexpected error occured.");
        //       }

        //       vm.status = apiObj.data;
        //       vm.statusLoading = false;
        //     }
        //   })
        //   .finally(() => vm.timeUntilStatusRefresh = SECONDS_UNTIL_STATUS_REFRESH);
      } else if (this.timeUntilStatusRefresh > 0) {
        this.timeUntilStatusRefresh--;
      }
    },
    runTest() {
      const lockbox = this.espModules.lockbox;
      console.log("lockbox", lockbox);
    },
  },
});
