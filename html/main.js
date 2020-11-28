const app = Vue.createApp({
    data() {
        return {
            buttonState: 'Checking...',
            buttonAction: null,
            buttonText: '...',
            polling: null
        }
    },
    methods: {
        onButtonClick() {
            if (this.buttonAction == null)
            {
                return;
            }
            const formData = new FormData();
            formData.append('state', this.buttonAction)
           
            this.buttonState = this.buttonAction == 'ON' ? 'Turning on...' : 'Turning off...';
            this.buttonText = '...';
            this.buttonAction = null;

            axios.
                post('/state', formData).
                then(response => this.getState()).
                catch(error => { alert('failed to change power state'); this.getState() });
        },
        gotState (state) {
            if (state == 'OFF') {
                this.buttonState = 'Switch Off';
                this.buttonAction = 'ON';
                this.buttonText = 'On';
            } else if (state == 'ON') {
                this.buttonState = 'Switch On';
                this.buttonAction = 'OFF';
                this.buttonText = 'Off';
            } else {
                console.log('Got unexpected power state "' + state + '"')
                this.buttonState = 'Unknown power state';
                this.buttonAction = null;
                this.buttonText = '...';
            }
        },
        getState() {
            axios.
                get('/state').
                then(response => this.gotState(response.data.toUpperCase())).
                catch(error => console.log('Failed to get power state: ' + error));
        }
    },
    mounted () {
        this.getState();
        this.polling = setInterval(this.getState, 1000);
    },
    beforeUnmount() {
        clearInterval(this.polling);
    }
})