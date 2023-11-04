const tapeSongOptions = [
  { title: "Kyle First Song", value: 1 },
  { title: "Kyle Second Song", value: 2 },
  { title: "Space - Jvke", value: 3 },
  { title: "RICK ROLL", value: 4 }
];

const vue = new Vue({
  el: "#vue-root",
  data() {
    return {
      //ESP Modules
      espModules: {
        clock: {
          ipAddress: "192.168.1.50",
          port: "1234",
          status: "Starting",
          loading: false,
        },
        lockbox: {
          ipAddress: "192.168.1.211",
          port: "1234",
          status: "Closed",
          loading: false,
        },
        modules: [
          { //This is essentially just a module for test
            ipAddress: "10.0.0.225",
            port: "1234",
            status: "Off",
            loading: false,
          }
        ],
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
          loading: false
        }
      },

      //Tape Player extras
      tapeSongOptions: tapeSongOptions,
      selectedSong: 1,

      //Clock Stuff
      currentTime: 0,
      adminMode: false,

      //General Stuff
      loading: false,
    };
  },

  watch: {
    adminMode(newMode) {
      const onOrOff = newMode ? "on" : "off";
      this.changeClockMode("manualOverride", onOrOff);
    },
  },

  methods: {
    sendESPMessage(module, message, state){
      console.log('sending message to: ', module);
      const vm = this;
      const esp = this.espModules[module];
      console.log(esp);
      // if(!state){
      //   state = this.selectedSong;
      // }

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
    sendLockboxMessage(message, state) {
      const vm = this;
      this.loading = true;
      const lockbox = this.espModules.lockbox;

      fetch(`http://${lockbox.ipAddress}:${lockbox.port}?${message}=${state}`)
        .then((resp) => resp.json())
        .then((apiObj) => {
          if (apiObj) {
            if (apiObj.error) {
              console.log("An unexpected error occured.");
            }

            console.log("Response from lockbox", apiObj);
            vm.loading = false;
          }
        });
    },
    sendModuleMessage(moduleNum, message, state){
      console.log(moduleNum, message, state);
      const vm = this;
      this.loading = true;
      const module = this.espModules.modules[moduleNum];
      module.loading = true;

      fetch(`http://${module.ipAddress}:${module.port}?${message}=${state}`)
        .then((resp) => resp.json())
        .then((apiObj) => {
          if (apiObj) {
            if (apiObj.error) {
              console.log("An unexpected error occured.");
            }

            console.log("Response from lockbox", apiObj);
            vm.loading = false;
            module.loading = false;
          }
        });
    },
    checkClockStatus(){
      const vm = this;
      this.loading = true;
      const clock = this.espModules.clock;

      fetch(`http://${clock.ipAddress}:${clock.port}?${mode}=${onOrOff}`)
        .then((resp) => resp.json())
        .then((apiObj) => {
          if (apiObj) {
            if (apiObj.error) {
              console.log("An unexpected error occured.");
            }

            vm.clockOperation = apiObj.mode ?? "Undefined";
            vm.loading = false;
          }
        });
    },
    changeClockMode(mode, onOrOff) {
      const vm = this;
      this.loading = true;
      const clock = this.espModules.clock;

      fetch(`http://${clock.ipAddress}:${clock.port}?${mode}=${onOrOff}`)
        .then((resp) => resp.json())
        .then((apiObj) => {
          if (apiObj) {
            if (apiObj.error) {
              console.log("An unexpected error occured.");
            }

            vm.clockOperation = apiObj.mode ?? "Undefined";
            vm.loading = false;
          }
        });
    },
  },
});
