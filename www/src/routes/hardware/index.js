import { h, Component } from 'preact';
import TextField from 'preact-material-components/TextField';
import 'preact-material-components/TextField/style.css';
import { view } from 'preact-easy-state';

import store from '../../service/store';
import style from './style.css';

class Hardware extends Component {
  render() {
    return (
      <div class={`${style.home} page`}>
        <h3>Network Settings</h3>
        <TextField label="Host name" value={store.config.host} onChange={ev => (store.config.host = ev.target.value)} />
        <h3>Hardware Settings</h3>
        <h5>
          <span class={style.warning}>WARNING:</span>&nbsp;
		  incorrect settings may cause a physical damage to your device
          or represent a fire hazard
        </h5>
        <TextField
          label="PWM Frequency, Hz"
          value={store.config.hw.freq}
          onChange={ev => (store.config.hw.freq = ev.target.value)}
        />
      </div>
    );
  }
}

export default view(Hardware);