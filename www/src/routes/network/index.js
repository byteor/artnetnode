import { h, Component } from 'preact';
import { view } from 'preact-easy-state';
import style from './style.css';

class Network extends Component {
  render() {
    return (
      <div class={`${style.home} page`}>
        <h3>Network Settings</h3>
      </div>
    );
  }
}

export default view(Network);
