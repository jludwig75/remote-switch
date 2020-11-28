const app = Vue.createApp({
    data() {
        return {
            buttonImages: {
                'ON': '/power_on.png',
                'OFF': '/power_off.png',
                null: '/power_disabled.png'
            },
            buttonState: null,
            buttonStatusMessage: 'Checking...',
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
           
            this.buttonStatusMessage = this.buttonAction == 'ON' ? 'Turning switch on...' : 'Turning switch off...';
            this.buttonText = '...';
            this.buttonAction = null;

            axios.
                post('/state', formData).
                then(response => this.getState()).
                catch(error => { alert('failed to change power state'); this.getState() });
        },
        gotState (state) {
            this.buttonState = state.toUpperCase();
            if (state == 'OFF') {
                this.buttonStatusMessage = 'Switch is off';
                this.buttonAction = 'ON';
                this.buttonText = 'On';
            } else if (state == 'ON') {
                this.buttonStatusMessage = 'Switch is on';
                this.buttonAction = 'OFF';
                this.buttonText = 'Off';
            } else {
                console.log('Got unexpected power state "' + state + '"')
                this.buttonStatusMessage = 'Unknown power state';
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
    computed: {
        buttonImage() {
            if (!(this.buttonState in this.buttonImages)) {
                return this.buttonImages[null];
            }

            return this.buttonImages[this.buttonState];
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