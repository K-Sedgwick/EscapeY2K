const vue = new Vue({
    el: '#vue-root',
    data(){
        return {
            //ESP Modules
            espModules: [
                {
                    ipAddress: '10.0.0.255', 
                    port: '1234',
                    name: 'ONLY RECEIVE'
                }
            ],

            //Clock Stuff
            currentTime: 0,
            clockOperation: 'none',
        }
    }
});