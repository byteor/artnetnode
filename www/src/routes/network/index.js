import { h, Component } from 'preact';
import Button from 'preact-material-components/Button';
import 'preact-material-components/Button/style.css';
import TextField from 'preact-material-components/TextField';
import 'preact-material-components/TextField/style.css';
import { view } from 'preact-easy-state';

import store from '../../service/store';
import style from './style';

class Network extends Component {
  render() {
    return (
      <div class={`${style.home} page`}>
        <h3>Network Settings</h3>
        <TextField
          label="Host name"
          value={store.config.host}
          onBlur={store.save}
          onChange={ev => (store.config.host = ev.target.value)}
        />
        <Button raised>Save</Button>
      </div>
    );
  }
}

export default view(Network);
