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
    };
  },

  methods: {
		reverse() {
			const vm = this;
			const clock = this.espModules.clock;

      fetch(`http://${clock.ipAddress}:${clock.port}?reverse=on`)
				.then(resp => resp.json())
				.then(apiObj => {
					if(apiObj){
						console.log('reverse - data from clock', apiObj);
					}
				});
    },
		fastForward() {
			const vm = this;
			const clock = this.espModules.clock;

      fetch(`http://${clock.ipAddress}:${clock.port}?fastForward=on`)
				.then(resp => resp.json())
				.then(apiObj => {
					if(apiObj){
						console.log('fastForward - data from clock', apiObj);
						vm.currentTime = apiObj.ticksCompleted;
					}
				});
    },
		normal() {
			const vm = this;
			const clock = this.espModules.clock;

      fetch(`http://${clock.ipAddress}:${clock.port}?normal=on`)
				.then(resp => resp.json())
				.then(apiObj => {
					if(apiObj){
						console.log('normal - data from clock', apiObj);
						vm.currentTime = apiObj.ticksCompleted;
					}
				});
    },
		reset() {
			const vm = this;
			const clock = this.espModules.clock;

      fetch(`http://${clock.ipAddress}:${clock.port}?reset=on`)
				.then(resp => resp.json())
				.then(apiObj => {
					if(apiObj){
						console.log('normal - data from clock', apiObj);
						vm.currentTime = apiObj.ticksCompleted;
					}
				});
    },
  },
});
