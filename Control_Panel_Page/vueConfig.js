const vue = new Vue({
  el: "#vue-root",
  data() {
    return {
      //ESP Modules
      espModules: {
				clock: {
          ipAddress: "10.0.0.121",
          port: "1234",
          name: "ONLY RECEIVE",
        }
			},

      //Clock Stuff
      currentTime: 0,
      clockOperation: "none",
			adminMode: false,

			//General Stuff
	  	loading: false
    };
  },

	watch: {
		adminMode(newMode){
			const onOrOff = newMode ? 'on' : 'off';
			this.changeClockMode('manualOverride', onOrOff);
		}
	},

  methods: {
		changeClockMode(mode, onOrOff){
			const vm = this;
			this.loading = true;
			const clock = this.espModules.clock;

      fetch(`http://${clock.ipAddress}:${clock.port}?${mode}=${onOrOff}`)
				.then(resp => resp.json())
				.then(apiObj => {
					if(apiObj){
						if(apiObj.error){
							console.log("An unexpected error occured.");
						}

						vm.clockOperation = apiObj.mode ?? "Undefined";
						vm.loading = false;
					}
				});
		},
  },
});
