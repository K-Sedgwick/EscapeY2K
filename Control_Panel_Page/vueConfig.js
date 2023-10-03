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
      },

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
