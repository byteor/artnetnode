import { h, Component } from 'preact';
import Button from 'preact-material-components/Button';
import 'preact-material-components/Button/style.css';
import TextField from 'preact-material-components/TextField';
import 'preact-material-components/TextField/style.css';

import style from './style';

export default class Network extends Component {
	state = {
		host: {
			name: 'aaa'
		}
	};

	render() {
		return (
			<div class={`${style.home} page`}>
				<h3>Network Settings</h3>
				<TextField label="Host name" value={this.state.host.name} />
				<Button raised>Save</Button>
			</div>
		);
	}
}
